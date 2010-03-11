#include <iostream>

#include <stdlib.h>

#include "internetserver.hpp"
#include "httpmaster.hpp"

#define PORT 8080

int main(int argc, char **argv) {
  HttpMaster master;

  uint32_t bind_address = INADDR_ANY;
  short port = PORT;

  InternetServer *server = new InternetServer(bind_address, port, &master, 1);
    
  server->Run();
  std::cout << "Hit q and return to exit" << std::endl;
  char response;
  std::cin >> response;
  server->Shutdown();
  delete server;
   
  return EXIT_SUCCESS;
}
