// Copyright 2021 Emmanuel D. Solis. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachWebApp.hpp"

#include <math.h>     // Incluir funciones matematicas
#include <stdbool.h>  // Uso de booleanos
#include <stdio.h>
#include <stdlib.h>
#include <string>  // Para poder usar strings.


GoldbachWebApp::GoldbachWebApp(/* args */) {
}

GoldbachWebApp::~GoldbachWebApp() {
}

// TODO(you): Fix code redundancy in the following methods

bool GoldbachWebApp::serveHomepage(HttpRequest& httpRequest
        , HttpResponse& httpResponse) {
    (void)httpRequest;

    // TODO(you) Move form to your view class, e.g GoldbachWebApp

    // Set HTTP response metadata (headers)
    httpResponse.setHeader("Server", "AttoServer v1.0");
    httpResponse.setHeader("Content-type", "text/html; charset=ascii");

    // Build the body of the response
    std::string title = "Goldbach sums de Bryan y Emma";
    httpResponse.body() << "<!DOCTYPE html>\n"
                        << "<html lang=\"en\">\n"
                        << "  <meta charset=\"ascii\"/>\n"
                        << "  <title>" << title << "</title>\n"
                        << "  <style>body {font-family: monospace}</style>\n"
                        << "  <h1>" << title << "</h1>\n"
                        << "  <form method=\"get\" action=\"/goldbach\">\n"
                        << "    <label for=\"number\">Number</label>\n"
                        << "    <input type=\"text\" name=\"number\" required/>\n"
                        << "    <button type=\"submit\">Calculate</button>\n"
                        << "  </form>\n"
                        << "</html>\n";

    // Send the response to the client (user agent)
    return httpResponse.send();
}

bool GoldbachWebApp::serveNotFound(HttpRequest& httpRequest
        , HttpResponse& httpResponse) {
    (void)httpRequest;

    // Set HTTP response metadata (headers)
    httpResponse.setStatusCode(404);
    httpResponse.setHeader("Server", "AttoServer v1.0");
    httpResponse.setHeader("Content-type", "text/html; charset=ascii");

    // Build the body of the response
    std::string title = "Not found";
    httpResponse.body() << "<!DOCTYPE html>\n"
                        << "<html lang=\"en\">\n"
                        << "  <meta charset=\"ascii\"/>\n"
                        << "  <title>" << title << "</title>\n"
                        << "  <style>body {font-family: monospace} h1 {color: red}</style>\n"
                        << "  <h1>" << title << "</h1>\n"
                        << "  <p>The requested resouce was not found in this server.</p>\n"
                        << "  <hr><p><a href=\"/\">Homepage</a></p>\n"
                        << "</html>\n";

    // Send the response to the client (user agent)
    return httpResponse.send();
}

// TODO(you) Move domain-logic from WebServer controller to a view class
// e.g GoldbachWebApp, and a model class e.g GoldbachCalculator
bool GoldbachWebApp::serveGoldbachSums(HttpRequest &httpRequest, HttpResponse &httpResponse,
                                       std::vector<int64_t> *numbersToCalculate, Queue<SubList *> *consumerQueue) {

    (void)httpRequest;
    std::string print_numbers = "";

    for (size_t index = 0; index < numbersToCalculate[0].size(); index++) {
        print_numbers += std::to_string(numbersToCalculate[0][index]) + " ";
    }

    // Super lista que contiene todos los numeros que ingreso el usuario.
    // Dentro de la super lista se crean tantos contenedores como numeros que
    // ingreso el usuario.
    // Cada uno de los contenedores tiene su propio numero Goldbach con qué
    // trabajar y tambien sirve como contenedor para almacenar los resultados.
    auto* superList = new SuperList(numbersToCalculate);
    std::vector<SubList*>* sublistArray = superList->getSublistArray();

    // Mete punteros a cada uno de los contenedores dentro de la consumingQueue,
    // la cual es usada por los ensambladores.
    for (size_t index = 0; index < superList->getSuperListLength(); index++) {
        consumerQueue->push(sublistArray[0][index]);
    }

    //UNa vez que todos los contenedores son ingresados a la consumingQueue, 
    // este hilo espera mediante un semaforo de la super lista a que todos los
    // contenedores hayan sido procesados antes de poder imprimirlos. 
    superList->waitForCompletion();


    // Set HTTP response metadata (headers)
    httpResponse.setHeader("Server", "AttoServer v1.0");
    httpResponse.setHeader("Content-type", "text/html; "
                                           "charset=ascii");

    // Build the body of the response
    std::string title = "Goldbach sums for " + print_numbers;
    httpResponse.body() << "<!DOCTYPE html>\n"
                        << "<html lang=\"en\">\n"
                        << "  <meta charset=\"ascii\"/>\n"
                        << "  <title>" << title << "</title>\n"
                        << "  <style>body {font-family: monospace} "
                           ".err {color: red}</style>\n"
                        << "  <h1>" << title << "</h1>\n"
                        << "  <hr><p><a href=\"/\">Back</a></p>\n"
                        << "</html>\n";
    for (size_t index = 0; index < superList->getSuperListLength(); index++) {
        if (numbersToCalculate[0][index] < 0 && llabs(numbersToCalculate[0][index])
                                                > 5) {
            httpResponse.body()
                    << "  <h2>" << numbersToCalculate[0][index] << "</h2>\n"
                    << "  <p>" << numbersToCalculate[0][index] << ": " << sublistArray[0][index]->getSumsArray()->size() << " sums: ";
            printSums(httpResponse, sublistArray[0][index]);
        } else if (llabs(numbersToCalculate[0][index]) <= 5) {
            httpResponse.body()
                    << "  <h2>"<< numbersToCalculate[0][index] <<"</h2>\n"
                    << "  <p>"<< numbersToCalculate[0][index] << ": NA</p>"
                    << "</html>\n";
        } else {
            httpResponse.body()
                    << "  <h2>" << numbersToCalculate[0][index] << "</h2>\n"
                    << "  <p>" << numbersToCalculate[0][index] << ": "
                    << sublistArray[0][index]->getSumsArray()->size()
                    << " sums</p>"
                    << "</html>\n";


        }
    }
    httpResponse.body() << "  <hr><p><a href=\"/\">Back</a></p>\n"
                        << "</html>\n";
    // Send the response to the client (user agent)
    delete superList;
    delete numbersToCalculate;
    return httpResponse.send();
}

std::vector<int64_t> *GoldbachWebApp::analyzeString(std::string route) {
    const std::string delimiter = "%2C";
    std::string token;
    int64_t number;
    size_t index;
    auto* inputNumbersArray = new std::vector<int64_t>;
    bool cond = true;
    route += delimiter;
    index = route.find(delimiter);
    while (index != std::string::npos) {
        number = 0;
        token = route.substr(0, index);
        try {
            number = std::stoll(token);
        } catch(...) {
            cond = false;
        }
        if (cond != false) {
            inputNumbersArray->push_back(number);
        }
        route.erase(0, index + delimiter.length());
        index = route.find(delimiter);
        cond = true;
    }
    return inputNumbersArray;

}

void GoldbachWebApp:: printSums(HttpResponse& httpResponse,
                                SubList* subList) {

    bool cond = false;
    std::vector<Sum*>* sumsArray = subList->getSumsArray();

    for (size_t index = 0; index < sumsArray[0].size(); index++) {
        if (cond) {
            break;
        } else if (subList->getGoldbachNumber() % 2 == 0) {
            httpResponse.body() << sumsArray[0][index]->getFirstNumber() << " + "
                                << sumsArray[0][index]->getSecondNumber();

            if (index + 1 != sumsArray[0].size()) {
                httpResponse.body() << ", ";
            } else {
                cond = true;
            }
        } else {
            httpResponse.body() << sumsArray[0][index]->getFirstNumber() << " + "
                                << sumsArray[0][index]->getSecondNumber() << " + "
                                << sumsArray[0][index]->getThirdNumber();

            if (index + 1 != sumsArray[0].size()) {
                httpResponse.body() << ", ";
            } else {
                cond = true;
            }
        }
    }
}
