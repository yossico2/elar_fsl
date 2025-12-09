#!/bin/bash
# Run FSL integration tests end-to-end
# Usage: ./run_integration.sh

set -e

cd "$(dirname "$0")/../build-debug"

# Start FSL in background
./fsl &
FSL_PID=$!

# Wait for FSL to initialize
sleep 1

# Run integration tests (from project root)
cd ../
python3 tests/integration_test.py
TEST_RESULT=$?

# Kill FSL
kill $FSL_PID
wait $FSL_PID 2>/dev/null || true

exit $TEST_RESULT
