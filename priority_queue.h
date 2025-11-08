#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "structs.h"

// Create a new priority queue with initial capacity
PriorityQueue* pq_create(int capacity);

// Insert a job into the priority queue
int pq_insert(PriorityQueue* pq, Job* job);

// Extract and return the job with minimum priority (highest priority)
Job* pq_extract_min(PriorityQueue* pq);

// Peek at the minimum priority job without removing it
Job* pq_peek(PriorityQueue* pq);

// Check if priority queue is empty
int pq_is_empty(PriorityQueue* pq);

// Get the size of the priority queue
int pq_size(PriorityQueue* pq);

// Free the priority queue (does not free jobs themselves)
void pq_free(PriorityQueue* pq);

// Helper functions for heap operations
void heapify_up(PriorityQueue* pq, int index);
void heapify_down(PriorityQueue* pq, int index);

#endif // PRIORITY_QUEUE_H

