#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "structs.h"
#include "node_list.h"
#include "priority_queue.h"
#include "hash_table.h"
#include "job_list.h"

// Save the current state to a file
// Returns 1 on success, 0 on failure
int save_state(const char* filename, NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs, int current_time, int next_job_id);

// Load the state from a file
// Returns 1 on success, 0 on failure
// The loaded state will be stored in the provided pointers
int load_state(const char* filename, NodeList** nodes, PriorityQueue** pq, HashTable** running_jobs, JobList** completed_jobs, int* current_time, int* next_job_id);

#endif // PERSISTENCE_H

