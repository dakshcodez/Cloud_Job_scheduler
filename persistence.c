#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int save_state(const char* filename, NodeList* nodes, PriorityQueue* pq, HashTable* running_jobs, JobList* completed_jobs, int current_time, int next_job_id) {
    if (!filename || !nodes || !pq || !running_jobs || !completed_jobs) {
        return 0; // Error
    }
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        return 0; // Failed to open file
    }
    
    // Write header
    fprintf(file, "# Cloud Job Scheduler State File\n");
    fprintf(file, "TIME %d\n", current_time);
    fprintf(file, "NEXT_JOB_ID %d\n", next_job_id);
    
    // Write nodes
    fprintf(file, "NODES %d\n", nodes->size);
    for (int i = 0; i < nodes->size; i++) {
        ResourceNode* node = nl_get(nodes, i);
        fprintf(file, "NODE %d %d %d %d %d\n",
                node->node_id, node->total_cpu, node->total_ram,
                node->available_cpu, node->available_ram);
    }
    
    // Write pending jobs (from priority queue)
    // We need to extract all jobs, save them, then re-insert them
    PriorityQueue* temp_pq = pq_create(pq->capacity);
    if (!temp_pq) {
        fclose(file);
        return 0;
    }
    
    int pending_count = pq_size(pq);
    Job** pending_jobs = (Job**)malloc(pending_count * sizeof(Job*));
    if (!pending_jobs) {
        pq_free(temp_pq);
        fclose(file);
        return 0;
    }
    
    // Extract all jobs
    for (int i = 0; i < pending_count; i++) {
        pending_jobs[i] = pq_extract_min(pq);
    }
    
    // Write pending jobs
    fprintf(file, "PENDING_JOBS %d\n", pending_count);
    for (int i = 0; i < pending_count; i++) {
        Job* job = pending_jobs[i];
        fprintf(file, "JOB %d %d %d %d %d %d %d\n",
                job->job_id, job->priority, job->required_cpu,
                job->required_ram, job->duration, job->status, job->arrival_time);
    }
    
    // Re-insert jobs back into priority queue
    for (int i = 0; i < pending_count; i++) {
        pq_insert(pq, pending_jobs[i]);
    }
    free(pending_jobs);
    pq_free(temp_pq);
    
    // Write running jobs
    int running_count = ht_size(running_jobs);
    fprintf(file, "RUNNING_JOBS %d\n", running_count);
    
    // Traverse hash table to save running jobs
    for (int i = 0; i < running_jobs->size; i++) {
        HashNode* current = running_jobs->table[i];
        while (current) {
            fprintf(file, "RUNNING_JOB %d %d\n", current->job->job_id, current->node_id);
            fprintf(file, "JOB %d %d %d %d %d %d %d\n",
                    current->job->job_id, current->job->priority,
                    current->job->required_cpu, current->job->required_ram,
                    current->job->duration, current->job->status,
                    current->job->arrival_time);
            current = current->next;
        }
    }
    
    // Write completed jobs
    fprintf(file, "COMPLETED_JOBS %d\n", jl_size(completed_jobs));
    JobNode* current = completed_jobs->head;
    while (current) {
        fprintf(file, "JOB %d %d %d %d %d %d %d\n",
                current->job->job_id, current->job->priority,
                current->job->required_cpu, current->job->required_ram,
                current->job->duration, current->job->status,
                current->job->arrival_time);
        current = current->next;
    }
    
    fclose(file);
    return 1; // Success
}

int load_state(const char* filename, NodeList** nodes, PriorityQueue** pq, HashTable** running_jobs, JobList** completed_jobs, int* current_time, int* next_job_id) {
    if (!filename) {
        return 0; // Error
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0; // Failed to open file
    }
    
    char line[1024];
    int line_num = 0;
    
    // Initialize structures
    *nodes = nl_create(10);
    *pq = pq_create(10);
    *running_jobs = ht_create(16);
    *completed_jobs = jl_create();
    
    if (!*nodes || !*pq || !*running_jobs || !*completed_jobs) {
        fclose(file);
        return 0;
    }
    
    int pending_count = 0;
    int running_count = 0;
    int completed_count = 0;
    int nodes_count = 0;
    int last_running_job_id = -1;
    int last_running_node_id = -1;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        // Parse line
        if (strncmp(line, "TIME ", 5) == 0) {
            sscanf(line, "TIME %d", current_time);
        } else if (strncmp(line, "NEXT_JOB_ID ", 12) == 0) {
            sscanf(line, "NEXT_JOB_ID %d", next_job_id);
        } else if (strncmp(line, "NODES ", 6) == 0) {
            sscanf(line, "NODES %d", &nodes_count);
        } else if (strncmp(line, "NODE ", 5) == 0) {
            int node_id, total_cpu, total_ram, available_cpu, available_ram;
            if (sscanf(line, "NODE %d %d %d %d %d",
                      &node_id, &total_cpu, &total_ram, &available_cpu, &available_ram) == 5) {
                ResourceNode* node = (ResourceNode*)malloc(sizeof(ResourceNode));
                if (node) {
                    node->node_id = node_id;
                    node->total_cpu = total_cpu;
                    node->total_ram = total_ram;
                    node->available_cpu = available_cpu;
                    node->available_ram = available_ram;
                    nl_add(*nodes, node);
                }
            }
        } else if (strncmp(line, "PENDING_JOBS ", 13) == 0) {
            sscanf(line, "PENDING_JOBS %d", &pending_count);
        } else if (strncmp(line, "RUNNING_JOBS ", 13) == 0) {
            sscanf(line, "RUNNING_JOBS %d", &running_count);
        } else if (strncmp(line, "COMPLETED_JOBS ", 15) == 0) {
            sscanf(line, "COMPLETED_JOBS %d", &completed_count);
        } else if (strncmp(line, "RUNNING_JOB ", 12) == 0) {
            // Store the job_id and node_id for the next JOB line
            sscanf(line, "RUNNING_JOB %d %d", &last_running_job_id, &last_running_node_id);
        } else if (strncmp(line, "JOB ", 4) == 0) {
            int job_id, priority, required_cpu, required_ram, duration, status, arrival_time;
            if (sscanf(line, "JOB %d %d %d %d %d %d %d",
                      &job_id, &priority, &required_cpu, &required_ram,
                      &duration, &status, &arrival_time) == 7) {
                Job* job = (Job*)malloc(sizeof(Job));
                if (job) {
                    job->job_id = job_id;
                    job->priority = priority;
                    job->required_cpu = required_cpu;
                    job->required_ram = required_ram;
                    job->duration = duration;
                    job->status = status;
                    job->arrival_time = arrival_time;
                    
                    // Check if this is a running job (preceded by RUNNING_JOB line)
                    if (job_id == last_running_job_id) {
                        job->status = 1; // Mark as running
                        ht_insert(*running_jobs, job, last_running_node_id);
                        last_running_job_id = -1; // Reset
                    } else if (status == 0) { // Pending
                        pq_insert(*pq, job);
                    } else if (status == 2) { // Completed
                        jl_add(*completed_jobs, job);
                    }
                }
            }
        }
    }
    
    fclose(file);
    return 1; // Success
}

