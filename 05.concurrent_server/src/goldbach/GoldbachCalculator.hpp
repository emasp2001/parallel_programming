// Copyright 2021 Emmanuel D. Solis. Universidad de Costa Rica. CC BY 4.0

#ifndef GOLDBACHCALCULATOR_H
#define GOLDBACHCALCULATOR_H

#include <stdio.h>
#include <string>
#include "SuperList.hpp"
#include <cmath>
#include <vector>

class GoldbachCalculator {
 private:
  /**
   * @brief Generates an array of prime numbers to optimize conjectures.
   * @param amplitude Amplitude of such array.
   * */
  std::vector<int64_t> * generatePrimeArray(int64_t amplitude);

  /**
   * @brief Find out if a number is prime.
   * @param number greater that 1.
   * @return true if it's prime, 0 if it's not.
   */
  bool esPrimo(int64_t number);

  /**
   * @brief Find solutions only for pair numbers.
   * @param sublist the number to find the solution.
   * @return the number of posible solutions for the number.
   */
  void solucionesPares(SubList *sublist, std::vector<int64_t> *primeArray);

  /**
   * @brief Find solutions only for odd numbers.
   * @param sublist the number to find the solution.
   * @return the number of posible solutions for the number.
   */
  void solucionesImpares(SubList *sublist, std::vector<int64_t> *primeArray);

 public:
  GoldbachCalculator();
  ~GoldbachCalculator();


  /**
   * @brief Call other funcstions to create a solution for one number.
   * @param sublist the number to find the solution.
   * @return the number of posible solutions for the number.
   */
  void calculateGoldbach(SubList *sublist);
};

#endif /* GOLDBACHCALCULATOR_H */
