// Copyright 2021 Marco Piedra, Bryan Sandí, Emmanuel Solís

#ifndef PROYECTOS_MAGIC_FOREST_SRC_CONTROLLER_HPP_
#define PROYECTOS_MAGIC_FOREST_SRC_CONTROLLER_HPP_

#include <fstream>
#include <string>

#include "Job.hpp"

/**
 * \brief Map processing controller
 */
class Controller {
 public:
  /// Constructor
  Controller();
  /// Destructor
  ~Controller();
  /**
   * \brief Run job processing.
   * \param argc Argument count.
   * \param argv Argument values.
   * \return Error code.
   */
  int run(int argc, char* argv[]);

  Job job;
};

#endif  // PROYECTOS_MAGIC_FOREST_SRC_CONTROLLER_HPP_
