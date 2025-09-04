#include <WiFi.h>
#include "esp_eap_client.h"   // New WPA2-Enterprise API

// Replace with your WiFi credentials
const char* ssid     = "IITR_WIFI";   // Org WiFi SSID
const char* username = "24116013";    // WiFi username
const char* password = "Motoe4-Plus@";// WiFi password

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Starting WPA2 Enterprise connection on ESP32...");

  WiFi.disconnect(true);   // Disconnect in case already connected
  WiFi.mode(WIFI_STA);     // Station mode

  // WPA2 Enterprise (PEAP/MSCHAPv2)
  esp_eap_client_set_identity((uint8_t*)username, strlen(username));
  esp_eap_client_set_username((uint8_t*)username, strlen(username));
  esp_eap_client_set_password((uint8_t*)password, strlen(password));
  esp_eap_client_enable();

  WiFi.begin(ssid);

  Serial.println("Connecting to WiFi...");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("✅ Connected! IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("⏳ Still connecting...");
  }
  delay(5000);
}
