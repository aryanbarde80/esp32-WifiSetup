# ESP32 WiFi Configuration Portal

Easily connect your ESP32 to WiFi without hardcoding credentials.  
Starts in **AP mode**, hosts a web page for SSID & password input, then optionally switches to **STA mode** (auto-connect currently disabled) after credentials are entered.

## Features
- 📡 AP mode for WiFi setup via browser  
- 💾 EEPROM storage for WiFi credentials with reset option  
- 💡 LED status indicators for AP mode, STA connection, and connection attempts  
- ⚡ Non-blocking LED blinking for smooth visual feedback  
- 🌐 Asynchronous web server using ESPAsyncWebServer  

## How to Use
1. Power on ESP32 — it creates the network **wifiSetup-Esp32** (password: `bluewave@123`)  
2. Connect and open browser at `http://192.168.4.1`  
3. Enter WiFi details and submit  
4. ESP32 saves credentials to EEPROM (auto-connect is currently disabled; device stays in AP mode)  
5. After modifying code, you can enable auto-connect by uncommenting the connection call  

## License
MIT License
