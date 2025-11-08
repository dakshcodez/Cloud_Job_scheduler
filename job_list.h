#ifndef JOB_LIST_H
#define JOB_LIST_H

#include "structs.h"

// Create a new job list
JobList* jl_create(void);

// Add a job to the list (adds to the end)
int jl_add(JobList* jl, Job* job);

// Get the size of the list
int jl_size(JobList* jl);

// Free the job list (does not free jobs themselves)
void jl_free(JobList* jl);

// Traverse the list and print jobs (for status command)
void jl_print(JobList* jl);

#endif // JOB_LIST_H

