//  Copyright 2021 Bryan Sandí

#ifndef LIST_H
#define LIST_H


#include <cstdlib>
#include <cstdio>
#include <mutex>
#include <vector>
#include <Semaphore.hpp>

class SuperList;
class SubList;
class Sum;

/**
* @brief SuperList class. Contains SubLists.
**/
class SuperList {
public:
    /**
     * @brief Default Constructor for SuperList.
     **/
    SuperList();
    /**
     * @brief Constructor for SuperList.
     * @param arrayOfInputs Array of values input by the user.
     * @param length length of arrayOfInputs.
     **/
    SuperList(std::vector<int64_t> *arrayOfInputs);

    /**
     * @brief Destructor for SuperList.
     **/
    virtual ~SuperList();

    /**
     * @brief Getter for user inputs array.
     **/
    std::vector<int64_t> * getInputArray() const;

    /**
     * @brief Getter for SuperList length.
     **/
    size_t getSuperListLength() const;

    /**
     * @brief Getter for Completed.
     **/
    size_t getCompleted();

    /**
     * @brief Getter for SubList array.
     * */
    std::vector<SubList *> *getSublistArray() const;

    /**
     * @brief Increases Completed value by 1.
     **/
    void increaseCompleted();

    /**
     * @brief Unlocks this SuperList's Semaphore. Called when
     * all elements inside this SuperList have been processed.
     **/
    void waitForCompletion();

    /**
     * @brief Locks this SuperList's Semaphore. Called after
     * sending all elements inside this SuperList into goldbachConsumers
     * line, program stops execution until all elements have been
     * processed.
     **/
    void signalCompletion();

private:
    std::vector<int64_t>* inputArray;
    std::vector<SubList*>* sublistArray;
    size_t completed;
    Semaphore* canContinue;
    Semaphore* control;

    /**
     * @brief Creates a new SubList inside this SuperList.
     * @param inputNumber Input number each sublist has to work with.
     **/
    void insertSublist(int64_t inputNumber);

    /**
     * @brief Deletes all SubLists from this Superlist. Called from
     * SuperList destructor.
     **/
    void emptySuperList();
};

/**
 * @brief Container sub-class to store Goldbach Sums for each number.
 * */
class SubList{
public:
    /**
     * @brief Default constructor for subList.
     * */
    SubList();

    /**
     * @brief Constructor for Sublist
     * @param newGoldbachNumber Goldbach Number to calculate
     * @param newParent Pointer to parent SuperList
     * */
    SubList(int64_t newGoldbachNumber, SuperList* newParent);

    /**
     * @brief Destructor for Sublist
     * */
    virtual ~SubList();

    /**
     * @brief Inserts new sum to this container.
     * @param first First element for sum.
     * @param second Second element for sum.
     * @param third Third element for sum.
     * */
    void insertSum (int64_t first, int64_t second, int64_t third);

    int64_t getGoldbachNumber() const;

    void setGoldbachNumber(int64_t newGoldbachNumber);

    std::vector<Sum*> *getSumsArray() const;

    void setSumsArray(std::vector<Sum*> *newSumsArray);

    SuperList *getParent() const;

    void setParent(SuperList *newParent);


private:
    int64_t goldbachNumber;
    std::vector<Sum*>* sumsArray;
    SuperList* parent;
};

class Sum{
public:

    /**
     * @brief Default constructor for Sum
     * */
    Sum();

    /**
     * @brief Constructor for Sum
     * */
    Sum(int64_t newGoldbachNumber, int64_t newFirst, int64_t newSecond, int64_t newThird);

    /**
     * @brief Destructor for Sum
     * */
    virtual ~Sum();

    int64_t getGoldbachNumber();

    void setGoldbachNumber(int64_t newGoldbachNumber);

    int64_t getFirstNumber();

    void setFirstNumber(int64_t newFirstNumber);

    int64_t getSecondNumber();

    void setSecondNumber(int64_t newSecondNumber);

    int64_t getThirdNumber();

    void setThirdNumber(int64_t newThirdNumber);

private:
    int64_t goldbachNumber;
    int64_t firstNumber;
    int64_t secondNumber;
    int64_t thirdNumber;
};

#endif  /// LIST_H
