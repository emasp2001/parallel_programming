// Copyright 2021 Marco Piedra, Bryan Sandí, Emmanuel Solís

#include <iostream>
#include <fstream>
#include <string>

#include "Controller.hpp"

Controller::Controller() {}

Controller::~Controller() {}

int Controller::run(int argc, char* argv[]) {
  // Used to store the file's directory.
  std::string fileDir;
  // Used to show an error.
  int error = EXIT_SUCCESS;
  // Anilyze arguments.
  if (argc == 2) {
    fileDir = argv[1];
  } else {
    error = EXIT_FAILURE;
  }
  job.processJobs(fileDir, argc, argv);
  job.~Job();

  return error;
}
