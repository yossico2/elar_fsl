# FSL Integration Tests

This document describes the integration tests for the FSL system, simulating both the ground segment (GSL) and space segment apps using Python sockets.

## What the Tests Do

- **Simulate GSL (Ground Segment):**
  - Sends UDP packets to FSL's UDP port, mimicking uplink messages from the ground.
  - Verifies that FSL routes these messages to the correct UDS client (app) based on the opcode.

- **Simulate Apps (Space Segment):**
  - Sends payloads to FSL's UDS server sockets, mimicking downlink messages from apps.
  - Verifies that FSL routes these messages to the correct UDP destination (GSL).

- **End-to-End Verification:**
  - Ensures that message routing, header handling, and socket communication work as expected.

## Test Script

The integration tests are implemented in `tests/integration_test.py` using Python's `socket` module.

### Main Test Functions
- `test_udp_to_uds()`: Simulates GSL sending a UDP packet to FSL and verifies receipt by the correct UDS client.
- `test_uds_to_udp()`: Simulates an app sending to a UDS server socket and verifies receipt by GSL via UDP.

## How to Run Integration Tests

1. **Start the FSL application:**
   - Make sure FSL is running and listening on the configured UDP and UDS sockets.
   - Example:
     ```bash
     ./build-debug/fsl
     ```

2. **Run the integration test script:**
   - In another terminal, execute:
     ```bash
     python3 tests/integration_test.py
     ```

3. **Observe the output:**
   - The script will print test progress and results.
   - If all tests pass, you will see:
     ```
     All integration tests passed.
     ```
   - If a test fails, an assertion error will be shown.

## Notes
- Ensure the UDS and UDP socket paths/ports in the test script match those in your `config.xml`.
- You can extend the script to simulate more apps, opcodes, or edge cases as needed.

---
For more details, see the comments in `integration_test.py`.
