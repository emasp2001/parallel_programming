// Copyright 2021 Bryan Sandí Barrantes <brayan.sandi@ucr.ac.cr> CC-BY-4
// Cola para almacenar los números recibidos en los archivos

#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

typedef int64_t queue_data_t;

typedef struct queue_node {
    queue_data_t data;
    struct queue_node* next;
} queue_node_t;

typedef struct {
    pthread_mutex_t can_access_queue;
    queue_node_t* head;
    queue_node_t* tail;
} queue_t;

int queue_init(queue_t* queue);
int queue_destroy(queue_t* queue);
bool queue_is_empty(queue_t* queue);
int queue_enqueue(queue_t* queue, const queue_data_t data);
int queue_dequeue(queue_t* queue, queue_data_t* data);
void queue_clear(queue_t* queue);

#endif  // QUEUE_H