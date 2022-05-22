// Copyright 2021 Marco Piedra, Bryan Sandí, Emmanuel Solís

#ifndef PROYECTOS_MAGIC_FOREST_SRC_MAP_HPP_
#define PROYECTOS_MAGIC_FOREST_SRC_MAP_HPP_

#include <string>
#include <vector>
#include <fstream>

/**
 * \brief Forest map.
 * Consists of map features represented as characters
 * (t: tree, l: lake, -: meadow)
 */
class Map {
 private:
  /// Number of map rows
  int rowCount;
  /// Number of map columns
  int columnCount;
  /// Initial map content
  std::vector<std::string> initialMap;
  /// Final map content
  std::vector<std::string> finalMap;
  /// Map file name
  std::string fileOutputName;
  /// This variable will be used to store the original file name.
  std::string fileOriginalName;

  std::ofstream singleFinalFile;

  /// File to write the new matrix

  /// Tree feature
  const char TREE = 'a';
  /// Lake feature
  const char LAKE = 'l';
  /// Meadow feature
  const char MEADOW = '-';
  /// Maximum amount of cells that need to be counted.
  const int COUNT_AREA = 8;
  /// Horizontal offset array.
  const int HORIZONTAL_OFFSET[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  /// Vertical offset array.
  const int VERTICAL_OFFSET[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

  /**
   * \brief Evaluate a map.
   * \param iterations Number of nights to evaluate.
   */
  void evaluateMap(int iterations, bool printEveryMap);
  /**
   * \brief Write finalMap to a file.
   */
  void writeSingleMapFile(std::vector<std::string> mapToWrite, int excNumber);
  /**
   * \brief Write finalMap to a file.
   */
  void writeSeveralMapFiles(std::vector<std::string> mapToWrite, int excNumber);
  /**
   * \brief Apply map evaluation rules on a map cell.
   * \param row Map row.
   * \param column Map column.
   */
  char applyRules(int row, int column);
  /**
   * \brief Count surrounding map features.
   * \param row Map row.
   * \param column Map column.
   * \param feature Map feature to count.
   */
  int countFeatures(size_t row, size_t column, char feature);

  void createOfstreamFile();

 public:
  /// Constructor
  Map();
  /// Copy constructor
  Map(const Map& map);
  /// Move constructor
  Map(const Map&& map);
  /// Destructor
  ~Map();
  /**
   * \brief Write finalMap to a file.
   */
  void readMapFile(std::string filepath, int iterate);
};

#endif  // PROYECTOS_MAGIC_FOREST_SRC_MAP_HPP_
