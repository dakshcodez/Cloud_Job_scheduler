# Cloud Job Scheduler Simulator

A comprehensive CPU scheduler simulator written in C that simulates a cloud computing environment. The system schedules incoming jobs (tasks) onto available nodes (servers) based on job priority and resource requirements (CPU, RAM).

## Features

### Data Structures
- **Dynamic Array** (`NodeList`): Stores the list of `ResourceNode`s
- **Singly Linked List** (`JobList`): Stores the list of completed jobs
- **Min-Heap** (`PriorityQueue`): Binary tree used as a priority queue for pending jobs
- **Hash Table** (`HashTable`): Separate chaining implementation for O(1) average-case lookups of running jobs

### Algorithms
- **Priority-Based Scheduling**: Jobs are scheduled based on priority (lower number = higher priority)
- **Search Algorithm**: Searches the dynamic array of nodes for available slots
- **Sorting**: Min-heap maintains jobs sorted by priority
- **Traversal**: Traverses hash table and node array for updates and scheduling

### Persistence
- **Save State**: Writes current state of all queues and nodes to a text file
- **Load State**: Reads and restores state from a saved file

### Error Handling
- Comprehensive error checking for `malloc` failures
- Input validation for user commands
- Logical error detection (e.g., jobs requiring more resources than available)

## Building

```bash
make
```

This will create the `scheduler` executable.

To clean build artifacts:
```bash
make clean
```

## Usage

Run the scheduler:
```bash
./scheduler
```

### Available Commands

- `add-node <cpu> <ram>` - Add a resource node with specified CPU and RAM capacity
- `add-job <priority> <cpu> <ram> <duration>` - Add a job with priority, resource requirements, and duration
- `run-tick` - Advance the simulation by one time step
- `status` - Display current status (pending, running, completed jobs, and node status)
- `save <filename>` - Save the current state to a file
- `load <filename>` - Load state from a file
- `help` - Show available commands
- `exit` - Exit the program

### Example Session

```
> add-node 100 200
Added node 1: CPU=100, RAM=200

> add-node 50 100
Added node 2: CPU=50, RAM=100

> add-job 1 30 50 5
Added job 1: Priority=1, CPU=30, RAM=50, Duration=5

> add-job 2 20 30 3
Added job 2: Priority=2, CPU=20, RAM=30, Duration=3

> status
=== Scheduler Status ===

Nodes:
  Node 1: CPU 100/100, RAM 200/200
  Node 2: CPU 50/50, RAM 100/100

Pending Jobs (Priority Queue):
  Total: 2 jobs
  Next: Job 1 (Priority=1, CPU=30, RAM=50, Duration=5)

Running Jobs:
  (none)

Completed Jobs:
  (none)

> run-tick
Simulation advanced to time 1

> status
...
```

## Project Structure

```
CPU_scheduler/
├── structs.h              # Core data structure definitions
├── priority_queue.h/c      # Min-heap implementation
├── hash_table.h/c          # Hash table with separate chaining
├── node_list.h/c           # Dynamic array for nodes
├── job_list.h/c            # Linked list for completed jobs
├── scheduler.h/c           # Core scheduling logic
├── persistence.h/c         # Save/load state functionality
├── main.c                  # CLI interface
└── Makefile                # Build configuration
```

## Time Complexity Analysis

- **Priority Queue (Min-Heap)**:
  - Insert: O(log n)
  - Extract Min: O(log n)
  - Peek: O(1)

- **Hash Table**:
  - Insert: O(1) average case
  - Find: O(1) average case
  - Remove: O(1) average case
  - Traverse: O(n)

- **Node List (Dynamic Array)**:
  - Search: O(n)
  - Add: O(1) amortized

- **Job List (Linked List)**:
  - Add: O(n) (adds to end)
  - Traverse: O(n)

## Requirements Met

✅ **(Req 3.i)** Different and suitable data structures (dynamic array, linked list)  
✅ **(Req 3.ii)** Search and sort algorithms (search nodes, sort by priority)  
✅ **(Req 3.iii)** Scheduling methods (Priority-Based Scheduling)  
✅ **(Req 3.iv)** Hash Table and Tree (Min-Heap priority queue, Hash table with separate chaining)  
✅ **(Req 3.v)** Traversal algorithms (hash table and array traversal)  
✅ **(Req 3.vi)** Persistence (save_state/load_state)  
✅ **(Req 3.vii)** Error handling (malloc checks, input validation)  
✅ **(Req 3.ix)** Efficient time complexity (appropriate data structures chosen)  
✅ **(Req 3.x)** User Interface (CLI with all required commands)

## License

This project is provided as-is for educational purposes.
