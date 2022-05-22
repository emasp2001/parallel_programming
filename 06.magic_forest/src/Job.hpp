// Copyright 2021 Marco Piedra, Bryan Sandí, Emmanuel Solís

#ifndef PROYECTOS_MAGIC_FOREST_SRC_JOB_HPP_
#define PROYECTOS_MAGIC_FOREST_SRC_JOB_HPP_

#include <string>
#include <vector>

#include "Map.hpp"
#include <mpi.h>

/**
 * \brief Job processing.
 */
class Job {
 public:
  /// Constructor
  Job();
  /// Destructor
  ~Job();
  /**
   * \brief Process map jobs.
   * \param filepath Job filepath.
   */
  void processJobs(std::string fileDir, int argc, char* argv[]);

 private:
  /// Used to call Map class methods.
  Map map;
  void process_with_MPI(std::string fileDir, int argc, char* argv[]);
  void process_with_OMP(std::string fileDir);
};

#endif  // PROYECTOS_MAGIC_FOREST_SRC_JOB_HPP_
