# 1. Setze IDF_PATH korrekt auf den Ordner, der direkt tools/ enthält:
export IDF_PATH="$HOME/esp/esp-idf/esp-idf"
export IDF_PATH_FORCE=1

# 2. Wechsle in genau diesen Ordner und source die Umgebung
cd "$IDF_PATH"
source ./export.sh

# 3. Geh zurück in dein Projekt und entferne das alte Build-Verzeichnis
cd /Users/christianwilhelm/C99NC/ESP32/4F5LCD
rm -rf build

# 4. Nun sollte idf.py ohne Fehler funktionieren:
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash

