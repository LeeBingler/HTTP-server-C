#!/bin/bash

echo "[TEST] Request GET OK"

./http-server-c -p 8080 -r ./www/   &
SERVER_PID=$!

sleep 1

curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/index.html -H "Connection: close" | grep -q 200
RESULT=$?

kill $SERVER_PID

if [ $RESULT -eq 0 ]; then
    echo "✅ [GET 200] index.html"
    exit 0
else
    echo "❌ [GET 200] index.html failed"
    exit 1
fi