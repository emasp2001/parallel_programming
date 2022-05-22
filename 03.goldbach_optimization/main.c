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
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "numbersQueue.h"

// --------------------- DECLARATION ---------------------------

int32_t cant_numbers;  // The numbers of elements to be analize.
int current_index;  // The index that the threads are analizing concurrently.
size_t numbers_of_threads;  // The number of threads to create.
// pthread_mutex_t can_access_position;
sem_t can_access_position;

typedef struct {
  int64_t thread_count;
  int64_t data_count;
  pthread_mutex_t can_access_position;
  int64_t ordered_print;
  queue_t data_queue;
} shared_data_t;

typedef struct {
  int64_t num_to_calculate;
  int64_t position_thread;
  int64_t cant_sums;
  int64_t prime_counter;
  queue_t queue_value_a;
  queue_t queue_value_b;
  queue_t queue_value_c;
  queue_t prime_queue;
  shared_data_t* shared_data;
} private_data_t;

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
void solucionesPares(void* data);

/**
 * @brief Find solutions only for odd numbers.
 * @param number the number to find the solution.
 * @param fRespuesta is 1 if what printed the solutions, 0 for just the amout of solutions.
 */
void solucionesImpares(void* data);

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
void print_sums();

/**
 * @brief Liberates the memory originally allocated.
 * @param number the first element.
 */
void free_number_list(struct Numeros *number);

void* goldbach(void* data);

// ----------------------- MAIN -------------------------


int main(int argc, char* argv[]) {

  int error = EXIT_SUCCESS;

  shared_data_t* shared_data = (shared_data_t*)
    calloc(1, sizeof(shared_data_t));

  pthread_mutex_init(&shared_data->can_access_position, NULL);

  int64_t number = 0ll;

  shared_data->data_count = 0;
  shared_data->thread_count = 0;

  // Si se logra reservar memoria, entonces continua.
  if (shared_data) {
    // Lee posibles parametros digitados por el usuario. Si no es así
    // entonces usa como valor la cantidad de núcleos para trabajar.
    if (argc == 2) {
      if (sscanf(argv[1], "%zu", &shared_data->thread_count) != 1
        || errno) {
        fprintf(stderr, "error: invalid thread count\n");
        error = EXIT_FAILURE;
      }
    } else {
      shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    }

    // Si lo logra continua, sino muestra un mensaje de error.
    if (error == EXIT_SUCCESS) {
      queue_init(&shared_data->data_queue);
      while (scanf("%" SCNd64, &number) == 1) {
        // Mete en las colas los valores del archivo .txt
        queue_enqueue(&shared_data->data_queue, number);
        // Cuenta la cantidad de valores que contiene el archivo txt. Esto con
        // el fin de poder tener una carga de trabajo definida. Será útil para
        // poder asignar el trabajo de manera dinámica.
        ++shared_data->data_count;
      }

      if (shared_data->thread_count == 0) {
        shared_data->thread_count = shared_data->data_count;
      }

      struct timespec start_time;
      clock_gettime(CLOCK_MONOTONIC, &start_time);

      error = create_threads(shared_data);

      struct timespec finish_time;
      clock_gettime(CLOCK_MONOTONIC, &finish_time);

      double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
        (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
      printf("Execution time: %lf\n", elapsed);

      queue_destroy(&shared_data->data_queue);
      pthread_mutex_destroy(&shared_data->can_access_position);
    } else {
      fprintf(stderr, "error: could not create threads\n");
      error = 3;
    }
  }
  free(shared_data);
  return error;

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

void* goldbach(void* data) {
  // Este valor se usa para almacenar el número al que se le va a calcular
  // las sumas de Goldbach.
  int64_t number = 0;

  private_data_t* private_data = (private_data_t*)data;
  shared_data_t* shared_data = private_data->shared_data;

  // Se usa el mutex para evitar una condición de carrera. Se saca de la cola
  // los valores de forma ordenada (como vienen en el documento o como los
  // ingrese el usuario) y se le relaciona a este número una posición usada
  // para imprimir los valores en orden.
  pthread_mutex_lock(&shared_data->can_access_position);

  queue_dequeue(&shared_data->data_queue, &number);
  private_data->position_thread = shared_data->ordered_print;
  ++shared_data->ordered_print;

  pthread_mutex_unlock(&shared_data->can_access_position);

  // Asigna el número a calcular a una variable de los datos privados de cada
  // hilo. Esto es para ser usada a la hora de la impresión.
  private_data->num_to_calculate = number;

  // Si el valor es negativo, se cambia a positivo para realizar los cálculos
  int64_t num = number;
  if (number < 0) {
    num *= -1;
  }

  // No permite el ingreso de números menores a 5. Imprime NA si son menores.
  if (num > 5) {
    // Líneas 240 a 247: Se encarga de almacenar en una cola todos los números
    // primos entre 2 (línea 241) y el número a calcular. Además usa un conta-
    // dor para saber la cantidad de primos. Ese contador se va a utilizar
    // para, posteriormente, almacenar estos primos en un arreglo.
    private_data->prime_counter = 1;
    queue_enqueue(&private_data->prime_queue, 2);
    for (int i = 3; i < num; i+=2) {
      if (esPrimo(i)) {
        queue_enqueue(&private_data->prime_queue, i);
        ++private_data->prime_counter;
      }
    }
    // Revisa si es par o impar.
    if (num % 2 == 0) {
      // Realiza cálculos con números pares.
      solucionesPares(&private_data[0]);
    } else {
      // Realiza cálculos con números impares.
      solucionesImpares(&private_data[0]);
    }
  }
  queue_destroy(&private_data->prime_queue);
  return NULL;
}

void solucionesPares(void* data) {
  // Set the values.
  private_data_t *private_data = (private_data_t*)data;
  int64_t number = private_data->num_to_calculate;
  int64_t sum_counter = 0;
  bool negative = number < 0;
  int64_t num = number;
  if(number<0){
    num *= -1;
  }

  int64_t primerPrueba = number - 3;
  if (esPrimo(primerPrueba) == true) {
    sum_counter++;
    queue_enqueue(&private_data->queue_value_a, primerPrueba); 
  }
    // Pasamos a probar todos los demas posibles casos.
  for (int64_t i = 6; i < number/2; i += 6) {
      // Por la probabilidad de 6n+-1->numPrimo.
    if (esPrimo(i-1) == true) {
      if (esPrimo(number - (i-1)) == true) {
        sum_counter++;
        if(negative) {
          queue_enqueue(&private_data->queue_value_a, i-1);
          queue_enqueue(&private_data->queue_value_b, number - (i-1));
        }
      }
    }
    if (esPrimo(i+1) == true) {
      if (esPrimo(number - (i+1)) == true) {
        sum_counter++;
        if(negative){
          queue_enqueue(&private_data->queue_value_a, i+1);
          queue_enqueue(&private_data->queue_value_b, number - (i+1));
        }
      }
    }
  }
  private_data->cant_sums = sum_counter;
}

void solucionesImpares(void* data) {
    // Set the values.
  private_data_t *private_data = (private_data_t*)data;
  int64_t number = private_data->num_to_calculate;
  int64_t sum_counter = 0;
  bool negative = number < 0;
  int64_t num = number;
  if(number<0){
    num *= -1;
  }

  //struct Node* head = NULL;  // The head of the list.
  int64_t max_iterador1 = (number/3);
  for (int64_t i = 2; i != max_iterador1; i++) {
    if (esPrimo(i)) {
      for (int64_t j = i; j < (number-i-j-1); j++) {
        if (esPrimo(j) && esPrimo(number-i-j) &&
            (i + j + (number-i-j) == number)) {
          sum_counter++;
          if (negative){
            queue_enqueue(&private_data->queue_value_a, i);
            queue_enqueue(&private_data->queue_value_a, j);
            queue_enqueue(&private_data->queue_value_c, (number-i-j));
          }
        }
      }
    }
  }
  private_data->cant_sums = sum_counter;
}

// --------------- PARALLEL SOLUTION -----------------

int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;

  // Ciclo encargado de repartir a los hilos de manera dinámica el trabajo a
  // realizar.
  while (shared_data->data_count > 0) {
    shared_data->ordered_print = 0;

    pthread_t* threads = (pthread_t*) calloc(shared_data->data_count,
      sizeof(pthread_t));

    private_data_t* private_data = (private_data_t*)
      calloc(shared_data->data_count, sizeof(private_data_t));

    if (threads && private_data) {
      // Revisa que la cantidad de hilos a crear no sea mayor que la
      // carga pendiente de trabajo, para evitar crear hilos sin trabajo.
      if (shared_data->data_count < shared_data->thread_count) {
        shared_data->thread_count = shared_data->data_count;
      }
      // Crea la cantidad de hilos solicitados.
      for (int64_t index = 0; index < shared_data->thread_count; index++) {
        private_data[index].shared_data = shared_data;
        queue_init(&private_data[index].queue_value_a);
        queue_init(&private_data[index].queue_value_b);
        queue_init(&private_data[index].queue_value_c);

        if (pthread_create(&threads[index], NULL, goldbach,
        &private_data[index]) == EXIT_SUCCESS) {
          --shared_data->data_count;
        } else {
          fprintf(stderr, "Fallo al crear hilo: %zu\n", index);
          error = 21;
          shared_data->thread_count = index;
          break;
        }
      }
      for (int64_t index = 0; index < shared_data->thread_count; index++) {
        pthread_join(threads[index], NULL);
      }
      // Método que organiza la impresión de los números para que estos se
      // impriman en orden. Se compara el índice con el valor de la posición
      // asignada a cada número, para que se impriman en el mismo orden que
      // vienen en el documento.
      for (int64_t index = 0; index < shared_data->thread_count; index++) {
        for (int64_t inner = 0; inner < shared_data->thread_count; inner++) {
          if (private_data[inner].position_thread == index) {
            print_sums(&private_data[inner]);
          }
        }
      }

      for (int64_t index = 0; index < shared_data->thread_count; index++) {
        queue_destroy(&private_data[index].queue_value_a);
        queue_destroy(&private_data[index].queue_value_b);
        queue_destroy(&private_data[index].queue_value_c);
      }
    } else {
      fprintf(stderr,
        "error: no se pudo reservar memoria para todos los hilos");
      error = 22;
    }
    free(threads);
    free(private_data);
  }
  return error;
}

void print_sums(void* data) {
  private_data_t* private_data = (private_data_t*)data;

  int64_t cant_sums = private_data->cant_sums;
  int64_t number = private_data->num_to_calculate;
  int64_t commaCounter = 1;

  bool negative = number < 0;

  printf("%ld: ", number);

  int64_t num = number;
  if (number < 0) {
    num *= -1;
  }

  if (num <= 5) {
    printf("NA\n");
  } else {
    printf("%ld sums", cant_sums);

    if (negative) {
      printf(": ");
      int64_t valueA = 0;
      int64_t valueB = 0;

      if (num % 2 == 0) {
        for (int64_t i = 0; i < cant_sums; i++) {
          queue_dequeue(&private_data->queue_value_a, &valueA);
          queue_dequeue(&private_data->queue_value_b, &valueB);
          printf("%ld + %ld", valueA, valueB);
          if (commaCounter < cant_sums) {
            printf(", ");
            commaCounter++;
          }
        }
        printf("\n");
      } else {
        int64_t valueC = 0;
        for (int64_t i = 0; i < cant_sums; i++) {
          queue_dequeue(&private_data->queue_value_a, &valueA);
          queue_dequeue(&private_data->queue_value_b, &valueB);
          queue_dequeue(&private_data->queue_value_c, &valueC);
          printf("%ld + %ld + %ld", valueA, valueB, valueC);
          if (commaCounter < cant_sums) {
            printf(", ");
            commaCounter++;
          }
        }
        printf("\n");
      }
    } else {
      printf("\n");
    }
  }
}