#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

// Structure to pass to hash table traversal callback
typedef struct {
    NodeList* nodes;
    HashTable* running_jobs;
    JobList* completed_jobs;
    int* completed_job_ids;
    int* completed_node_ids;
    int completed_count;
    int completed_capacity;
} UpdateContext;

// Callback function for updating running jobs
static void update_running_job(Job* job, int node_id, void* user_data) {
    UpdateContext* ctx = (UpdateContext*)user_data;
    
    if (!job || job->status != 1) { // Not running
        return;
    }
    
    // Decrement duration
    job->duration--;
    
    // If job is complete, mark it for removal (don't remove during traversal)
    if (job->duration == 0) {
        job->status = 2; // Mark as completed
        
        // Find the node and release resources
        ResourceNode* node = NULL;
        for (int i = 0; i < ctx->nodes->size; i++) {
            if (ctx->nodes->nodes[i]->node_id == node_id) {
                node = ctx->nodes->nodes[i];
                break;
            }
        }
        
        if (node) {
            // Release resources
            node->available_cpu += job->required_cpu;
            node->available_ram += job->required_ram;
        }
        
        // Store job ID and node ID for removal after traversal
        if (ctx->completed_count >= ctx->completed_capacity) {
            // Resize arrays
            int new_capacity = ctx->completed_capacity * 2;
            if (new_capacity == 0) new_capacity = 10;
            ctx->completed_job_ids = (int*)realloc(ctx->completed_job_ids, new_capacity * sizeof(int));
            ctx->completed_node_ids = (int*)realloc(ctx->completed_node_ids, new_capacity * sizeof(int));
            if (ctx->completed_job_ids && ctx->completed_node_ids) {
                ctx->completed_capacity = new_capacity;
            } else {
                return; // Failed to allocate
            }
        }
        
        ctx->completed_job_ids[ctx->completed_count] = job->job_id;
        ctx->completed_node_ids[ctx->completed_count] = node_id;
        ctx->completed_count++;
    }
}

void run_scheduler_tick(NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs, int current_time) {
    (void)current_time; // Suppress unused parameter warning (reserved for future use)
    if (!nodes || !pq || !running_jobs || !completed_jobs) {
        return;
    }
    
    // Phase 1: Update Running Jobs
    UpdateContext ctx;
    ctx.nodes = nodes;
    ctx.running_jobs = running_jobs;
    ctx.completed_jobs = completed_jobs;
    ctx.completed_job_ids = NULL;
    ctx.completed_node_ids = NULL;
    ctx.completed_count = 0;
    ctx.completed_capacity = 0;
    
    // Traverse the hash table to update all running jobs
    // This collects completed job IDs without modifying the hash table
    ht_traverse(running_jobs, update_running_job, &ctx);
    
    // Now remove completed jobs from hash table and add to completed list
    for (int i = 0; i < ctx.completed_count; i++) {
        Job* completed_job = ht_remove(running_jobs, ctx.completed_job_ids[i]);
        if (completed_job) {
            jl_add(completed_jobs, completed_job);
        }
    }
    
    // Free temporary arrays
    free(ctx.completed_job_ids);
    free(ctx.completed_node_ids);
    
    // Phase 2: Schedule New Jobs
    // Try to schedule as many jobs as possible (backfilling)
    while (1) {
        // Peek at the highest priority job
        Job* job = pq_peek(pq);
        if (!job) {
            break; // No more pending jobs
        }
        
        // Search for an available node
        int node_index = nl_find_available_node(nodes, job);
        
        if (node_index != -1) {
            // Found an available node, schedule the job
            Job* job_to_run = pq_extract_min(pq);
            if (!job_to_run) {
                break; // Should not happen, but safety check
            }
            
            // Mark job as running
            job_to_run->status = 1;
            
            // Deduct resources from the node
            ResourceNode* node = nl_get(nodes, node_index);
            if (node) {
                node->available_cpu -= job_to_run->required_cpu;
                node->available_ram -= job_to_run->required_ram;
                
                // Add to running jobs hash table
                ht_insert(running_jobs, job_to_run, node->node_id);
            }
            
            // Continue loop to try scheduling the next job (backfilling)
        } else {
            // No available node found, break the loop
            break;
        }
    }
}

void print_status(NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs) {
    printf("\n=== Scheduler Status ===\n\n");
    
    // Print nodes
    printf("Nodes:\n");
    if (!nodes || nodes->size == 0) {
        printf("  (none)\n");
    } else {
        for (int i = 0; i < nodes->size; i++) {
            ResourceNode* node = nl_get(nodes, i);
            printf("  Node %d: CPU %d/%d, RAM %d/%d\n",
                   node->node_id,
                   node->available_cpu, node->total_cpu,
                   node->available_ram, node->total_ram);
        }
    }
    
    // Print pending jobs
    printf("\nPending Jobs (Priority Queue):\n");
    if (pq_is_empty(pq)) {
        printf("  (none)\n");
    } else {
        // Create a temporary priority queue to print without modifying the original
        PriorityQueue* temp_pq = pq_create(pq->capacity);
        if (temp_pq) {
            // Copy jobs (we'll just print the count and peek at top)
            printf("  Total: %d jobs\n", pq_size(pq));
            Job* top = pq_peek(pq);
            if (top) {
                printf("  Next: Job %d (Priority=%d, CPU=%d, RAM=%d, Duration=%d)\n",
                       top->job_id, top->priority, top->required_cpu,
                       top->required_ram, top->duration);
            }
            pq_free(temp_pq);
        }
    }
    
    // Print running jobs
    printf("\nRunning Jobs:\n");
    int running_count = ht_size(running_jobs);
    if (running_count == 0) {
        printf("  (none)\n");
    } else {
        printf("  Total: %d jobs\n", running_count);
        // Print all running jobs
        for (int i = 0; i < running_jobs->size; i++) {
            HashNode* current = running_jobs->table[i];
            while (current) {
                printf("  Job %d: Priority=%d, CPU=%d, RAM=%d, Duration=%d (Node %d)\n",
                       current->job->job_id,
                       current->job->priority,
                       current->job->required_cpu,
                       current->job->required_ram,
                       current->job->duration,
                       current->node_id);
                current = current->next;
            }
        }
    }
    
    // Print completed jobs
    printf("\nCompleted Jobs:\n");
    jl_print(completed_jobs);
    
    printf("\n");
}

