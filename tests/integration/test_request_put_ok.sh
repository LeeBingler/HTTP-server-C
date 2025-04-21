#!/bin/bash

echo "[TEST] Request PUT 201 Created"

rm -f ./www/data.txt

./http-server-c -p 8080 -r ./www/   &
SERVER_PID=$!

sleep 1

curl -s -o /dev/null -X PUT -w "%{http_code}" http://localhost:8080/data.txt -H "Connection: close" -d "HI" | grep -q 201
RESULT=$?

kill $SERVER_PID

if [ $RESULT -eq 0 ]; then
    echo "✅ [PUT 201] data.txt created"
    rm -f ./www/data.txt
    exit 0
else
    echo "❌ [PUT ERROR] data.txt failed"
    exit 1
fi