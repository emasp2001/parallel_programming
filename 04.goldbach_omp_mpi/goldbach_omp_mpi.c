  // Copyright 2021 Emmanuel D. Solis <emmanuel.solispomares@ucr.ac.cr> CC-BY-4
  // Creates solutions for the goldbach conjeture, for pairs and odds numbers.
  /**
 * @file main.c
 * @author Emmanuel D. Solis
 * @brief Solution for the goldbach conjeture.
 * @version 0.1
 * @date 2021-05-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <math.h>  // Incluir funciones matematicas
#include <stdbool.h>  // Uso de booleanos
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <omp.h>
// #include <mpi.h>
#include <stdint.h>
#include <inttypes.h>

  // Methods
  // Memory manage by list.
/**
 * @brief Used for implement list to save the solutions.
 */
struct Node;

/**
 * @brief introduce a new element to a list.
 * @param head_ref is reference (pointer to pointer) to the first link to the list.
 * @param new_num1 the first new number of one solution.
 * @param new_num2 the second new number of one solution.
 * @param new_num3 the third new number of one solution, used only for odd numbers.
 */
void append(struct Node** head_ref, int64_t new_num1, int64_t new_num2,
            int64_t new_num3);

/**
 * @brief liberates the memory asked for malloc.
 * @param node is the reference to the head of the list.
 */
void freeList(struct Node *node);

/**
 * @brief prints contents of linked list starting from head.
 * @param node a reference to the first link of the list.
 */
void printList(struct Node *node);

/**
 * @brief Call other funcstions to create a solution for one number.
 * @param numero the number to find the solution.
 */
void generadorSoluciones(int64_t numero);

/**
 * @brief Find out if a number is prime.
 * @param number greater that 1.
 * @return true if it's prime, 0 if it's not.
 */
bool esPrimo(int64_t number);

/**
 * @brief Find solutions only for pair numbers.
 * @param number the number to find the solution.
 * @param fRespuesta is 1 if what printed the solutions, 0 for just the amout of solutions.
 */
void solucionesPares(int64_t number, int16_t fRespuesta);

/**
 * @brief Find solutions only for odd numbers.
 * @param number the number to find the solution.
 * @param fRespuesta is 1 if what printed the solutions, 0 for just the amout of solutions.
 */
void solucionesImpares(int64_t number, int16_t fRespuesta);


int main(int argc, char* argv[]) {
  int64_t numeroLeido;
  // Version con Open MP solamente.
  // #if 0
  while (scanf("%li", &numeroLeido) == 1) {
    generadorSoluciones(numeroLeido);
  }
  // #endif

  // Version hibrida
  #if 0
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

    if (rank != 0) {
      while (flag == 1) {
        // Receive the value count from process 0
        int target = rank;
        if (MPI_Send(&target, /*count*/ 1, MPI_INT, /*target*/ 0
          , /*tag*/ 0, MPI_COMM_WORLD) == MPI_SUCCESS) {
          MPI_Recv(&numeroLeido, /*capacity*/ 1, MPI_INT
            , /*source*/ 0, /*tag*/ MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          if (status.MPI_TAG == COMPUTE) {
            printf("Numero %i recibido por %i\n", numeroLeido, rank);
            // generadorSoluciones(numeroLeido);
          } else {
            flag = 0;
            break;
          }
        }
      }
    } else {
      // Process 0 distribute the work.
      int target = 1;
      printf("Ingrese el numero que desea averiguar.\n");
      while (scanf("%li", &numeroLeido) == 1) {
        if (MPI_Recv(&target, /*capacity*/ 2, MPI_INT, MPI_ANY_SOURCE
          , /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) == MPI_SUCCESS) {
          if (MPI_Send(&numeroLeido, /*count*/ 1, MPI_INT
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
  #endif

  return 0;
}

bool esPrimo(int64_t number) {
  bool primo = true;
  if (number < 2) {
    primo = false;
  } else {
    if (number != 2 || number != 3 || number != 5 || number != 7 ||
        number != 11 || number != 13) {
      int64_t max = sqrt(number);
      for (int64_t i = 2; i <= max; i++) {
        if (number%i == 0) {
          primo = false;
          break;
        }
      }
    }
  }
  return primo;
}

void generadorSoluciones(int64_t numero) {
  int16_t tipoRespuesta;
    // Compruebo que sea un numero NO menor que 5; asumo que es <64 bits.
  if (numero <= 5 && numero >= -5) {
    printf("%li: NA\n", numero);
  } else {  // En caso donde ya seria un numero valido.
    if (numero > 0) {  // Solo se desea saber la cant de soluciones.
      tipoRespuesta = 0;
    } else {  // Se desean los resultados.
      tipoRespuesta = 1;
      numero *= -1;
    }
    if (numero%2 == 0) {  // Caso que sea par.
      solucionesPares(numero, tipoRespuesta);
    } else {
      solucionesImpares(numero, tipoRespuesta);
    }
  }
}

void solucionesPares(int64_t number, int16_t fRespuesta) {
  int64_t cantSoluciones = 0;
  struct Node* head = NULL;  // The head of the list.
    // Primero probamos el caso base que podria suceder, el 3.
  int64_t primerPrueba = number - 3;
  if (esPrimo(primerPrueba) == true) {
    cantSoluciones++;
    append(&head, 3, primerPrueba, 0);
  }
  // Pasamos a probar todos los demas posibles casos.
  #pragma omp parallel for default(none) shared(number, cantSoluciones, head)
  for (int64_t i = 6; i < number/2; i += 6) {
      // Por la probabilidad de 6n+-1->numPrimo.
    if (esPrimo(i-1) == true) {
      if (esPrimo(number - (i-1)) == true) {
        cantSoluciones++;
        #pragma omp critical
        append(&head, (i-1), (number - (i-1)), 0);
      }
    }
    if (esPrimo(i+1) == true) {
      if (esPrimo(number - (i+1)) == true) {
        cantSoluciones++;
        #pragma omp critical
        append(&head, (i+1), (number - (i+1)), 0);
      }
    }
  }
  if (fRespuesta == 1) {
    printf("-%li: %li sums:", number, cantSoluciones);
    printList(head);
  } else {
    printf("%li: %li sums", number, cantSoluciones);
  }
  printf("\n");
  freeList(head);
}

/**
 * @brief Find solutions only for odd numbers.
 * @param number the number to find the solution.
 * @param fRespuesta is 1 if what printed the solutions, 0 for just the amout of solutions.
 * @param cantSoluciones variable where to save the total of solutions.
 */
void solucionesImpares(int64_t number, int16_t fRespuesta) {
  int64_t cantSoluciones = 0;
  struct Node* head = NULL;  // The head of the list.
  int64_t max_iterador1 = (number/3);
  #pragma omp parallel for default(none) \
    shared(max_iterador1, number, cantSoluciones, head)
  for (int64_t i = 2; i < max_iterador1; i++) {
    for (int64_t j = i; j < (number-i-j-1); j++) {
      if (esPrimo(i)) {
        if (esPrimo(j) && esPrimo(number-i-j) &&
            (i + j + (number-i-j) == number)) {
          cantSoluciones++;
          #pragma omp critical
          append(&head, i, j, (number-i-j));
        }
      }
    }
  }
  if (fRespuesta == 1) {
    printf("-%li: %li sums:", number, cantSoluciones);
    printList(head);
  } else {
    printf("%li: %li sums", number, cantSoluciones);
  }
  printf("\n");
  freeList(head);
}

  // A linked list node
struct Node {
    // Codigo adaptado de: https://www.geeksforgeeks.org/linked-list-set-2-inserting-a-node/
  int64_t num1;
  int64_t num2;
  int64_t num3;
  struct Node *next;
};

void append(struct Node** head_ref, int64_t new_num1, int64_t new_num2,
            int64_t new_num3) {
  /* 1. allocate node */
  struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
  struct Node *last = *head_ref;  /* used in step 5*/
  /* 2. put in the data  */
  new_node->num1 = new_num1;
  new_node->num2 = new_num2;
  new_node->num3 = new_num3;
  /* 3. This new node is going to be the last node, so make next of
   it as NULL*/
  new_node->next = NULL;
  /* 4. If the Linked List is empty, then make the new node as head */
  if (*head_ref == NULL) {
    *head_ref = new_node;
    return;
  }
  /* 5. Else traverse till the last node */
  while (last->next != NULL)
    last = last->next;
  /* 6. Change the next of last node */
  last->next = new_node;
  return;
}

void printList(struct Node *node) {
  while (node != NULL) {
    if (node->num3 == 0) {  // Means that is a solution for a par number,
        // so only print the first 2.
      printf(" %li+%li, ", node->num1, node->num2);
    } else {
      printf(" %li+%li+%li, ", node->num1, node->num2, node->num3);
    }
    node = node->next;
  }
}

void freeList(struct Node *node) {
  while (node != NULL) {
    // Se mueve por toda la lista.
    struct Node *tmp = node->next;
    free(node);  // Libera la memoria.
    node = tmp;
  }
}
