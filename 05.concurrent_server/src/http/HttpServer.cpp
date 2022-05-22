// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <stdexcept>
#include <string>

#include "HttpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Socket.hpp"
#include "Queue.hpp"
#include "HttpConnectionHandler.hpp"

HttpServer::HttpServer() {
}

HttpServer::~HttpServer() {
}

void HttpServer::listenForever(const char* port) {
  return TcpServer::listenForever(port);
}

void HttpServer::handleClientConnection(Socket& client) {
  // Make a copy of the Socket
  this->socketQueue->push(client);
}

void HttpServer::create_new_handlers(int my_max_connections) {
    my_handlers = new std::vector<HttpConnectionHandler*>();
    my_handlers[0].resize(my_max_connections);
    const Socket& stopCond = Socket();

  for (int iterator = 0; iterator < my_max_connections; iterator++) {
    my_handlers[0][iterator] = new HttpConnectionHandler(this, this->socketQueue, stopCond);
  }
  for (int iterator = 0; iterator < my_max_connections; iterator++) {
    my_handlers[0][iterator]->startThread();
  }

}

void HttpServer::startHttpServer(int maxConnections) {
  initializeQueue();
  this->create_new_handlers(maxConnections);
}

// Detiene el servidor. Mete en la cola un socket especial para terminar la
// ejecución.
// Espera a que cada hilo finalice y luego libera la memoria.
void HttpServer::stopHttpServer(int maxConnections) {
  auto* stopCondition = new Socket();

  for (int index = 0; index < maxConnections; index++) {
    this->socketQueue->push(*stopCondition);
  }

  for (auto finishing : this->my_handlers[0]) {
    finishing->waitToFinish();
  }

  for (auto terminated : this->my_handlers[0]) {
    delete terminated;
  }

  delete this->socketQueue;
  delete this->my_handlers;
  delete stopCondition;
}

void HttpServer::initializeQueue() {
    this->socketQueue = new Queue<Socket>();
}
