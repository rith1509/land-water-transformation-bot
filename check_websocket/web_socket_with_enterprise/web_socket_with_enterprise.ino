// #include <ESP8266WiFi.h>
#include <WiFi.h>
#include "esp_wpa2.h" 
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// --- Wi-Fi credentials ---
#define WIFI_SSID     "IITR_WIFI_HIGHSPEED"   // Replace with your network SSID
#define EAP_IDENTITY  "esp32"          // Phase 1 identity (optional)
#define EAP_USERNAME  "24112054"          // Phase 2 username
#define EAP_PASSWORD  "@#Whatisurname1?#@"    // Phase 2 password

// --- WebSocket server on port 81 ---
WebSocketsServer webSocket = WebSocketsServer(81);

// --- WebSocket event handler ---
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT){
    Serial.printf("client %d: %s\n", num, payload);

    // Optional: parse JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting ESP32 WPA2-Enterprise connection...");

  // Set Wi-Fi to station mode
  WiFi.mode(WIFI_STA);

  // Configure WPA2 Enterprise credentials
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));

  // Enable WPA2 Enterprise
  esp_wifi_sta_wpa2_ent_enable();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // --- WebSocket setup ---
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("WebSocket server started on port 81");
}

void loop() {
  webSocket.loop();} // keep WebSocket alive
