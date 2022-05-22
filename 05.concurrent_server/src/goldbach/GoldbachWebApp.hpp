// Copyright 2021 Emmanuel D. Solis. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHWEBAPP_H
#define GOLDBACHWEBAPP_H

#include <stdio.h>
#include <string>
#include <vector>
#include "WebServer.hpp"
#include "GoldbachCalculator.hpp"
#include "Queue.hpp"

class GoldbachWebApp {
 public:
  GoldbachWebApp(/* args */);
  ~GoldbachWebApp();
  bool serveHomepage(HttpRequest& httpRequest
  , HttpResponse& httpResponse);
  bool serveNotFound(HttpRequest& httpRequest
  , HttpResponse& httpResponse);
  bool serveGoldbachSums(HttpRequest &httpRequest, HttpResponse &httpResponse,
                         std::vector<int64_t> *numbersToCalculate, Queue<SubList *> *consumerQueue);
  void finishAbruptly();

  void printSums(HttpResponse& httpResponse, SubList* subList);
  /** 
   * @brief Analiza cadena con números recibida por string y extrae los números
   * @param numbers Ruta en forma de string (contiene los input adentro).
   * @return Devuelve vector con los números ingresados por el usuario.
  */
  std::vector<int64_t>* analyzeString(std::string route);
};

#endif /* GOLDBACHWEBAPP_H */
