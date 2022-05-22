// Copyright 2021 Emmanuel D. Solis <emmanuel.solispomares@ucr.ac.cr> CC-BY-4
  // Creates solutions for the goldbach conjeture, for pairs and odds numbers.
  /**
 * @file main.c
 * @author Emmanuel D. Solis
 * @brief Parallel olution for the goldbach conjeture.
 * @version 0.1
 * @date 2021-05-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <math.h>  // Incluir funciones matematicas
#include <stdbool.h>  // Uso de booleanos
#include <pthread.h>  // Most important library.
#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

// --------------------- DECLARATION ---------------------------

int32_t cant_numbers;  // The numbers of elements to be analize.
int current_index;  // The index that the threads are analizing concurrently.
size_t numbers_of_threads;  // The number of threads to create.
// pthread_mutex_t can_access_position;
sem_t can_access_position;

/**
 * @brief Used for implement list to save the solutions.
 */
struct Node {
    // Codigo adaptado de: https://www.geeksforgeeks.org/linked-list-set-2-inserting-a-node/
  int64_t num1;
  int64_t num2;
  int64_t num3;
  struct Node *next;
};

/**
 * @brief Used to implement a list of the number available to solve.
 */
struct Numeros {
  int32_t indice;
  int64_t numero;
  struct Node *soluciones;
  int32_t cant_soluciones;
  struct Numeros* next;
};

struct Numeros *head;


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
 * @brief introduce a new number to solve later to a list.
 * @param head_ref is reference (pointer to pointer) to the first link to the list.
 * @param index is a number given to each node to search each one later.
 * @param number is the number to be solved.
 */
void append_new_number(struct Numeros** head_ref, int32_t index,
  int64_t number);

/**
 * @brief Reads the asked numbers and put them in a list to consume later.
 * @return struct Numeros* as the ref to the first element to be consume.
 */
struct Numeros *read_numbers();

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
 * @brief Prints the numbers of the list saving all the numbers to consume.
 * @param node a reference to the first link of the list.
 */
void printNumberList(struct Numeros *node);

/**
 * @brief Call other funcstions to create a solution for one number.
 * @param numero the number to find the solution.
 */
void generadorSoluciones(int64_t numero, struct Numeros *my_node);

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
void solucionesPares(int64_t number, int16_t fRespuesta, struct Numeros *node);

/**
 * @brief Find solutions only for odd numbers.
 * @param number the number to find the solution.
 * @param fRespuesta is 1 if what printed the solutions, 0 for just the amout of solutions.
 */
void solucionesImpares(int64_t number, int16_t fRespuesta,
  struct Numeros *node);

/**
 * @brief Analize in case of receiving the desire number of threads by parameter.
 * @details In case of not receiving it uses the numbers of cores as number of threads.
 * @return int32_t as the number of threads.
 */
int32_t analize_arguments(int argc, char* argv[]);

/**
 * @brief Get the object element for the specific index.
 * @param node is the head of the numbers list.
 * @param index is the searched index.
 * @return struct Numeros* as a reference.
 */
struct Numeros *get_number_index(struct Numeros *node, int32_t index);

/**
 * @brief Create a threads object.
 * @return int as an error message.
 */
int create_threads();

/**
 * @brief The subroutine for each thread to execute.
 * @return void* as require for pthread library.
 */
void* run();

/**
 * @brief Prints all the solutions created.
 */
void print_all();

/**
 * @brief Liberates the memory originally allocated.
 * @param number the first element.
 */
void free_number_list(struct Numeros *number);

// ----------------------- MAIN -------------------------


int main(int argc, char* argv[]) {
  head = read_numbers();
  numbers_of_threads = analize_arguments(argc, argv);

  int error = create_threads();
  print_all();
  free_number_list(head);
  return error;
}

// ----------------------- IMPLEMENTATION -------------------------

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

void generadorSoluciones(int64_t numero, struct Numeros *my_node) {
  int16_t tipoRespuesta;
    // Compruebo que sea un numero NO menor que 5; asumo que es <64 bits.
  if (numero <= 5 && numero >= -5) {
    my_node->cant_soluciones = 0;
    my_node->soluciones = NULL;  // Cambiarlo para imprimir un NA.
  } else {  // En caso donde ya seria un numero valido.
    if (numero > 0) {  // Solo se desea saber la cant de soluciones.
      tipoRespuesta = 0;
    } else {  // Se desean los resultados.
      tipoRespuesta = 1;
      numero *= -1;
    }
    if (numero%2 == 0) {  // Caso que sea par.
      solucionesPares(numero, tipoRespuesta, my_node);
    } else {
      solucionesImpares(numero, tipoRespuesta, my_node);
    }
  }
}

void solucionesPares(int64_t number, int16_t fRespuesta, struct Numeros *node) {
  int64_t cantSoluciones = 0;
  struct Node* head = NULL;  // The head of the list.
    // Primero probamos el caso base que podria suceder, el 3.
  int64_t primerPrueba = number - 3;
  if (esPrimo(primerPrueba) == true) {
    cantSoluciones++;
    append(&head, 3, primerPrueba, 0);
  }
    // Pasamos a probar todos los demas posibles casos.
  for (int64_t i = 6; i < number/2; i += 6) {
      // Por la probabilidad de 6n+-1->numPrimo.
    if (esPrimo(i-1) == true) {
      if (esPrimo(number - (i-1)) == true) {
        cantSoluciones++;
        append(&head, (i-1), (number - (i-1)), 0);
      }
    }
    if (esPrimo(i+1) == true) {
      if (esPrimo(number - (i+1)) == true) {
        cantSoluciones++;
        append(&head, (i+1), (number - (i+1)), 0);
      }
    }
  }
  node->cant_soluciones = cantSoluciones;
  if (fRespuesta == 1) {
    node->soluciones = head;
  } else {
    node->soluciones = NULL;
    freeList(head);
  }
}

void solucionesImpares(int64_t number, int16_t fRespuesta,
  struct Numeros *node) {
  int64_t cantSoluciones = 0;
  struct Node* head = NULL;  // The head of the list.
  int64_t max_iterador1 = (number/3);
  for (int64_t i = 2; i != max_iterador1; i++) {
    if (esPrimo(i)) {
      for (int64_t j = i; j < (number-i-j-1); j++) {
        if (esPrimo(j) && esPrimo(number-i-j) &&
            (i + j + (number-i-j) == number)) {
          cantSoluciones++;
          append(&head, i, j, (number-i-j));
        }
      }
    }
  }
  node->cant_soluciones = cantSoluciones;
  if (fRespuesta == 1) {
    node->soluciones = head;
  } else {
    node->soluciones = NULL;
    freeList(head);
  }
}

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

void append_new_number(struct Numeros** head_ref, int32_t index,
  int64_t number) {
  /* 1. allocate node */
  struct Numeros* new_node = (struct Numeros*) malloc(sizeof(struct Numeros));
  struct Numeros *last = *head_ref;  /* used in step 5*/
  /* 2. put in the data  */
  new_node->indice = index;
  new_node->cant_soluciones = 0;
  new_node->numero = number;
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

void printNumberList(struct Numeros *node) {
  while (node != NULL) {
    printf("%li, ", node->numero);
    node = node->next;
  }
  printf("\nPrinted all the numbers.\n");
}

void freeList(struct Node *node) {
  while (node != NULL) {
    // Se mueve por toda la lista.
    struct Node *tmp = node->next;
    free(node);  // Libera la memoria.
    node = tmp;
  }
}

void free_number_list(struct Numeros *number) {
  while (number != NULL) {
    // Se mueve por toda la lista.
    struct Numeros *tmp = number->next;
    free(number);  // Libera la memoria.
    freeList(number->soluciones);
    number = tmp;
  }
}

// --------------- PARALLEL SOLUTION -----------------

struct Numeros *read_numbers() {
  struct Numeros *head = NULL;
  int32_t cant_numeros = 0;
  int64_t numeroLeido;
  printf("Ingrese el numero que desea averiguar.\n");
  // Read the numbers.
  while (scanf("%li", &numeroLeido) == 1) {
    // generadorSoluciones(numeroLeido);
    append_new_number(&head, cant_numeros++, numeroLeido);
  }
  cant_numbers = cant_numeros;
  return head;
}

int32_t analize_arguments(int argc, char* argv[]) {
  int32_t number_of_threads = 0;
  if (argc == 2) {
    sscanf(argv[1], "%i", &number_of_threads);
  } else {
    number_of_threads = sysconf(_SC_NPROCESSORS_ONLN);
  }
  return number_of_threads;
}

struct Numeros *get_number_index(struct Numeros *node, int32_t index) {
  while (node != NULL) {
    if (node->indice == index) {
      break;
    } else {
      node = node->next;
    }
  }
  return node;
}

int create_threads() {
  int error = EXIT_SUCCESS;
  pthread_t* threads = (pthread_t*) calloc(numbers_of_threads
    , sizeof(pthread_t));
  if (threads) {
    for (size_t index = 0; index < numbers_of_threads; ++index) {
      if (error == EXIT_SUCCESS) {
        if (pthread_create(&threads[index], /*attr*/ NULL, run
          , NULL) == EXIT_SUCCESS) {
        } else {
          fprintf(stderr, "error: could not create thread %zu\n", index);
          error = 21;
          numbers_of_threads = index;
          break;
        }
      } else {
        fprintf(stderr, "error: could not init semaphore %zu\n", index);
        error = 22;
        numbers_of_threads = index;
        break;
      }
    }

    for (size_t index = 0; index < numbers_of_threads; ++index) {
      pthread_join(threads[index], /*value_ptr*/ NULL);
    }
    free(threads);
  } else {
    fprintf(stderr, "error: could not allocate memory for %zu threads\n"
      , numbers_of_threads);
    error = 22;
  }
  return error;
}

void* run() {
  // Do heavy task
  while (current_index != cant_numbers) {
    if (current_index == cant_numbers) {
      break;
    } else {
      sem_wait(&can_access_position);
      int tmp_index = current_index;
      current_index++;
      sem_post(&can_access_position);
      struct Numeros *tmp = get_number_index(head, tmp_index);
      generadorSoluciones(tmp->numero, tmp);
    }
  }
  return NULL;
}

void print_all() {
  while (head != NULL) {
    if (head->soluciones != NULL) {
      printf("%li %i sums: ", head->numero, head->cant_soluciones);
      printList(head->soluciones);
      printf("\n");
    } else {
      printf("%li %i sums\n", head->numero, head->cant_soluciones);
    }
    head = head->next;
  }
}