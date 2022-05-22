// Copyright 2021 Emmanuel D. Solis. Universidad de Costa Rica. CC BY 4.0

#include "GoldbachCalculator.hpp"

#include <math.h>     // Incluir funciones matematicas
#include <stdbool.h>  // Uso de booleanos
#include <stdio.h>
#include <stdlib.h>

#include <string>  // Para poder usar strings.
#include <vector>


GoldbachCalculator::GoldbachCalculator() { }
GoldbachCalculator::~GoldbachCalculator() {

}

bool GoldbachCalculator::esPrimo(int64_t number) {
    bool isPrime = true;

    if (number % 2 == 0) {
        isPrime = false;
    } else {
        int64_t root = sqrt(number);

        for (int64_t i = 3; i <= root; i += 2) {
            if (number % i == 0) {
                isPrime = false;
                break;
            }
        }
    }
    return isPrime;
}

std::vector<int64_t> * GoldbachCalculator::generatePrimeArray(int64_t amplitude) {
    auto* primeArray = new std::vector<int64_t>();

    if (!primeArray) {
        perror("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    primeArray->push_back(2);

    for (int64_t i = 3; i <= amplitude; i += 2) {
        if (esPrimo(i)) {
            primeArray->push_back(i);
        }
    }

    return primeArray;
}

void GoldbachCalculator::calculateGoldbach(SubList *sublist) {
    int64_t numero = sublist->getGoldbachNumber();
    std::vector<int64_t>* primeArray = generatePrimeArray(llabs(sublist->getGoldbachNumber()));

    // Compruebo que sea un numero NO menor que 5; asumo que es <64 bits.
    if (llabs(numero) <= 5) {
        return;
    } else {                     // En caso donde ya seria un numero valido.
        if (numero % 2 == 0) {     // Caso que sea par.
            solucionesPares(sublist, primeArray);
        } else {
            solucionesImpares(sublist, primeArray);
        }
    }
    delete primeArray;
}

void GoldbachCalculator::solucionesPares(SubList *sublist, std::vector<int64_t> *primeArray) {
    int64_t inputNumber = sublist->getGoldbachNumber();
    size_t primeArray_index1 = 0;

    int64_t candidate_1;
    int64_t candidate_2;

    if (inputNumber < 0) {
        inputNumber *= -1;
    }

    while (true) {
        candidate_1 = primeArray[0][primeArray_index1++];
        candidate_2 = (inputNumber - candidate_1);

        if (candidate_2 < candidate_1) {
            break;
        }

        if (esPrimo(candidate_2)) {
            sublist->insertSum(candidate_1, candidate_2, 0);
        }
    }
}

void GoldbachCalculator::solucionesImpares(SubList *sublist, std::vector<int64_t> *primeArray) {
    int64_t inputNumber = sublist->getGoldbachNumber();
    size_t primeArray_index1 = 1;
    size_t primeArray_index2 = 1;

    int64_t candidate_1 = 0;
    int64_t candidate_2;
    int64_t candidate_3;

    if (inputNumber < 0) {
        inputNumber *= -1;
    }

    while (candidate_1 <= inputNumber/3) {
        candidate_1 = primeArray[0][primeArray_index1++];

        while (true) {
            candidate_2 = primeArray[0][primeArray_index2++];
            candidate_3 = (inputNumber - candidate_2 - candidate_1);

            if (candidate_3 < candidate_2) {
                break;
            }

            if (esPrimo(candidate_3)) {
                sublist->insertSum(candidate_1, candidate_2, candidate_3);
            }
        }
        primeArray_index2 = primeArray_index1;
    }
}

