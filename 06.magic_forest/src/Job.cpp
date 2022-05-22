// Copyright 2021 Marco Piedra, Bryan Sandí, Emmanuel Solís

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

#include "Job.hpp"

const size_t MESSSAGE_LEN = 256;

Job::Job() {}

Job::~Job() {}

void Job::processJobs(std::string fileDir, int argc, char* argv[]) {
  // process_with_OMP(fileDir);
  process_with_MPI(fileDir, argc, argv);
}

void Job::process_with_OMP(std::string fileDir) {
  // Stores the file's path
  std::ifstream jobFile(fileDir);
  // Strings used to store values red from the txt file.
  std::string repetitionsString;
  std::string mapToEvaluate;
  // Value used to store an integer value wich is in charge of how many
  // times the map has to be red.
  int repetitions;

  while (jobFile >> mapToEvaluate >> repetitions) {
    const std::string& message = mapToEvaluate;
    assert(message.length() < MESSSAGE_LEN);
    map.readMapFile(message, repetitions);
    mapToEvaluate.clear();
  }
  map.~Map();
}

// #if 0
void Job::process_with_MPI(std::string fileDir, int argc, char* argv[]) {
  // Stores the file's path
  std::ifstream jobFile(fileDir);
  // Strings used to store values red from the txt file.
  std::string repetitionsString;
  std::string mapToEvaluate;
  // Value used to store an integer value wich is in charge of how many
  // times the map has to be red.
  int repetitions;

  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    // Common variables.
    int rank = -1;  // process_number
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int process_count = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    MPI_Status status;

    int flag = 1;
    const int COMPUTE = 1;
    const int STOP = 2;

    std::stringstream buffer;

    if (rank != 0) {
      while (flag == 1) {
        // Receive the value count from process 0
        int target = rank;
        if (MPI_Send(&target, /*count*/ 1, MPI_INT, /*target*/ 0
          , /*tag*/ 0, MPI_COMM_WORLD) == MPI_SUCCESS) {
          std::vector<char> message(MESSSAGE_LEN, '\0');
          MPI_Recv(&message[0], /*capacity*/ MESSSAGE_LEN, MPI_CHAR
            , /*source*/ 0, /*tag*/ MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          if (status.MPI_TAG == COMPUTE) {
            std::string map_name;
            for (uint32_t i=0; i < message.size(); i++) {
              map_name += message[i];
            }
            std::stringstream test(map_name);
            std::string segment;
            std::vector<std::string> seglist;
            while (std::getline(test, segment, '+')) {
              seglist.push_back(segment);
            }
            std::cout << "Nombre mapa: " << seglist[0] << " repeticiones "
              << seglist[1] << ", recibido por " << rank << std::endl;
            map.readMapFile(seglist[0], std::stoi(seglist[1]));
            std::vector<char> message_to_send(MESSSAGE_LEN, '\0');
            for (uint32_t i=0; i < seglist[0].length(); i++) {
              message_to_send[i] = seglist[0][i];
            }
            //map.readMapFile(&message_to_send[0], std::stoi(seglist[1]));
          } else {
            flag = 0;
            break;
          }
        }
      }
    } else {
      // Process 0 distribute the work.
      int target = 1;
      while (jobFile >> mapToEvaluate >> repetitions) {
        if (MPI_Recv(&target, /*capacity*/ 2, MPI_INT, MPI_ANY_SOURCE
          , /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) == MPI_SUCCESS) {
          std::string& message = mapToEvaluate;
          assert(message.length() < MESSSAGE_LEN);
          message += "+";
          message.append(std::to_string(repetitions));
          if (MPI_Send(message.data(), /*count*/ message.length(), MPI_CHAR
            , target, /*tag*/ COMPUTE, MPI_COMM_WORLD) == MPI_SUCCESS) {
          }
        }
      }
      flag = 0;
      for (int iterator=1; iterator < process_count; iterator++) {
        MPI_Send(&flag, /*count*/ 1, MPI_INT, /*target*/ iterator
          , /*tag*/ STOP, MPI_COMM_WORLD);
      }
    }
    MPI_Finalize();
  }
}
// #endif
