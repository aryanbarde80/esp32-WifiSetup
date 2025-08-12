#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);

String inputSSID;
String inputPassword;

void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SetupNetwork", "password123");

  Serial.println("Access Point is ON");
  Serial.print("Connect to WiFi: ");
  Serial.println("SetupNetwork");
  Serial.println("Password: password123");

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void connectToWiFi(const String& ssid, const String& pass) {
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
  } else {
    Serial.println("\nFailed to connect. Restarting...");
    delay(1500);
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  startAccessPoint();

  // Serve a simple HTML form
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    String html = R"rawliteral(
      <h2>WiFi Configuration</h2>
      <form id='wifiForm'>
        SSID: <input name='ssid' id='ssid'><br>
        Password: <input name='password' id='password' type='password'><br><br>
        <button type='submit'>Connect</button>
      </form>
      <script>
        const form = document.getElementById('wifiForm');
        form.addEventListener('submit', async e => {
          e.preventDefault();
          const ssid = document.getElementById('ssid').value;
          const password = document.getElementById('password').value;

          const res = await fetch('/configure', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ ssid, password })
          });

          alert(await res.text());
        });
      </script>
    )rawliteral";

    req->send(200, "text/html", html);
  });

  // Handle POST with JSON WiFi credentials
  server.on("/configure", HTTP_POST, [](AsyncWebServerRequest* req) {}, NULL,
    [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<256> json;
      auto error = deserializeJson(json, data, len);

      if (error) {
        req->send(400, "text/plain", "Bad JSON");
        return;
      }

      inputSSID = json["ssid"].as<String>();
      inputPassword = json["password"].as<String>();

      Serial.println("Received SSID: " + inputSSID);
      Serial.println("Received Password: " + inputPassword);

      req->send(200, "text/plain", "Trying to connect...");

      connectToWiFi(inputSSID, inputPassword);
    }
  );

  server.begin();
  Serial.println("Server started.");
}

void loop() {
  // no need to do anything here
}
