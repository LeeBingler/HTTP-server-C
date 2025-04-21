#!/bin/bash

echo "[TEST] Keep-Alive"

./http-server-c -p 8080 -r ./www/ &
SERVER_PID=$!
sleep 1

RESPONSE=$(curl -s -D - \
  -H "Connection: keep-alive" \
  --http1.1 \
  --keepalive-time 2 \
  http://localhost:8080/index.html \
  http://localhost:8080/index.html)

kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

COUNT=$(echo "$RESPONSE" | grep -c "HTTP/1.1 200 OK")

if [ "$COUNT" -eq 2 ]; then
    echo "✅ Keep-Alive fonctionne (2 requêtes réussies sur une connexion)"
    exit 0
else
    echo "❌ Échec du Keep-Alive : seulement $COUNT réponse(s) reçue(s)"
    exit 1
fi