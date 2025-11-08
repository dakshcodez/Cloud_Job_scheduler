#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>

HashTable* ht_create(int size) {
    if (size <= 0) {
        size = 16; // Default size
    }
    
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) {
        return NULL;
    }
    
    ht->table = (HashNode**)calloc(size, sizeof(HashNode*));
    if (!ht->table) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    return ht;
}

unsigned int hash(int job_id, int table_size) {
    // Simple hash function
    return (unsigned int)(job_id % table_size);
}

int ht_insert(HashTable* ht, Job* job, int node_id) {
    if (!ht || !job) {
        return 0; // Error
    }
    
    unsigned int index = hash(job->job_id, ht->size);
    
    // Check if job already exists
    HashNode* current = ht->table[index];
    while (current) {
        if (current->job_id == job->job_id) {
            // Update existing entry
            current->job = job;
            current->node_id = node_id;
            return 1; // Success
        }
        current = current->next;
    }
    
    // Create new hash node
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) {
        return 0; // Failed to allocate
    }
    
    new_node->job_id = job->job_id;
    new_node->job = job;
    new_node->node_id = node_id;
    new_node->next = ht->table[index];
    ht->table[index] = new_node;
    
    return 1; // Success
}

HashNode* ht_find(HashTable* ht, int job_id) {
    if (!ht) {
        return NULL;
    }
    
    unsigned int index = hash(job_id, ht->size);
    HashNode* current = ht->table[index];
    
    while (current) {
        if (current->job_id == job_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL; // Not found
}

Job* ht_remove(HashTable* ht, int job_id) {
    if (!ht) {
        return NULL;
    }
    
    unsigned int index = hash(job_id, ht->size);
    HashNode* current = ht->table[index];
    HashNode* prev = NULL;
    
    while (current) {
        if (current->job_id == job_id) {
            // Remove this node
            if (prev) {
                prev->next = current->next;
            } else {
                ht->table[index] = current->next;
            }
            
            Job* job = current->job;
            free(current);
            return job;
        }
        prev = current;
        current = current->next;
    }
    
    return NULL; // Not found
}

int ht_size(HashTable* ht) {
    if (!ht) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < ht->size; i++) {
        HashNode* current = ht->table[i];
        while (current) {
            count++;
            current = current->next;
        }
    }
    return count;
}

void ht_traverse(HashTable* ht, ht_traverse_callback callback, void* user_data) {
    if (!ht || !callback) {
        return;
    }
    
    for (int i = 0; i < ht->size; i++) {
        HashNode* current = ht->table[i];
        while (current) {
            callback(current->job, current->node_id, user_data);
            current = current->next;
        }
    }
}

void ht_free(HashTable* ht) {
    if (!ht) {
        return;
    }
    
    for (int i = 0; i < ht->size; i++) {
        HashNode* current = ht->table[i];
        while (current) {
            HashNode* next = current->next;
            free(current);
            current = next;
        }
    }
    
    free(ht->table);
    free(ht);
}

