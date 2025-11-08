# Testing Guide for Cloud Job Scheduler Simulator

## Quick Start Testing

### 1. Build the Project
```bash
make clean
make
```

### 2. Quick Automated Test

Run the quick test script to verify basic functionality:

```bash
./quick_test.sh
```

This will run a series of commands demonstrating:
- Adding nodes
- Adding jobs with different priorities
- Running simulation ticks
- Job completion and resource release

### 3. Interactive Testing

Run the simulator interactively:

```bash
./scheduler
```

Then try these commands in sequence:

```
> add-node 100 200
> add-node 50 100
> add-job 1 30 50 5
> add-job 2 20 30 3
> status
> run-tick
> status
> run-tick
> status
```

## Comprehensive Test Scenarios

### Test 1: Basic Scheduling
**Goal**: Verify jobs are scheduled based on priority

```
> add-node 100 200
> add-job 5 20 30 3    # Lower priority (5)
> add-job 1 30 50 2    # Higher priority (1)
> status               # Should show job 2 (priority 1) as next
> run-tick             # Job 2 should start running
> status               # Verify job 2 is running, job 1 still pending
```

### Test 2: Resource Matching
**Goal**: Verify jobs only run on nodes with sufficient resources

```
> add-node 50 50
> add-job 1 60 60 5    # Requires more than node has
> add-job 1 30 30 5    # Fits on node
> run-tick
> status               # Only job 2 should be running
```

### Test 3: Multiple Jobs Scheduling (Backfilling)
**Goal**: Verify multiple jobs can be scheduled in one tick

```
> add-node 100 200
> add-node 100 200
> add-job 1 30 50 5
> add-job 2 40 60 3
> add-job 3 20 30 4
> run-tick
> status               # Should show multiple jobs running
```

### Test 4: Job Completion
**Goal**: Verify jobs complete and resources are released

```
> add-node 100 200
> add-job 1 30 50 2    # Duration = 2
> run-tick             # Job starts, duration = 1
> status               # Job should be running
> run-tick             # Job completes, duration = 0
> status               # Job should be in completed list, resources freed
```

### Test 5: Persistence (Save/Load)
**Goal**: Verify state can be saved and loaded

```
> add-node 100 200
> add-job 1 30 50 3
> run-tick
> save test.state
> status               # Note the state
> load test.state
> status               # Should match previous state
```

### Test 6: Error Handling
**Goal**: Verify proper error messages

```
> add-job 1 30 50 5    # Should fail - no nodes
> add-node 50 50
> add-job 1 100 100 5  # Should fail - job too large
> add-node -10 20      # Should fail - invalid input
```

### Test 7: Priority Queue Ordering
**Goal**: Verify jobs are processed in priority order

```
> add-node 200 300
> add-job 5 20 30 2
> add-job 1 20 30 2
> add-job 3 20 30 2
> status               # Next job should be priority 1
> run-tick             # Priority 1 job runs
> status               # Next should be priority 3
> run-tick             # Priority 3 job runs
> status               # Next should be priority 5
```

## Automated Testing Script

Use the provided `test.sh` script for automated testing:

```bash
chmod +x test.sh
./test.sh
```

## Manual Testing Checklist

- [ ] Add nodes with various CPU/RAM configurations
- [ ] Add jobs with different priorities
- [ ] Verify priority queue ordering (lower number = higher priority)
- [ ] Test resource matching (jobs only run if resources available)
- [ ] Test backfilling (multiple jobs scheduled per tick)
- [ ] Test job completion and resource release
- [ ] Test save/load functionality
- [ ] Test error handling (invalid inputs, insufficient resources)
- [ ] Test edge cases (empty queues, no nodes, etc.)
- [ ] Test with many jobs and nodes

## Expected Behaviors

1. **Priority**: Lower priority number = higher priority (job with priority 1 runs before priority 5)
2. **Resource Matching**: Job only runs if a node has `available_cpu >= required_cpu` AND `available_ram >= required_ram`
3. **Backfilling**: Multiple jobs can be scheduled in a single tick if resources allow
4. **Job Completion**: When duration reaches 0, job moves to completed list and resources are freed
5. **State Persistence**: Save/load should preserve all jobs, nodes, and their states

## Troubleshooting

- **Jobs not scheduling**: Check if nodes have sufficient resources
- **Wrong priority order**: Verify priority queue implementation (min-heap)
- **Resources not freed**: Check job completion logic in scheduler
- **Save/load fails**: Verify file permissions and format

