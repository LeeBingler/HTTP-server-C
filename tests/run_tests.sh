#!/bin/bash

echo "[TEST] HTTP-server-C"
echo "===================================="
echo ""

FAILED=0

make re

for test_script in tests/integration/test_*.sh; do
    if [ -x "$test_script" ]; then
        $test_script
        if [ $? -ne 0 ]; then
            FAILED=1
        fi
        echo ""
    else
        echo "⚠️  $test_script not executable"
    fi
done

if [ "$FAILED" -eq 0 ]; then
    echo "✅ All tests succeed !"
    exit 0
else
    echo "❌ Some tests have failed."
    exit 1
fi