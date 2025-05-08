### Build & Flash Your ESP32

1. **Navigate to your project directory**  
   cd /path/to/your/project

2. **Set Target**
   idf.py set-target esp32

3. **Build**
   idf.py build

4. **Flash**
   idf.py -p /dev/ttyUSB0 flash
   -p /dev/ttyUSB0: replace with your actual serial port (Windows might be COM3, macOS /dev/cu.SLAB_USBtoUART, etc.)