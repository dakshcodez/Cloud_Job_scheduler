#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>

PriorityQueue* pq_create(int capacity) {
    if (capacity <= 0) {
        capacity = 10; // Default capacity
    }
    
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        return NULL;
    }
    
    pq->jobs = (Job**)malloc(capacity * sizeof(Job*));
    if (!pq->jobs) {
        free(pq);
        return NULL;
    }
    
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

// Helper function to resize the heap
static int pq_resize(PriorityQueue* pq) {
    int new_capacity = pq->capacity * 2;
    Job** new_jobs = (Job**)realloc(pq->jobs, new_capacity * sizeof(Job*));
    if (!new_jobs) {
        return 0; // Failed to resize
    }
    
    pq->jobs = new_jobs;
    pq->capacity = new_capacity;
    return 1;
}

void heapify_up(PriorityQueue* pq, int index) {
    if (index == 0) return;
    
    int parent = (index - 1) / 2;
    
    // Min-heap: parent should have lower or equal priority value
    if (pq->jobs[parent]->priority > pq->jobs[index]->priority) {
        // Swap
        Job* temp = pq->jobs[parent];
        pq->jobs[parent] = pq->jobs[index];
        pq->jobs[index] = temp;
        
        heapify_up(pq, parent);
    }
}

void heapify_down(PriorityQueue* pq, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;
    
    // Find the smallest priority among node and its children
    if (left < pq->size && pq->jobs[left]->priority < pq->jobs[smallest]->priority) {
        smallest = left;
    }
    if (right < pq->size && pq->jobs[right]->priority < pq->jobs[smallest]->priority) {
        smallest = right;
    }
    
    // If smallest is not the current node, swap and continue
    if (smallest != index) {
        Job* temp = pq->jobs[index];
        pq->jobs[index] = pq->jobs[smallest];
        pq->jobs[smallest] = temp;
        
        heapify_down(pq, smallest);
    }
}

int pq_insert(PriorityQueue* pq, Job* job) {
    if (!pq || !job) {
        return 0; // Error
    }
    
    // Resize if necessary
    if (pq->size >= pq->capacity) {
        if (!pq_resize(pq)) {
            return 0; // Failed to resize
        }
    }
    
    // Add job at the end
    pq->jobs[pq->size] = job;
    pq->size++;
    
    // Heapify up to maintain min-heap property
    heapify_up(pq, pq->size - 1);
    
    return 1; // Success
}

Job* pq_extract_min(PriorityQueue* pq) {
    if (!pq || pq->size == 0) {
        return NULL;
    }
    
    // Root is the minimum
    Job* min_job = pq->jobs[0];
    
    // Move last element to root
    pq->jobs[0] = pq->jobs[pq->size - 1];
    pq->size--;
    
    // Heapify down to maintain min-heap property
    if (pq->size > 0) {
        heapify_down(pq, 0);
    }
    
    return min_job;
}

Job* pq_peek(PriorityQueue* pq) {
    if (!pq || pq->size == 0) {
        return NULL;
    }
    return pq->jobs[0];
}

int pq_is_empty(PriorityQueue* pq) {
    return !pq || pq->size == 0;
}

int pq_size(PriorityQueue* pq) {
    return pq ? pq->size : 0;
}

void pq_free(PriorityQueue* pq) {
    if (pq) {
        free(pq->jobs);
        free(pq);
    }
}

