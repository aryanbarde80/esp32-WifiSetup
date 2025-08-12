# ESP32 WiFi Configuration Portal

Easily connect your ESP32 to WiFi without hardcoding credentials.  
Starts in **AP mode**, hosts a web page for SSID & password input, then switches to **STA mode** to connect.

## Features
- 📡 AP mode for WiFi setup via browser  
- ⚡ Asynchronous & non-blocking server  
- 🔄 Auto-restart if connection fails  
- 🌐 CORS support for web apps  

## How to Use
1. Power on ESP32 — it creates the network **ESP_SETUP** (password: `12345678`)  
2. Connect and open browser at `http://192.168.4.1`  
3. Enter WiFi details and submit  
4. ESP32 connects to your WiFi and shows IP in Serial Monitor  

## License
MIT License
