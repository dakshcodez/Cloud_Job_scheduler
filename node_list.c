#include "node_list.h"
#include <stdlib.h>
#include <stdio.h>

NodeList* nl_create(int capacity) {
    if (capacity <= 0) {
        capacity = 10; // Default capacity
    }
    
    NodeList* nl = (NodeList*)malloc(sizeof(NodeList));
    if (!nl) {
        return NULL;
    }
    
    nl->nodes = (ResourceNode**)malloc(capacity * sizeof(ResourceNode*));
    if (!nl->nodes) {
        free(nl);
        return NULL;
    }
    
    nl->size = 0;
    nl->capacity = capacity;
    return nl;
}

static int nl_resize(NodeList* nl) {
    int new_capacity = nl->capacity * 2;
    ResourceNode** new_nodes = (ResourceNode**)realloc(nl->nodes, new_capacity * sizeof(ResourceNode*));
    if (!new_nodes) {
        return 0; // Failed to resize
    }
    
    nl->nodes = new_nodes;
    nl->capacity = new_capacity;
    return 1;
}

int nl_add(NodeList* nl, ResourceNode* node) {
    if (!nl || !node) {
        return 0; // Error
    }
    
    // Resize if necessary
    if (nl->size >= nl->capacity) {
        if (!nl_resize(nl)) {
            return 0; // Failed to resize
        }
    }
    
    nl->nodes[nl->size] = node;
    nl->size++;
    return 1; // Success
}

ResourceNode* nl_get(NodeList* nl, int index) {
    if (!nl || index < 0 || index >= nl->size) {
        return NULL;
    }
    return nl->nodes[index];
}

int nl_size(NodeList* nl) {
    return nl ? nl->size : 0;
}

int nl_find_available_node(NodeList* nl, Job* job) {
    if (!nl || !job) {
        return -1;
    }
    
    // Search through the dynamic array for a node that can fit the job
    for (int i = 0; i < nl->size; i++) {
        ResourceNode* node = nl->nodes[i];
        if (node->available_cpu >= job->required_cpu &&
            node->available_ram >= job->required_ram) {
            return i; // Found an available node
        }
    }
    
    return -1; // No available node found
}

void nl_free(NodeList* nl) {
    if (nl) {
        free(nl->nodes);
        free(nl);
    }
}

