
#include <cuda.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include "nvEncodeAPI.h"

void* gdevice;
CUcontext cuContextCurr;

NVENCSTATUS InitCuda(uint32_t deviceID)
{
  CUresult cuResult;
  CUdevice device;

  int deviceCount = 0;
  int SMminor = 0, SMmajor = 0;

  cuResult = cuInit(0);
  if (cuResult != CUDA_SUCCESS)
  {
    std::cout << "cuInit error: " << cuResult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  cuResult = cuDeviceGetCount(&deviceCount);
  if (cuResult != CUDA_SUCCESS)
  {
    std::cout << "cuDeviceGetCount error: " << cuResult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  // If dev is negative value, we clamp to 0
  if ((int)deviceID < 0) deviceID = 0;

  if (deviceID > (unsigned int)deviceCount - 1)
  {
    std::cout << "Invalid Device Id = " << deviceID << std::endl;
    return NV_ENC_ERR_INVALID_ENCODERDEVICE;
  }

  cuResult = cuDeviceGet(&device, deviceID);
  if (cuResult != CUDA_SUCCESS)
  {
    std::cout << "cuDeviceGet error: " << cuResult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  cuResult = cuDeviceComputeCapability(&SMmajor, &SMminor, deviceID);
  if (cuResult != CUDA_SUCCESS)
  {
    std::cout << "cuDeviceComputeCapability error: " << cuResult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  if (((SMmajor << 4) + SMminor) < 0x30)
  {
    std::cout << "GPU " << deviceID
              << " does not have NVENC capabilities exiting\n"
              << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  cuResult = cuCtxCreate((CUcontext*)(&gdevice), 0, device);
  if (cuResult != CUDA_SUCCESS)
  {
    std::cout << "cuCtxCreate error: " << cuResult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  cuResult = cuCtxPopCurrent(&cuContextCurr);
  if (cuResult != CUDA_SUCCESS)
  {
    std::cout << "cuCtxPopCurrent error: " << cuResult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }
  return NV_ENC_SUCCESS;
}

int main(int argc, char** argv)
{
  NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS sessionparam;
  NV_ENC_INITIALIZE_PARAMS encinitparam;
  NV_ENC_PRESET_CONFIG presetcfg;
  NV_ENC_CONFIG encodecfg;
  NV_ENC_PIC_PARAMS encodepicparam;

  NVENCSTATUS nvstatus = NV_ENC_SUCCESS;
  void* encoder        = nullptr;
  void* inputbuffer    = nullptr;
  void* outputbuffer   = nullptr;

  NV_ENCODE_API_FUNCTION_LIST encodeAPI;  // = new NV_ENCODE_API_FUNCTION_LIST;

  if (argc < 2)
  {
    std::cout << "Not enought argument" << std::endl;
    return 1;
  }
  // Init cuda context and device
  InitCuda(0);

  // Create encode APIs
  encodeAPI.version = NV_ENCODE_API_FUNCTION_LIST_VER;
  nvstatus          = NvEncodeAPICreateInstance(&encodeAPI);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "NvEncodeAPICreateInstance failed" << std::endl;
    return 1;
  }

  // Start Encoding Session
  std::memset(&sessionparam, 0, sizeof(NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS));
  sessionparam.version    = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
  sessionparam.deviceType = NV_ENC_DEVICE_TYPE_CUDA;
  sessionparam.device     = gdevice;
  sessionparam.apiVersion = NVENCAPI_VERSION;
  nvstatus = encodeAPI.nvEncOpenEncodeSessionEx(&sessionparam, &encoder);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncOpenEncodeSessionEx failed" << std::endl;
    return 1;
  }

  // Set encoder initialization parameters
  encinitparam.version           = NV_ENC_INITIALIZE_PARAMS_VER;
  encinitparam.encodeGUID        = NV_ENC_CODEC_H264_GUID;
  encinitparam.presetGUID        = NV_ENC_PRESET_LOW_LATENCY_HQ_GUID;
  encinitparam.encodeWidth       = 2048;
  encinitparam.encodeHeight      = 4096;
  encinitparam.darWidth          = 2048;
  encinitparam.darHeight         = 4096;
  encinitparam.maxEncodeWidth    = 0;
  encinitparam.maxEncodeHeight   = 0;
  encinitparam.frameRateNum      = 90;
  encinitparam.enableEncodeAsync = 0;
  encinitparam.encodeConfig      = &encodecfg;
  encinitparam.enablePTD         = 1;

  memset(&encodecfg, 0, sizeof(NV_ENC_CONFIG));
  memset(&presetcfg, 0, sizeof(NV_ENC_PRESET_CONFIG));
  presetcfg.version           = NV_ENC_PRESET_CONFIG_VER;
  presetcfg.presetCfg.version = NV_ENC_CONFIG_VER;

  nvstatus = encodeAPI.nvEncGetEncodePresetConfig(
      encoder, encinitparam.encodeGUID, encinitparam.presetGUID, &presetcfg);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncGetEncodePresetConfig failed" << std::endl;
    return 1;
  }

  // Set encoder configurations
  memcpy(&encodecfg, &presetcfg, sizeof(NV_ENC_CONFIG));
  encodecfg.gopLength                    = NVENC_INFINITE_GOPLENGTH;
  encodecfg.frameIntervalP               = 1;  // IPP
  encodecfg.frameFieldMode               = NV_ENC_PARAMS_FRAME_FIELD_MODE_FRAME;
  encodecfg.rcParams.rateControlMode     = NV_ENC_PARAMS_RC_CONSTQP;
  encodecfg.rcParams.constQP.qpInterP    = 32;
  encodecfg.rcParams.constQP.qpIntra     = 32;
  encodecfg.rcParams.initialRCQP.qpIntra = 24;
  encodecfg.encodeCodecConfig.h264Config.maxNumRefFrames = 16;
  encodecfg.encodeCodecConfig.h264Config.chromaFormatIDC = 1;  // YUV420
  encodecfg.encodeCodecConfig.h264Config.disableDeblockingFilterIDC = 0;

  // Initialize encoder
  encodeAPI.nvEncInitializeEncoder(encoder, &encinitparam);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncInitializeEncoder failed" << std::endl;
    return 1;
  }

  // Create input buffer
  NV_ENC_CREATE_INPUT_BUFFER inputbufferparam;
  std::memset(&inputbufferparam, 0, sizeof(NV_ENC_CREATE_INPUT_BUFFER));
  inputbufferparam.version    = NV_ENC_CREATE_INPUT_BUFFER_VER;
  inputbufferparam.width      = 2048;
  inputbufferparam.height     = 4096;
  inputbufferparam.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;
  inputbufferparam.bufferFmt  = NV_ENC_BUFFER_FORMAT_IYUV;  // Is this yuv420?

  nvstatus = encodeAPI.nvEncCreateInputBuffer(encoder, &inputbufferparam);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncCreateInputBuffer failed" << std::endl;
    return 1;
  }
  inputbuffer = inputbufferparam.inputBuffer;

  // Create output buffer
  NV_ENC_CREATE_BITSTREAM_BUFFER outputbufferparam;
  std::memset(&outputbufferparam, 0, sizeof(NV_ENC_CREATE_BITSTREAM_BUFFER));
  outputbufferparam.version    = NV_ENC_CREATE_BITSTREAM_BUFFER_VER;
  outputbufferparam.size       = 2 * 1024 * 1024;  // No idea why
  outputbufferparam.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;

  nvstatus = encodeAPI.nvEncCreateBitstreamBuffer(encoder, &outputbufferparam);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncCreateBitstreamBuffer failed" << std::endl;
    return 1;
  }
  outputbuffer = outputbufferparam.bitstreamBuffer;

  // Load a frame into input buffer
  NV_ENC_LOCK_INPUT_BUFFER inputbufferlocker;
  std::memset(&inputbufferlocker, 0, sizeof(NV_ENC_LOCK_INPUT_BUFFER));
  inputbufferlocker.version     = NV_ENC_LOCK_INPUT_BUFFER_VER;
  inputbufferlocker.inputBuffer = inputbuffer;
  if ((nvstatus = encodeAPI.nvEncLockInputBuffer(
           encoder, &inputbufferlocker)) != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncLockInputBuffer failed" << std::endl;
    return 1;
  }
  std::ifstream fs(argv[1] /*"sample_2048x4096_YUV420.yuv"*/,
                   std::ifstream::in | std::ifstream::binary);
  fs.read(reinterpret_cast<char*>(inputbufferlocker.bufferDataPtr),
          12582912);  // 2048*4096*1.5
  fs.close();
  if ((nvstatus = encodeAPI.nvEncUnlockInputBuffer(encoder, inputbuffer)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncUnlockInputBuffer failed" << std::endl;
    return 1;
  }

  // Prepare picture for encoding
  std::memset(&encodepicparam, 0, sizeof(NV_ENC_PIC_PARAMS));
  encodepicparam.version         = NV_ENC_PIC_PARAMS_VER;
  encodepicparam.inputWidth      = 2048;
  encodepicparam.inputHeight     = 4096;
  encodepicparam.inputPitch      = 2048;
  encodepicparam.inputBuffer     = inputbuffer;
  encodepicparam.outputBitstream = outputbuffer;
  encodepicparam.bufferFmt       = NV_ENC_BUFFER_FORMAT_IYUV;
  encodepicparam.pictureStruct   = NV_ENC_PIC_STRUCT_FRAME;
  encodepicparam.inputTimeStamp  = 0;

  // Encode a frame
  if ((nvstatus = encodeAPI.nvEncEncodePicture(encoder, &encodepicparam)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncEncodePicture failed" << std::endl;
    return 1;
  }

  // Retrieve encoded frame
  NV_ENC_LOCK_BITSTREAM outputbufferlocker;
  std::memset(&outputbufferlocker, 0, sizeof(NV_ENC_LOCK_BITSTREAM));
  outputbufferlocker.version         = NV_ENC_LOCK_BITSTREAM_VER;
  outputbufferlocker.outputBitstream = outputbuffer;
  if ((nvstatus = encodeAPI.nvEncLockBitstream(encoder, &outputbufferlocker)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncLockBitstream failed " << nvstatus << std::endl;
    return 1;
  }

  std::cout << "Encoded size: " << outputbufferlocker.bitstreamSizeInBytes
            << std::endl;
  std::ofstream ofs("sample0.h264", std::ofstream::out | std::ofstream::binary);
  ofs.write(
      reinterpret_cast<const char*>(outputbufferlocker.bitstreamBufferPtr),
      outputbufferlocker.bitstreamSizeInBytes);
  ofs.close();

  if ((nvstatus = encodeAPI.nvEncUnlockBitstream(encoder, outputbuffer)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncUnlockBitstream failed" << std::endl;
    return 1;
  }

  // Encode second frame
  if ((nvstatus = encodeAPI.nvEncEncodePicture(encoder, &encodepicparam)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncEncodePicture failed" << std::endl;
    return 1;
  }

  // Retrieve second encoded frame
  std::memset(&outputbufferlocker, 0, sizeof(NV_ENC_LOCK_BITSTREAM));
  outputbufferlocker.version         = NV_ENC_LOCK_BITSTREAM_VER;
  outputbufferlocker.outputBitstream = outputbuffer;
  if ((nvstatus = encodeAPI.nvEncLockBitstream(encoder, &outputbufferlocker)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncLockBitstream failed " << nvstatus << std::endl;
    return 1;
  }

  std::cout << "Encoded size: " << outputbufferlocker.bitstreamSizeInBytes
            << std::endl;
  ofs.open("sample1.h264", std::ofstream::out | std::ofstream::binary);
  ofs.write(
      reinterpret_cast<const char*>(outputbufferlocker.bitstreamBufferPtr),
      outputbufferlocker.bitstreamSizeInBytes);
  ofs.close();

  if ((nvstatus = encodeAPI.nvEncUnlockBitstream(encoder, outputbuffer)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "nvEncUnlockInputBuffer failed" << std::endl;
    return 1;
  }

  // Destroy input buffer
  if (inputbuffer)
  {
    nvstatus = encodeAPI.nvEncDestroyInputBuffer(encoder, inputbuffer);
    if (nvstatus != NV_ENC_SUCCESS)
    {
      std::cout << "nvEncDestroyInputBuffer failed" << std::endl;
      return 1;
    }
  }

  // Destroy output buffer
  if (outputbuffer)
  {
    nvstatus = encodeAPI.nvEncDestroyBitstreamBuffer(encoder, outputbuffer);
    if (nvstatus != NV_ENC_SUCCESS)
    {
      std::cout << "nvEncDestroyBitstreamBuffer failed" << std::endl;
      return 1;
    }
  }

  // Terminate Encoding Session
  nvstatus = encodeAPI.nvEncDestroyEncoder(encoder);
  if (nvstatus != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncDestroyEncoder failed" << std::endl;
    return 1;
  }

  return 0;
}