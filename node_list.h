#ifndef NODE_LIST_H
#define NODE_LIST_H

#include "structs.h"

// Create a new node list with initial capacity
NodeList* nl_create(int capacity);

// Add a node to the list
int nl_add(NodeList* nl, ResourceNode* node);

// Get a node by index
ResourceNode* nl_get(NodeList* nl, int index);

// Get the size of the list
int nl_size(NodeList* nl);

// Find an available node that can fit the job requirements
// Returns the index of the node, or -1 if no node is available
int nl_find_available_node(NodeList* nl, Job* job);

// Free the node list (does not free nodes themselves)
void nl_free(NodeList* nl);

#endif // NODE_LIST_H

