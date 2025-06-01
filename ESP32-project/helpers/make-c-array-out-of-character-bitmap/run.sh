#!/bin/bash

# Fallback: einfach python3 im Pfad verwenden
PYTHON_BIN=$(which python3)

if [ -z "$PYTHON_BIN" ]; then
    echo "❌ Kein python3 gefunden. Bitte installiere es mit:"
    echo "   brew install python"
    exit 1
fi

echo "✅ Verwende Python: $PYTHON_BIN"

# Pillow prüfen
$PYTHON_BIN -c "import PIL" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "📦 Pillow nicht gefunden – wird installiert..."
    $PYTHON_BIN -m pip install --user pillow --break-system-packages
fi

# Skript starten
echo "🚀 Starte bmp-to-c-array.py ..."
$PYTHON_BIN bmp-to-c-array.py

