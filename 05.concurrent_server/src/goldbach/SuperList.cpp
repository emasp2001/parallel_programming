//  Copyright 2021 Bryan Sandí
#include <vector>
#include <iostream>
#include "SuperList.hpp"

SuperList::SuperList() {
    this->inputArray = nullptr;
    this->completed = 0;
    this->canContinue = new Semaphore(0);
    this->control = new Semaphore(1);
}

SuperList::SuperList(std::vector<int64_t> *arrayOfInputs) {
    this->inputArray = arrayOfInputs;
    this->sublistArray = new std::vector<SubList*>();
    this->completed = 0;
    this->canContinue = new Semaphore(0);
    this->control = new Semaphore(1);

    for (size_t index = 0; index < arrayOfInputs[0].size(); index++) {
        this->insertSublist(this->inputArray[0][index]);
    }
}

SuperList::~SuperList() {
    this->control->wait();
    emptySuperList();
    this->control->signal();
    delete this->canContinue;
    delete this->control;
}

std::vector<int64_t> * SuperList::getInputArray() const {
    return this->inputArray;
}

size_t SuperList::getSuperListLength() const {
    this->control->wait();
    size_t size = this->sublistArray[0].size();
    this->control->signal();
    return size;
}

void SuperList::waitForCompletion() {
    this->canContinue->wait();
}

void SuperList::signalCompletion() {
    this->canContinue->signal();
}

void SuperList::insertSublist(int64_t inputNumber) {
    auto* newSublist = new SubList(inputNumber, this);

    this->sublistArray[0].push_back(newSublist);
}

void SuperList::emptySuperList() {
    SubList* temp;
    for (size_t index = 0; index < this->sublistArray[0].size(); index++) {
        temp = this->sublistArray[0][index];
        delete temp;
    }
    delete this->sublistArray;
}

size_t SuperList::getCompleted() {
    this->control->wait();
    size_t temp = this->completed;
    this->control->signal();
    return temp;
}

void SuperList::increaseCompleted() {
    this->control->wait();
    this->completed++;
    this->control->signal();
}

std::vector<SubList *> *SuperList::getSublistArray() const {
    return this->sublistArray;
}

/** ######################################### Sublist Class #########################################################*/

SubList::SubList() {
    this->goldbachNumber = 0;
    this->parent = nullptr;
    this->sumsArray = nullptr;
}

SubList::SubList(int64_t newGoldbachNumber, SuperList *newParent) {
    this->goldbachNumber = newGoldbachNumber;
    this->parent = newParent;
    this->sumsArray = new std::vector<Sum*>();
}

SubList::~SubList() {
    Sum* temp;
    if (this->sumsArray != nullptr) {
        for (size_t index = 0; index < this->sumsArray->size(); index++) {
            temp = this->sumsArray[0][index];
            delete temp;
        }
        delete this->sumsArray;
    }
}

void SubList::insertSum(int64_t first, int64_t second, int64_t third) {
    auto* newSum = new Sum(this->goldbachNumber, first, second, third);

    this->sumsArray[0].push_back(newSum);
}

int64_t SubList::getGoldbachNumber() const {
    return this->goldbachNumber;
}

void SubList::setGoldbachNumber(int64_t newGoldbachNumber) {
    SubList::goldbachNumber = newGoldbachNumber;
}

std::vector<Sum*> *SubList::getSumsArray() const {
    return this->sumsArray;
}

void SubList::setSumsArray(std::vector<Sum*> *newSumsArray) {
    SubList::sumsArray = newSumsArray;
}

SuperList *SubList::getParent() const {
    return this->parent;
}

void SubList::setParent(SuperList *newParent) {
    SubList::parent = newParent;
}

/** ######################################### Sum Class #########################################################*/

Sum::Sum() {
    this->goldbachNumber = 0;
    this->firstNumber = 0;
    this->secondNumber = 0;
    this->thirdNumber = 0;
}

Sum::Sum(int64_t newGoldbachNumber, int64_t newFirst, int64_t newSecond, int64_t newThird) {
    this->goldbachNumber = newGoldbachNumber;
    this->firstNumber = newFirst;
    this->secondNumber = newSecond;
    this->thirdNumber = newThird;
}

Sum::~Sum() {

}

int64_t Sum::getGoldbachNumber() {
    return this->goldbachNumber;
}

void Sum::setGoldbachNumber(int64_t newGoldbachNumber) {
    this->goldbachNumber = newGoldbachNumber;
}

int64_t Sum::getFirstNumber() {
    return this->firstNumber;
}

void Sum::setFirstNumber(int64_t newFirstNumber) {
    this->firstNumber = newFirstNumber;
}

int64_t Sum::getSecondNumber() {
    return this->secondNumber;
}

void Sum::setSecondNumber(int64_t newSecondNumber) {
    this->secondNumber = newSecondNumber;
}

int64_t Sum::getThirdNumber() {
    return this->thirdNumber;
}

void Sum::setThirdNumber(int64_t newThirdNumber) {
    this->thirdNumber = newThirdNumber;
}
