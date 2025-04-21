#!/bin/bash

echo "[TEST] POST request – should echo back body"

# Démarre le serveur en arrière-plan
./http-server-c -p 8080 -r ./www/ &
SERVER_PID=$!
sleep 1  # Laisse le serveur démarrer

# Effectue une requête POST
RESPONSE=$(curl -s -X POST -d "hello=world&test=123" http://localhost:8080/)

# Arrête le serveur
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

# Vérifie que le body est renvoyé tel quel
if [[ "$RESPONSE" == *"hello=world"* && "$RESPONSE" == *"test=123"* ]]; then
    echo "✅ POST work"
    exit 0
else
    echo "❌ POST failed"
    echo "Réponse reçue : $RESPONSE"
    exit 1
fi