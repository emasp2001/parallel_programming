// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <vector>

#include "TcpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Queue.hpp"

#define MAX_CLIENTS_SAME_TIME 8


class HttpServer : public TcpServer {
  DISABLE_COPY(HttpServer);

 protected:
  /// Lookup criteria for searching network information about this host
  struct addrinfo hints;
  Queue<Socket>* socketQueue;
  class HttpConnectionHandler* handler;
  std::vector<HttpConnectionHandler*>* my_handlers;

 public:
  /// Constructor
  HttpServer();
  /// Destructor
  ~HttpServer();
  /// Infinetelly listen for client connection requests and accept all of them.
  /// For each accepted connection request, the virtual onConnectionAccepted()
  /// will be called. Inherited classes must override that method
  void listenForever(const char* port);
  virtual bool handleHttpRequest(HttpRequest& httpRequest,
    HttpResponse& httpResponse) = 0;

  void create_new_handlers(int my_max_connections);


  /**
    * @brief Initiates instance for Queue and creates
    * consumers.
    * @param maxConnections Maximum amount of connections/
    * consumers allowed.
    **/
  void startHttpServer(int maxConnections);

  /**
    * @brief Waits for consumers to finish their work and
    * executes waitToFinish(), then closes all instances
    * to finish the program.
    * @param maxConnections Maximum amount of connections/
    * consumers allowed.
    **/
  void stopHttpServer(int maxConnections);

  /**
    * @brief Initializes the client/requests Queue
    **/
  void initializeQueue();

 protected:
  int max_connections = MAX_CLIENTS_SAME_TIME;
  /// This method is called each time a client connection request is accepted.
  void handleClientConnection(Socket& client) override;
  /// Called each time an HTTP request is received. Web server should analyze
  /// the request object and assemble a response with the response object.
  /// Finally send the response calling the httpResponse.send() method.
  /// @return true on success and the server will continue handling further
  /// HTTP requests, or false if server should stop accepting requests from
  /// this client (e.g: HTTP/1.0)
};

#endif  // HTTPSERVER_H
