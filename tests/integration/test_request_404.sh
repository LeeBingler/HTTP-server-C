#!/bin/bash

echo "[TEST] 404 Not Found"

./http-server-c -p 8080 -r ./www/ &
SERVER_PID=$!
sleep 1

HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/notfound.html -H "Connection: close")

kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

if [ "$HTTP_CODE" = "404" ]; then
    echo "✅ 404 Not Found fonctionne"
    exit 0
else
    echo "❌ Échec: attendu 404, reçu $HTTP_CODE"
    exit 1
fi