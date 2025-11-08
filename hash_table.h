#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "structs.h"

// Create a new hash table with specified size
HashTable* ht_create(int size);

// Hash function for job_id
unsigned int hash(int job_id, int table_size);

// Insert a job into the hash table
int ht_insert(HashTable* ht, Job* job, int node_id);

// Find a job in the hash table by job_id
HashNode* ht_find(HashTable* ht, int job_id);

// Remove a job from the hash table by job_id
Job* ht_remove(HashTable* ht, int job_id);

// Get the size of the hash table
int ht_size(HashTable* ht);

// Free the hash table (does not free jobs themselves)
void ht_free(HashTable* ht);

// Traverse all entries in the hash table (for updating running jobs)
// Callback function type: void callback(Job* job, int node_id, void* user_data)
typedef void (*ht_traverse_callback)(Job* job, int node_id, void* user_data);
void ht_traverse(HashTable* ht, ht_traverse_callback callback, void* user_data);

#endif // HASH_TABLE_H

