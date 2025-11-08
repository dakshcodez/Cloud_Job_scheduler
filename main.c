#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "node_list.h"
#include "priority_queue.h"
#include "hash_table.h"
#include "job_list.h"
#include "scheduler.h"
#include "persistence.h"

#define MAX_LINE_LENGTH 1024
#define MAX_FILENAME_LENGTH 256

// Global state
static int current_time = 0;
static int next_job_id = 1;

// Function to free all allocated memory
void cleanup(NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs) {
    // Use a simple approach: collect all job pointers first, then free them once
    // This avoids double-freeing when jobs move between structures
    
    // First, collect all unique job pointers
    #define MAX_JOBS 10000
    Job* jobs_to_free[MAX_JOBS];
    int job_count = 0;
    
    // Collect jobs from priority queue
    while (!pq_is_empty(pq) && job_count < MAX_JOBS) {
        Job* job = pq_extract_min(pq);
        if (job) {
            jobs_to_free[job_count++] = job;
        }
    }
    
    // Collect jobs from hash table (running jobs that haven't completed)
    for (int i = 0; i < running_jobs->size && job_count < MAX_JOBS; i++) {
        HashNode* current = running_jobs->table[i];
        while (current && job_count < MAX_JOBS) {
            if (current->job) {
                jobs_to_free[job_count++] = current->job;
            }
            current = current->next;
        }
    }
    
    // Collect jobs from completed list
    JobNode* current = completed_jobs->head;
    while (current && job_count < MAX_JOBS) {
        if (current->job) {
            // Check if we already have this job (shouldn't happen, but be safe)
            int found = 0;
            for (int i = 0; i < job_count; i++) {
                if (jobs_to_free[i] == current->job) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                jobs_to_free[job_count++] = current->job;
            }
        }
        current = current->next;
    }
    
    // Free all collected jobs (each job freed exactly once)
    for (int i = 0; i < job_count; i++) {
        free(jobs_to_free[i]);
    }
    
    // Free nodes
    for (int i = 0; i < nodes->size; i++) {
        free(nodes->nodes[i]);
    }
    
    // Free data structures (these will free HashNodes and JobNodes, but not jobs)
    nl_free(nodes);
    pq_free(pq);
    ht_free(running_jobs);  // Frees HashNode structures only
    jl_free(completed_jobs);  // Frees JobNode structures only
}

// Parse and execute commands
// Returns: 1 to continue, 0 to exit, 2 if pointers were updated (need to restart)
int execute_command(char* line, NodeList** nodes_ptr, PriorityQueue** pq_ptr, HashTable** running_jobs_ptr, JobList** completed_jobs_ptr) {
    NodeList* nodes = *nodes_ptr;
    PriorityQueue* pq = *pq_ptr;
    HashTable* running_jobs = *running_jobs_ptr;
    JobList* completed_jobs = *completed_jobs_ptr;
    char command[64];
    char arg1[256];
    int arg2, arg3, arg4, arg5;
    
    // Remove newline
    line[strcspn(line, "\n")] = 0;
    
    // Parse command
    if (sscanf(line, "%63s", command) != 1) {
        return 1; // Continue loop
    }
    
    if (strcmp(command, "add-node") == 0) {
        if (sscanf(line, "add-node %d %d", &arg2, &arg3) != 2) {
            printf("Error: Usage: add-node <cpu> <ram>\n");
            return 1;
        }
        
        if (arg2 <= 0 || arg3 <= 0) {
            printf("Error: CPU and RAM must be positive integers\n");
            return 1;
        }
        
        ResourceNode* node = (ResourceNode*)malloc(sizeof(ResourceNode));
        if (!node) {
            printf("Error: Failed to allocate memory for node\n");
            return 1;
        }
        
        node->node_id = nodes->size + 1;
        node->total_cpu = arg2;
        node->total_ram = arg3;
        node->available_cpu = arg2;
        node->available_ram = arg3;
        
        if (!nl_add(nodes, node)) {
            printf("Error: Failed to add node\n");
            free(node);
            return 1;
        }
        
        printf("Added node %d: CPU=%d, RAM=%d\n", node->node_id, node->total_cpu, node->total_ram);
        
    } else if (strcmp(command, "add-job") == 0) {
        if (sscanf(line, "add-job %d %d %d %d", &arg2, &arg3, &arg4, &arg5) != 4) {
            printf("Error: Usage: add-job <priority> <cpu> <ram> <duration>\n");
            return 1;
        }
        
        if (arg2 < 0 || arg3 <= 0 || arg4 <= 0 || arg5 <= 0) {
            printf("Error: Priority must be non-negative, and CPU, RAM, and duration must be positive\n");
            return 1;
        }
        
        // Check if any node can handle this job
        int max_cpu = 0, max_ram = 0;
        for (int i = 0; i < nodes->size; i++) {
            ResourceNode* node = nl_get(nodes, i);
            if (node->total_cpu > max_cpu) max_cpu = node->total_cpu;
            if (node->total_ram > max_ram) max_ram = node->total_ram;
        }
        
        if (nodes->size == 0) {
            printf("Error: No nodes available. Add nodes first.\n");
            return 1;
        }
        
        if (arg3 > max_cpu || arg4 > max_ram) {
            printf("Error: Job requires more resources (CPU=%d, RAM=%d) than any node can provide (max CPU=%d, max RAM=%d)\n",
                   arg3, arg4, max_cpu, max_ram);
            return 1;
        }
        
        Job* job = (Job*)malloc(sizeof(Job));
        if (!job) {
            printf("Error: Failed to allocate memory for job\n");
            return 1;
        }
        
        job->job_id = next_job_id++;
        job->priority = arg2;
        job->required_cpu = arg3;
        job->required_ram = arg4;
        job->duration = arg5;
        job->status = 0; // Pending
        job->arrival_time = current_time;
        
        if (!pq_insert(pq, job)) {
            printf("Error: Failed to add job to priority queue\n");
            free(job);
            return 1;
        }
        
        printf("Added job %d: Priority=%d, CPU=%d, RAM=%d, Duration=%d\n",
               job->job_id, job->priority, job->required_cpu,
               job->required_ram, job->duration);
        
    } else if (strcmp(command, "run-tick") == 0) {
        current_time++;
        run_scheduler_tick(nodes, pq, running_jobs, completed_jobs, current_time);
        printf("Simulation advanced to time %d\n", current_time);
        
    } else if (strcmp(command, "status") == 0) {
        print_status(nodes, pq, running_jobs, completed_jobs);
        
    } else if (strcmp(command, "save") == 0) {
        if (sscanf(line, "save %255s", arg1) != 1) {
            printf("Error: Usage: save <filename>\n");
            return 1;
        }
        
        if (save_state(arg1, nodes, pq, running_jobs, completed_jobs, current_time, next_job_id)) {
            printf("State saved to %s\n", arg1);
        } else {
            printf("Error: Failed to save state to %s\n", arg1);
        }
        
    } else if (strcmp(command, "load") == 0) {
        if (sscanf(line, "load %255s", arg1) != 1) {
            printf("Error: Usage: load <filename>\n");
            return 1;
        }
        
        // Clean up current state
        cleanup(nodes, pq, running_jobs, completed_jobs);
        
        // Load new state
        NodeList* new_nodes;
        PriorityQueue* new_pq;
        HashTable* new_running_jobs;
        JobList* new_completed_jobs;
        
        if (load_state(arg1, &new_nodes, &new_pq, &new_running_jobs, &new_completed_jobs, &current_time, &next_job_id)) {
            // Update pointers
            *nodes_ptr = new_nodes;
            *pq_ptr = new_pq;
            *running_jobs_ptr = new_running_jobs;
            *completed_jobs_ptr = new_completed_jobs;
            printf("State loaded from %s\n", arg1);
        } else {
            printf("Error: Failed to load state from %s\n", arg1);
            // Reinitialize empty structures on failure
            *nodes_ptr = nl_create(10);
            *pq_ptr = pq_create(10);
            *running_jobs_ptr = ht_create(16);
            *completed_jobs_ptr = jl_create();
        }
        
    } else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
        return 0; // Exit loop
        
    } else {
        printf("Unknown command: %s\n", command);
        printf("Available commands: add-node, add-job, run-tick, status, save, load, exit\n");
    }
    
    return 1; // Continue loop
}

int main(void) {
    printf("=== Cloud Job Scheduler Simulator ===\n");
    printf("Type 'help' for available commands, or 'exit' to quit.\n\n");
    
    // Initialize data structures
    NodeList* nodes = nl_create(10);
    PriorityQueue* pq = pq_create(10);
    HashTable* running_jobs = ht_create(16);
    JobList* completed_jobs = jl_create();
    
    if (!nodes || !pq || !running_jobs || !completed_jobs) {
        printf("Error: Failed to initialize data structures\n");
        return 1;
    }
    
    // Main command loop
    char line[MAX_LINE_LENGTH];
    int running = 1;
    
    while (running) {
        printf("> ");
        fflush(stdout);
        
        if (!fgets(line, sizeof(line), stdin)) {
            break; // EOF
        }
        
        if (strcmp(line, "help\n") == 0) {
            printf("\nAvailable commands:\n");
            printf("  add-node <cpu> <ram>     - Add a resource node\n");
            printf("  add-job <priority> <cpu> <ram> <duration> - Add a job\n");
            printf("  run-tick                 - Advance simulation by one time step\n");
            printf("  status                   - Show current status\n");
            printf("  save <filename>          - Save state to file\n");
            printf("  load <filename>          - Load state from file\n");
            printf("  exit                     - Exit the program\n\n");
            continue;
        }
        
        int result = execute_command(line, &nodes, &pq, &running_jobs, &completed_jobs);
        if (result == 0) {
            running = 0; // Exit
        }
    }
    
    // Cleanup
    cleanup(nodes, pq, running_jobs, completed_jobs);
    
    printf("Goodbye!\n");
    return 0;
}

