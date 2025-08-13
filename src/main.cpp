#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define LED_PIN 2      // Blue LED pin
#define EEPROM_SIZE 96 // For storing credentials

AsyncWebServer server(80);

String inputSSID;
String inputPassword;

void turnOnLED() {
  digitalWrite(LED_PIN, HIGH);
}

void turnOffLED() {
  digitalWrite(LED_PIN, LOW);
}

void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("wifiSetup-Esp32", "bluewave@123");

  Serial.println("Access Point Mode Started");
  Serial.println("SSID: wifiSetup-Esp32");
  Serial.println("Password: bluewave@123");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  turnOnLED();
}

void connectToWiFi(const String &ssid, const String &pass) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.print("Connecting to WiFi");
  int retries = 0;

  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    turnOnLED();
  } else {
    Serial.println("\nFailed to connect. Starting AP mode...");
    startAccessPoint();
  }
}

void saveCredentials(const String &ssid, const String &pass) {
  EEPROM.writeString(0, ssid);
  EEPROM.writeString(32, pass);
  EEPROM.commit();
}

void loadCredentials(String &ssid, String &pass) {
  ssid = EEPROM.readString(0);
  pass = EEPROM.readString(32);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  pinMode(LED_PIN, OUTPUT);
  turnOffLED();

  // Load saved Wi-Fi credentials
  loadCredentials(inputSSID, inputPassword);

  if (inputSSID.length() > 0) {
    Serial.println("Found saved Wi-Fi credentials, trying to connect...");
    connectToWiFi(inputSSID, inputPassword);
    if (WiFi.status() == WL_CONNECTED) {
      return; // Connected successfully
    }
  }

  // If not connected, start AP mode
  startAccessPoint();

  // Serve HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    const char *html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>BlueWave WiFi Setup</title>
        <style>
          body { font-family: Arial; text-align: center; background: linear-gradient(to right, #4facfe, #00f2fe); color: white; }
          .container { margin-top: 50px; background: rgba(0,0,0,0.3); padding: 20px; border-radius: 15px; width: 80%; max-width: 400px; margin-left: auto; margin-right: auto; }
          input { padding: 10px; margin: 10px; width: 80%; border-radius: 5px; border: none; }
          button { padding: 10px 20px; border: none; border-radius: 5px; background: #ff7e5f; color: white; font-size: 16px; cursor: pointer; }
          button:hover { background: #feb47b; }
        </style>
      </head>
      <body>
        <div class="container">
          <h2>BlueWave WiFi Setup</h2>
          <form id='wifiForm'>
            <input name='ssid' id='ssid' placeholder='WiFi SSID' required><br>
            <input name='password' id='password' type='password' placeholder='WiFi Password' required><br>
            <button type='submit'>Connect</button>
          </form>
        </div>
        <script>
          const form = document.getElementById('wifiForm');
          form.addEventListener('submit', async e => {
            e.preventDefault();
            const ssid = document.getElementById('ssid').value;
            const password = document.getElementById('password').value;

            // Sending via Query Params
            try {
              const res = await fetch(`/configure?ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}`);
              alert(await res.text());
            } catch (err) {
              alert('Error sending data!');
            }

            // ---------------- OLD JSON Method (commented) ----------------
            /*
            try {
              const res = await fetch('/configure', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ ssid, password })
              });
              alert(await res.text());
            } catch (err) {
              alert('Error sending data!');
            }
            */
          });
        </script>
      </body>
      </html>
    )rawliteral";

    req->send(200, "text/html", html);
  });

  // ----------- New: Handle Query Params -----------
  server.on("/configure", HTTP_GET, [](AsyncWebServerRequest *req) {
    if (req->hasParam("ssid") && req->hasParam("password")) {
      inputSSID = req->getParam("ssid")->value();
      inputPassword = req->getParam("password")->value();

      Serial.println("Received SSID (Query): " + inputSSID);
      Serial.println("Received Password (Query): " + inputPassword);

      saveCredentials(inputSSID, inputPassword);
      req->send(200, "text/plain", "WiFi credentials saved! Restarting...");

      delay(1000);
      ESP.restart();
    } else {
      req->send(400, "text/plain", "Missing SSID or password");
    }
  });

  // ----------- OLD JSON Method (Commented) -----------
  /*
  server.on("/configure", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
  {
    JsonDocument json;
    DeserializationError error = deserializeJson(json, data, len);

    if (error) {
      req->send(400, "text/plain", "Invalid JSON");
      return;
    }

    inputSSID = json["ssid"].as<String>();
    inputPassword = json["password"].as<String>();

    Serial.println("Received SSID (JSON): " + inputSSID);
    Serial.println("Received Password (JSON): " + inputPassword);

    saveCredentials(inputSSID, inputPassword);
    req->send(200, "text/plain", "WiFi credentials saved! Restarting...");

    delay(1000);
    ESP.restart();
  });
  */

  server.begin();
  Serial.println("Server started.");
}

void loop() {
  // Nothing needed here
}
