# Check if SSE/AVX instructions are available on the machine where
# the project is compiled.

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
   EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

   STRING(REGEX REPLACE "^.*(sse2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse2" "${SSE_THERE}" SSE2_TRUE)
   IF (SSE2_TRUE)
      set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")
   ELSE (SSE2_TRUE)
      set(SSE2_FOUND false CACHE BOOL "SSE2 available on host")
   ENDIF (SSE2_TRUE)

   # /proc/cpuinfo apparently omits sse3 :(
   STRING(REGEX REPLACE "^.*[^s](sse3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse3" "${SSE_THERE}" SSE3_TRUE)
   IF (NOT SSE3_TRUE)
      STRING(REGEX REPLACE "^.*(T2300).*$" "\\1" SSE_THERE ${CPUINFO})
      STRING(COMPARE EQUAL "T2300" "${SSE_THERE}" SSE3_TRUE)
   ENDIF (NOT SSE3_TRUE)

   STRING(REGEX REPLACE "^.*(ssse3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "ssse3" "${SSE_THERE}" SSSE3_TRUE)
   IF (SSE3_TRUE OR SSSE3_TRUE)
      set(SSE3_FOUND true CACHE BOOL "SSE3 available on host")
   ELSE (SSE3_TRUE OR SSSE3_TRUE)
      set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")
   ENDIF (SSE3_TRUE OR SSSE3_TRUE)
   IF (SSSE3_TRUE)
      set(SSSE3_FOUND true CACHE BOOL "SSSE3 available on host")
   ELSE (SSSE3_TRUE)
      set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")
   ENDIF (SSSE3_TRUE)

   STRING(REGEX REPLACE "^.*(sse4_1).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "sse4_1" "${SSE_THERE}" SSE41_TRUE)
   IF (SSE41_TRUE)
      set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")
   ELSE (SSE41_TRUE)
      set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   ENDIF (SSE41_TRUE)

   STRING(REGEX REPLACE "^.*(avx).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "avx" "${SSE_THERE}" AVX_TRUE)
   IF (AVX_TRUE)
      set(AVX_FOUND true CACHE BOOL "AVX available on host")
   ELSE (AVX_TRUE)
      set(AVX_FOUND false CACHE BOOL "AVX available on host")
   ENDIF (AVX_TRUE)
 
   STRING(REGEX REPLACE "^.*(avx2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "avx2" "${SSE_THERE}" AVX2_TRUE)
   IF (AVX2_TRUE)
      set(AVX2_FOUND true CACHE BOOL "AVX2 available on host")
   ELSE (AVX2_TRUE)
      set(AVX2_FOUND false CACHE BOOL "AVX2 available on host")
   ENDIF (AVX2_TRUE)

ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
   EXEC_PROGRAM("/usr/sbin/sysctl -n machdep.cpu.features" OUTPUT_VARIABLE
      CPUINFO)

   STRING(REGEX REPLACE "^.*[^S](SSE2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE2" "${SSE_THERE}" SSE2_TRUE)
   IF (SSE2_TRUE)
      set(SSE2_FOUND true CACHE BOOL "SSE2 available on host")
   ELSE (SSE2_TRUE)
      set(SSE2_FOUND false CACHE BOOL "SSE2 available on host")
   ENDIF (SSE2_TRUE)

   STRING(REGEX REPLACE "^.*[^S](SSE3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE3" "${SSE_THERE}" SSE3_TRUE)
   IF (SSE3_TRUE)
      set(SSE3_FOUND true CACHE BOOL "SSE3 available on host")
   ELSE (SSE3_TRUE)
      set(SSE3_FOUND false CACHE BOOL "SSE3 available on host")
   ENDIF (SSE3_TRUE)

   STRING(REGEX REPLACE "^.*(SSSE3).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSSE3" "${SSE_THERE}" SSSE3_TRUE)
   IF (SSSE3_TRUE)
      set(SSSE3_FOUND true CACHE BOOL "SSSE3 available on host")
   ELSE (SSSE3_TRUE)
      set(SSSE3_FOUND false CACHE BOOL "SSSE3 available on host")
   ENDIF (SSSE3_TRUE)

   STRING(REGEX REPLACE "^.*(SSE4.1).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "SSE4.1" "${SSE_THERE}" SSE41_TRUE)
   IF (SSE41_TRUE)
      set(SSE4_1_FOUND true CACHE BOOL "SSE4.1 available on host")
   ELSE (SSE41_TRUE)
      set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   ENDIF (SSE41_TRUE)

   STRING(REGEX REPLACE "^.*(AVX).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "AVX" "${SSE_THERE}" AVX_TRUE)
   IF (AVX_TRUE)
      set(AVX_FOUND true CACHE BOOL "AVX available on host")
   ELSE (AVX_TRUE)
      set(AVX_FOUND false CACHE BOOL "AVX available on host")
   ENDIF (AVX_TRUE)

   STRING(REGEX REPLACE "^.*(AVX2).*$" "\\1" SSE_THERE ${CPUINFO})
   STRING(COMPARE EQUAL "AVX2" "${SSE_THERE}" AVX2_TRUE)
   IF (AVX2_TRUE)
      set(AVX2_FOUND true CACHE BOOL "AVX2 available on host")
   ELSE (AVX2_TRUE)
      set(AVX2_FOUND false CACHE BOOL "AVX2 available on host")
   ENDIF (AVX2_TRUE)

ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
   # TODO
   set(SSE2_FOUND   true  CACHE BOOL "SSE2 available on host")
   set(SSE3_FOUND   false CACHE BOOL "SSE3 available on host")
   set(SSSE3_FOUND  false CACHE BOOL "SSSE3 available on host")
   set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   set(AVX_FOUND false CACHE BOOL "AVX available on host")
   set(AVX2_FOUND false CACHE BOOL "AVX2 available on host")
ELSE(CMAKE_SYSTEM_NAME MATCHES "Linux")
   set(SSE2_FOUND   true  CACHE BOOL "SSE2 available on host")
   set(SSE3_FOUND   false CACHE BOOL "SSE3 available on host")
   set(SSSE3_FOUND  false CACHE BOOL "SSSE3 available on host")
   set(SSE4_1_FOUND false CACHE BOOL "SSE4.1 available on host")
   set(AVX_FOUND false CACHE BOOL "AVX available on host")
   set(AVX2_FOUND false CACHE BOOL "AVX2 available on host")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(NOT SSE2_FOUND)
      MESSAGE(STATUS "Could not find hardware support for SSE2 on this machine.")
endif(NOT SSE2_FOUND)
if(NOT SSE3_FOUND)
      MESSAGE(STATUS "Could not find hardware support for SSE3 on this machine.")
endif(NOT SSE3_FOUND)
if(NOT SSSE3_FOUND)
      MESSAGE(STATUS "Could not find hardware support for SSSE3 on this machine.")
endif(NOT SSSE3_FOUND)
if(NOT SSE4_1_FOUND)
      MESSAGE(STATUS "Could not find hardware support for SSE4.1 on this machine.")
endif(NOT SSE4_1_FOUND)
if(NOT AVX_FOUND)
      MESSAGE(STATUS "Could not find hardware support for AVX on this machine.")
endif(NOT AVX_FOUND)
if(NOT AVX2_FOUND)
      MESSAGE(STATUS "Could not find hardware support for AVX2 on this machine.")
endif(NOT AVX2_FOUND)

mark_as_advanced(SSE2_FOUND SSE3_FOUND SSSE3_FOUND SSE4_1_FOUND, AVX_FOUND, AVX2_FOUND)

