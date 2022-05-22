// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0
// Serial web server's initial code for parallelization

#include <csignal>
#include <iostream>

#include "WebServer.hpp"

void sigint_handler(int sig) {
  std::cout << "\nSaliendo de la funcion abruptamente..." << std::endl;
  std::cout << "Limpiando memoria..." << std::endl;
  WebServer::getInstance().stop();
  std::cout << "Salida exitosa." << std::endl;
}

int main(int argc, char* argv[]) {
  if (signal(SIGINT, sigint_handler) == SIG_ERR) {
    std::cout << "Salgamos" << std::endl;
  }
  return WebServer::getInstance().start(argc, argv);
}
