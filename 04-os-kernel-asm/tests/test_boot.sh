#!/bin/bash

# Navigate to the project root if not already there
cd "$(dirname "$0")/.."

# Build the project
make clean
make

if [ ! -f os.img ]; then
    echo "BOOT TEST: FAILED (os.img not found)"
    exit 1
fi

echo "Running QEMU for 5 seconds..."

# Run QEMU with timeout
# timeout returns 124 if the command times out
timeout 5 qemu-system-i386 -drive format=raw,file=os.img \
      -display none -serial stdio \
      -no-reboot -no-shutdown \
      -device isa-debug-exit

EXIT_CODE=$?

if [ $EXIT_CODE -eq 124 ]; then
    echo "BOOT TEST: PASSED"
    exit 0
else
    echo "BOOT TEST: FAILED (QEMU exited with code $EXIT_CODE)"
    exit 1
fi
