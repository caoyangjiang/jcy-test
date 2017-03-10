// Copyright @ 2016 Caoyang Jiang

#include "Jcy/IEServer/ChatClient.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    jcy::chat_client c(io_service, iterator);

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    char line[jcy::chat_message::max_body_length + 1];
    while (std::cin.getline(line, jcy::chat_message::max_body_length + 1))
    {
      jcy::chat_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}