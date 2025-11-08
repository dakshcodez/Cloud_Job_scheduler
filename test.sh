#!/bin/bash

# Test script for Cloud Job Scheduler Simulator
# This script runs automated tests to verify functionality

echo "=== Cloud Job Scheduler Simulator - Automated Tests ==="
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

# Function to run a test
run_test() {
    local test_name="$1"
    local input_file="$2"
    local expected_output="$3"
    
    echo -n "Testing: $test_name... "
    
    # Run the test
    if ./scheduler < "$input_file" > /tmp/test_output.txt 2>&1; then
        # Check if expected output is in the result
        if grep -q "$expected_output" /tmp/test_output.txt; then
            echo -e "${GREEN}PASSED${NC}"
            ((TESTS_PASSED++))
            return 0
        else
            echo -e "${RED}FAILED${NC}"
            echo "  Expected: $expected_output"
            ((TESTS_FAILED++))
            return 1
        fi
    else
        echo -e "${RED}FAILED${NC} (execution error)"
        ((TESTS_FAILED++))
        return 1
    fi
}

# Check if scheduler exists
if [ ! -f ./scheduler ]; then
    echo -e "${RED}Error: scheduler executable not found. Run 'make' first.${NC}"
    exit 1
fi

echo "Running automated tests..."
echo ""

# Test 1: Add node
echo "Test 1: Adding a node"
cat > /tmp/test1.in <<EOF
add-node 100 200
status
exit
EOF
run_test "Add Node" /tmp/test1.in "Node 1: CPU"

# Test 2: Add job
echo "Test 2: Adding a job"
cat > /tmp/test2.in <<EOF
add-node 100 200
add-job 1 30 50 5
status
exit
EOF
run_test "Add Job" /tmp/test2.in "Job 1: Priority=1"

# Test 3: Run tick
echo "Test 3: Running a tick"
cat > /tmp/test3.in <<EOF
add-node 100 200
add-job 1 30 50 2
run-tick
status
exit
EOF
run_test "Run Tick" /tmp/test3.in "Running Jobs"

# Test 4: Job completion
echo "Test 4: Job completion"
cat > /tmp/test4.in <<EOF
add-node 100 200
add-job 1 30 50 1
run-tick
status
exit
EOF
run_test "Job Completion" /tmp/test4.in "Completed Jobs"

# Test 5: Priority ordering
echo "Test 5: Priority ordering"
cat > /tmp/test5.in <<EOF
add-node 200 300
add-job 5 20 30 2
add-job 1 20 30 2
status
exit
EOF
run_test "Priority Ordering" /tmp/test5.in "Priority=1"

# Test 6: Error handling - no nodes
echo "Test 6: Error handling (no nodes)"
cat > /tmp/test6.in <<EOF
add-job 1 30 50 5
exit
EOF
run_test "Error Handling" /tmp/test6.in "No nodes available"

# Test 7: Resource matching
echo "Test 7: Resource matching"
cat > /tmp/test7.in <<EOF
add-node 50 50
add-job 1 60 60 5
add-job 1 30 30 5
run-tick
status
exit
EOF
run_test "Resource Matching" /tmp/test7.in "Job 2"

# Test 8: Save state
echo "Test 8: Save state"
cat > /tmp/test8.in <<EOF
add-node 100 200
add-job 1 30 50 3
save /tmp/test_state.txt
exit
EOF
if ./scheduler < /tmp/test8.in > /tmp/test8.out 2>&1 && [ -f /tmp/test_state.txt ]; then
    echo -e "${GREEN}Test 8: Save State... PASSED${NC}"
    ((TESTS_PASSED++))
else
    echo -e "${RED}Test 8: Save State... FAILED${NC}"
    ((TESTS_FAILED++))
fi

# Test 9: Load state
echo "Test 9: Load state"
cat > /tmp/test9.in <<EOF
load /tmp/test_state.txt
status
exit
EOF
if [ -f /tmp/test_state.txt ]; then
    run_test "Load State" /tmp/test9.in "Node 1"
else
    echo -e "${YELLOW}Test 9: Load State... SKIPPED (no saved state)${NC}"
fi

# Summary
echo ""
echo "=== Test Summary ==="
echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
echo -e "${RED}Failed: $TESTS_FAILED${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi

