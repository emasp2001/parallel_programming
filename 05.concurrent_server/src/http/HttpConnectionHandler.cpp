// Copyright 2021 Emmanuel Solis. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <stdexcept>
#include <string>
#include "HttpConnectionHandler.hpp"
#include "Socket.hpp"


HttpConnectionHandler::~HttpConnectionHandler() {

}

int HttpConnectionHandler::run() {
  this->consumeForever();
  return EXIT_SUCCESS;
}

void HttpConnectionHandler::consume(const Socket& client) {
  // TODO(you): Make this method concurrent. Store client connections (sockets)
  // into a collection (e.g thread-safe queue) and stop

  // TODO(you) Move the following loop to a consumer thread class
  // While the same client asks for HTTP requests in the same connection
  // Create an object that parses the HTTP request from the socket
  HttpRequest httpRequest(client);

  // If the request is not valid or an error happened
  if (!httpRequest.parse()) {
    // Non-valid requests are normal after a previous valid request. Do not
    // close the connection yet, because the valid request may take time to
    // be processed. Just stop waiting for more requests
  }

  // A complete HTTP client request was received. Create an object for the
  // server responds to that client's request
  HttpResponse httpResponse(client);

  // Give subclass a chance to respond the HTTP request
  const bool handled = server->handleHttpRequest(httpRequest, httpResponse);

  // If subclass did not handle the request or the client used HTTP/1.0
  if (!handled || httpRequest.getHttpVersion() == "HTTP/1.0") {
    // The socket will not be more used, close the connection
    Socket temp = client;
    temp.close();
  }

  // This version handles just one client request per connection
}
