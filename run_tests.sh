#!/bin/bash

TEST_DIR="tests"
INTERPRETER="./interpreter"

if [ ! -x "$INTERPRETER" ]; then
    echo "Error: interpreter not found. Run make first."
    exit 1
fi

if [ ! -d "$TEST_DIR" ]; then
    echo "Error: tests directory not found."
    exit 1
fi

TOTAL=0
PASSED=0
FAILED=0

for file in "$TEST_DIR"/*.txt; do
    [ -f "$file" ] || continue

    TOTAL=$((TOTAL + 1))

    echo ""
    echo "========================================"
    echo "TEST: $file"
    echo "========================================"

    "$INTERPRETER" "$file"
    STATUS=$?

    if [ "$STATUS" -eq 0 ]; then
        echo ""
        echo "[PASS] $file"
        PASSED=$((PASSED + 1))
    else
        echo ""
        echo "[FAIL] $file (exit code: $STATUS)"
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo "========================================"
echo "             TEST SUMMARY"
echo "========================================"
echo "Total:  $TOTAL"
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo "========================================"
