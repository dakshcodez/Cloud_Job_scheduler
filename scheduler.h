#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "structs.h"
#include "node_list.h"
#include "priority_queue.h"
#include "hash_table.h"
#include "job_list.h"

// Run one tick of the scheduler
// Phase 1: Update running jobs (decrement duration, move completed jobs)
// Phase 2: Schedule new jobs from priority queue
void run_scheduler_tick(NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs, int current_time);

// Print the status of all queues and nodes
void print_status(NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs);

#endif // SCHEDULER_H

