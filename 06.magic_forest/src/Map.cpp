// Copyright 2021 Marco Piedra, Bryan Sandí, Emmanuel Solís
#include <fstream>
#include <iostream>
#include <utility>

#include "Map.hpp"

Map::Map() {}

Map::~Map() {}

void Map::readMapFile(std::string fileName, int iterate) {
  std::string filePath = "test/input/";
  filePath += fileName;
  // Opens the file.
  std::ifstream mapFile(filePath);
  // Stores the original file name in a variable used later.
  fileOriginalName = fileName;

  // Used to store a row red from the file.
  std::string tempRow;
  std::string tempColumn;
  std::string numOfIterations = std::to_string(iterate);

  // Reads the first values of the file, wich are the size of the matrix.
  mapFile >> tempRow >> tempColumn;

  // Change string values for int.
  rowCount = stoi(tempRow);
  columnCount = stoi(tempColumn);

  bool printEveryMap = true;

  if (iterate < 0) {
    printEveryMap = false;
    iterate = -iterate;
  }

  for (int index = 0; index < rowCount; index++) {
    mapFile >> tempRow;
    tempRow += "\n";
    initialMap.push_back(tempRow);
    finalMap.push_back(tempRow);
  }
  mapFile.close();

  if (printEveryMap) {
    writeSeveralMapFiles(initialMap, 0);
  } else {
    fileOriginalName += "-";
    fileOriginalName += std::to_string(iterate);
    singleFinalFile.open(fileOriginalName.c_str(), std::fstream::out);
    writeSingleMapFile(initialMap, 0);
  }
  evaluateMap(iterate, printEveryMap);
}

void Map::evaluateMap(int iterations, bool printEveryMap) {
  for (int repetitions = 1; repetitions <= iterations; repetitions++) {
    #pragma omp parallel for default(none) collapse(2) schedule(static) \
      shared(rowCount, columnCount)
    for (int row = 0; row < rowCount; row++) {
      for (int column = 0; column < columnCount; column++) {
        finalMap[row][column] = applyRules(row, column);
      }
    }
    if (printEveryMap) {
      writeSeveralMapFiles(finalMap, repetitions);
    }
    //std::swap(initialMap, finalMap);
    initialMap.swap(finalMap);
  }
  if (!printEveryMap) {
    //std::swap(initialMap, finalMap);
    initialMap.swap(finalMap);
    writeSingleMapFile(finalMap, iterations);
    singleFinalFile.close();
  }
}

char Map::applyRules(int row, int column) {
  // Feature at the center of the region to evaluate (size 3x3)
  char featureToChange = initialMap[row][column];

  // Evaluate a tree and its surroundings
  if (featureToChange == TREE) {
    // If a tree is surrounded by too many lakes, it will flood
    if (countFeatures(row, column, LAKE) >= 4) {
      featureToChange = LAKE;
    }
    // If a tree is surrounded by too many trees, it will become a meadow
    if (countFeatures(row, column, TREE) > 4) {
      featureToChange = MEADOW;
    }
  } else {
    if (featureToChange == LAKE) {  // Evaluate a lake and its surroundings
    // If a lake is surrounded by too few lakes, it will become a meadow
      if (countFeatures(row, column, LAKE) < 3) {
        featureToChange = MEADOW;
      }
    } else {
      // Evaluate a meadow and its surroundings
      if (featureToChange == MEADOW) {
        // If a meadow is surrounded by too many trees, it will become a tree
        if (countFeatures(row, column, TREE) >= 3) {
          featureToChange = TREE;
        }
      }
    }
  }
  return featureToChange;
}

int Map::countFeatures(size_t row, size_t column, char feature) {
  // This counter stores the amount of a especific fetaure around a cell.
  int featureCounter = 0;

  for (int index = 0; index < COUNT_AREA ; index++) {
    // Store the axe's values
    int currentRow = row + HORIZONTAL_OFFSET[index];
    int currentColumn = column + VERTICAL_OFFSET[index];

    // Makes sure it doesn't check cells beyond the edges.
    if ((currentRow >= 0) && (currentRow < rowCount)) {
      if ((currentColumn >= 0) && (currentColumn < columnCount)) {
        // If the value inside the cell equals the feature to count, it
        // increases the counter.
        if (initialMap[currentRow][currentColumn] == feature) {
          featureCounter++;
        }
      }
    }
  }
  return featureCounter;
}

void Map::writeSeveralMapFiles
  (std::vector<std::string> mapToWrite, int excNumber) {
  std::string tempString = fileOriginalName;
  std::ofstream finalFile;
  fileOriginalName += "-";
  fileOriginalName += std::to_string(excNumber);


  finalFile.open(fileOriginalName.c_str(), std::fstream::out);

  finalFile << std::to_string(excNumber) << ":" << std::endl;

  for (int i = 0; i < rowCount; i++) {
    finalFile << mapToWrite[i];
  }
  finalFile << std::endl;
  finalFile.close();
  fileOriginalName = tempString;
}

void Map::writeSingleMapFile
  (std::vector<std::string> mapToWrite, int excNumber) {
  singleFinalFile << std::to_string(excNumber) << ":" << std::endl;
  for (int i = 0; i < rowCount; i++) {
    singleFinalFile << mapToWrite[i];
  }
  singleFinalFile << std::endl;
}
