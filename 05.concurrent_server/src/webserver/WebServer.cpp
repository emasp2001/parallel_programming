// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include <cassert>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

#include "NetworkAddress.hpp"
#include "Socket.hpp"
#include "WebServer.hpp"

const char* const usage =
        "Usage: webserv [port] [max_connections]\n"
        "\n"
        "  port             Network port to listen incoming HTTP requests, default "
        DEFAULT_PORT "\n"
        "  max_connections  Maximum number of allowed client connections\n";

// TODO(you) Make WebServer a singleton class. See the Log class
WebServer::WebServer() {
}

WebServer::~WebServer() {
}

WebServer& WebServer::getInstance() {
    static WebServer ws;
    return ws;
}

int WebServer::start(int argc, char* argv[]) {
	try {
		if (this->analyzeArguments(argc, argv)) {
			// TODO(you) Handle signal 2 (SIGINT) and 15 (SIGTERM), see man signal
			// Signal handler should call WebServer::stopListening(), send stop
			// conditions and wait for all secondary threads that it created
			// HttpServer::create_new_handlers(max_connections);
			this->listenForConnections(this->port);
			const NetworkAddress& address = this->getNetworkAddress();
			std::cout << "web server listening on " << address.getIP()
			<< " port " << address.getPort() << "...\n";

			// Inicializa los Assemblers que consumen numeros Goldbach e
			// inicializa las colas respectivas.
			this->initializeGoldbachConsumers(this->max_connections);

			// Inicializa los consumidores de sockets y la cola de sockets
			this->startHttpServer(this->max_connections);
			
			// Ciclo infinito que mete sockets a la cola.
			this->acceptAllConnections();
		}
	} catch (const std::runtime_error& error) {
		std::cerr << "error: " << error.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

void WebServer::stop() {
	this->stopHttpServer(this->max_connections);
	this->destroyGoldbachConsumers();
}

bool WebServer::analyzeArguments(int argc, char* argv[]) {
	// Traverse all arguments
	for (int index = 1; index < argc; ++index) {
		const std::string argument = argv[index];
		if (argument.find("help") != std::string::npos) {
			std::cout << usage;
			return false;
		}
	}

	switch (argc) {
		case 2:
			this->port = argv[1];
			break;

		case 3:
			this->port = argv[1];
			this->max_connections = std::stoi(argv[2]);
			break;

		default:
				break;
	}

	return true;
}

bool WebServer::handleHttpRequest(HttpRequest& httpRequest,
																HttpResponse& httpResponse) {
	// Print IP and port from client
	const NetworkAddress& address = httpRequest.getNetworkAddress();
	std::cout << "connection established with client " << address.getIP()
						<< " port " << address.getPort() << std::endl;

	// Print HTTP request
	std::cout << "  " << httpRequest.getMethod()
						<< ' ' << httpRequest.getURI()
						<< ' ' << httpRequest.getHttpVersion() << std::endl;

	return this->route(httpRequest, httpResponse);
}

bool WebServer::route(HttpRequest& httpRequest, HttpResponse& httpResponse) {
	GoldbachWebApp webApp;
	// If the home page was asked
	if (httpRequest.getMethod() == "GET" && httpRequest.getURI() == "/") {
		return webApp.serveHomepage(httpRequest, httpResponse);
	}

	// TODO(you): URI can be a multi-value list, e.g: 100,2784,-53,200771728
	// TODO(you): change for sendGoldbachSums() if you prefer it
	std::smatch matches;

	// TODO(you): Numbers given by user may be larger than int64_t, reject them

	// If a number was asked in the form "/goldbach/1223"
	// or "/goldbach?number=1223"

	// Extrae los numeros Goldbach desde la ruta y los almacena en un vector de 
	// numeros.
	std::regex inQuery("^/goldbach(/|\\?number=)([+-]?\\d+(%2C[+-]?\\d+)*)$" );
	if (std::regex_search(httpRequest.getURI(), matches, inQuery)) {
		assert(matches.length() >= 3);
		std::string numbers = matches[2];
		std::vector<int64_t>* numbersToCalculate = webApp.analyzeString(numbers);
		if (numbersToCalculate[0].empty()) {
				return webApp.serveNotFound(httpRequest, httpResponse);
		}
		return webApp.serveGoldbachSums(httpRequest, httpResponse, numbersToCalculate, this->consumingQueue);
	}

	// Unrecognized request
	return webApp.serveNotFound(httpRequest, httpResponse);
}

void WebServer::initializeGoldbachConsumers(size_t amount) {
	this->stopCondition = new SubList();

	this->assemblerArray = 
	 new std::vector<class GoldbachAssembler<class SubList*, class SubList*>*>();

	this->assemblerArray[0].resize(amount);
	this->consumingQueue = new Queue<class SubList*>();
	this->producingQueue = new Queue<class SubList*>();

	for (size_t i = 0; i < amount; i++) {
		this->assemblerArray[0][i] =
			new class GoldbachAssembler<class SubList*, class SubList*>(
			this->consumingQueue,
			this->producingQueue,
			this->stopCondition);

		this->assemblerArray[0][i]->startThread();
	}
}

// Mete a la cola de assembler una condicion de parada.
// Espera que cada hilo termine su ejecución.
void WebServer::destroyGoldbachConsumers() {
	size_t size = this->assemblerArray[0].size();

	for (size_t index = 0; index < size; index++) {
		this->consumingQueue->push(this->stopCondition);
	}

	for (size_t index = 0; index < size; index++) {
		this->assemblerArray[0][index]->waitToFinish();
	}

	for (size_t index = 0; index < size; index++) {
		delete this->assemblerArray[0][index];
	}

	delete[] this->assemblerArray;
	delete this->consumingQueue;
	delete this->producingQueue;
	delete this->stopCondition;
}
