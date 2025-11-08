#include "job_list.h"
#include <stdlib.h>
#include <stdio.h>

JobList* jl_create(void) {
    JobList* jl = (JobList*)malloc(sizeof(JobList));
    if (!jl) {
        return NULL;
    }
    
    jl->head = NULL;
    jl->size = 0;
    return jl;
}

int jl_add(JobList* jl, Job* job) {
    if (!jl || !job) {
        return 0; // Error
    }
    
    JobNode* new_node = (JobNode*)malloc(sizeof(JobNode));
    if (!new_node) {
        return 0; // Failed to allocate
    }
    
    new_node->job = job;
    new_node->next = NULL;
    
    // Add to the end of the list
    if (jl->head == NULL) {
        jl->head = new_node;
    } else {
        JobNode* current = jl->head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }
    
    jl->size++;
    return 1; // Success
}

int jl_size(JobList* jl) {
    return jl ? jl->size : 0;
}

void jl_print(JobList* jl) {
    if (!jl || !jl->head) {
        printf("  (none)\n");
        return;
    }
    
    JobNode* current = jl->head;
    while (current) {
        printf("  Job %d: Priority=%d, CPU=%d, RAM=%d, Duration=%d\n",
               current->job->job_id,
               current->job->priority,
               current->job->required_cpu,
               current->job->required_ram,
               current->job->duration);
        current = current->next;
    }
}

void jl_free(JobList* jl) {
    if (jl) {
        JobNode* current = jl->head;
        while (current) {
            JobNode* next = current->next;
            free(current);
            current = next;
        }
        free(jl);
    }
}

