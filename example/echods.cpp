/*
 * Copyright 2010 Jonathan R. Guthrie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include <stdlib.h>

#include <internetserver.hpp>

#include "echomaster.hpp"

#define PORT 8181

#define KEYFILE "./test-priv.pem"
#define CERTFILE "./test-cert.pem"
#define CAFILE "./test-cert.pem"
#define CRLFILE "./test-crl.pem"

int main(int argc, char **argv) {
  EchoMaster master(10); // Idle timeout in seconds

  uint32_t bind_address = INADDR_ANY;
  short port = PORT;

  InternetServer *server = new InternetServer(bind_address, port, &master, KEYFILE, CERTFILE, CAFILE, CRLFILE, 1);
    
  server->run();
  std::cout << "Hit q and return to exit" << std::endl;
  char response;
  std::cin >> response;
  server->shutdown();
  delete server;
   
  return EXIT_SUCCESS;
}
