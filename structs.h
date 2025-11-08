#ifndef STRUCTS_H
#define STRUCTS_H

// A single job
typedef struct {
    int job_id;
    int priority;       // Lower number = higher priority
    int required_cpu;
    int required_ram;
    int duration;       // Time ticks remaining
    int status;         // 0=Pending, 1=Running, 2=Completed
    int arrival_time;   // Time when job was added
} Job;

// A single server node
typedef struct {
    int node_id;
    int total_cpu;
    int total_ram;
    int available_cpu;
    int available_ram;
} ResourceNode;

// --- NodeList (Dynamic Array for ResourceNodes) ---
typedef struct {
    ResourceNode** nodes;
    int size;
    int capacity;
} NodeList;

// --- PriorityQueue (Min-Heap for Pending Jobs) ---
typedef struct {
    Job** jobs;
    int size;
    int capacity;
} PriorityQueue;

// --- HashTable (for Running Jobs, using Separate Chaining) ---
typedef struct HashNode {
    int job_id;
    Job* job;
    int node_id; // Track which node this job is on
    struct HashNode* next;
} HashNode;

typedef struct {
    int size;
    HashNode** table;
} HashTable;

// --- JobList (Linked List for Completed Jobs) ---
typedef struct JobNode {
    Job* job;
    struct JobNode* next;
} JobNode;

typedef struct {
    JobNode* head;
    int size;
} JobList;

#endif // STRUCTS_H

