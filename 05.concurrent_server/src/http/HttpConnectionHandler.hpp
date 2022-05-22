// Copyright 2021 Emmanuel Solis. Universidad de Costa Rica. CC BY 4.0

#ifndef HANDLEHTTPREQUEST_H
#define HANDLEHTTPREQUEST_H

#include "Consumer.hpp"
#include "Queue.hpp"
#include "HttpServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "WebServer.hpp"

class HttpConnectionHandler : public Consumer<Socket> {
private:
    /* data */
    HttpServer *server;
public:
    HttpConnectionHandler(
            HttpServer* myServer = nullptr,
            Queue<Socket>* socketQueue = nullptr,
            const Socket& stopCondition = Socket())
            :
            Consumer<Socket>(socketQueue, stopCondition),
            server(myServer)


    {}

    ~HttpConnectionHandler();
    virtual int run();
    void consume(const Socket& client) override;
};

#endif /*HANDLEHTTPREQUEST_H*/
