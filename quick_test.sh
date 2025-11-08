#!/bin/bash

# Quick interactive test - demonstrates basic functionality

echo "=== Quick Test Demo ==="
echo "This will run a series of commands to test the scheduler"
echo ""

# Create test input
cat > /tmp/quick_test.in <<'EOF'
add-node 100 200
add-node 50 100
add-job 1 30 50 3
add-job 2 20 30 2
add-job 3 40 60 4
status
run-tick
status
run-tick
status
run-tick
status
exit
EOF

echo "Running test commands..."
echo ""
./scheduler < /tmp/quick_test.in

echo ""
echo "=== Test Complete ==="

