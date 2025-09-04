// Include necessary libraries
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

// --- Configuration ---
// WiFi Credentials for WPA2-Enterprise Network
const char* ssid = "IITR_WIFI";
const char* eap_identity = "24116013";
const char* eap_password = "Motoe4-Plus@";

// WebSocket Server Details -- THIS IS YOUR COMPUTER'S IP
const char* ws_host = "10.61.115.143";
const uint16_t ws_port = 3000;

// --- Pin Definitions (UPDATE THESE TO MATCH YOUR WIRING) ---
#define LEFT_MOTOR_PWM 23
#define LEFT_MOTOR_DIR 22
#define RIGHT_MOTOR_PWM 19
#define RIGHT_MOTOR_DIR 18

#define SERVO_ONE_PIN 13
#define SERVO_TWO_PIN 12

// --- Motor Control Settings ---
const int MAX_PWM_SPEED = 255;

// --- Global Objects ---
WebSocketsClient webSocket;
Servo servoOne;
Servo servoTwo;

// Function to handle incoming WebSocket events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WebSocket connected.");
      webSocket.sendTXT("{\"type\":\"identify\",\"id\":\"robot\"}");
      break;
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected.");
      break;
    case WStype_TEXT: {
      // Print the raw message that was just received from the server
      Serial.printf("Message from Server: %s\n", payload);

      // Now, continue with the existing logic to control the robot
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload, length);
      
      if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return;
      }

      const char* command_type = doc["type"];

      if (strcmp(command_type, "joystick") == 0) {
        float x = doc["x"];
        float y = doc["y"];
        handleJoystick(x, y);
      } else if (strcmp(command_type, "land_mode") == 0) {
        switchToLandMode();
      } else if (strcmp(command_type, "water_mode") == 0) {
        switchToWaterMode();
      }
      break;
    }
    case WStype_ERROR:
    case WStype_BIN:
    // ... other cases ...
      break;
  }
}

// Function to handle joystick data for differential steering
void handleJoystick(float x, float y) {
  float leftSpeed = (y + x) * MAX_PWM_SPEED;
  float rightSpeed = (y - x) * MAX_PWM_SPEED;

  leftSpeed = constrain(leftSpeed, -MAX_PWM_SPEED, MAX_PWM_SPEED);
  rightSpeed = constrain(rightSpeed, -MAX_PWM_SPEED, MAX_PWM_SPEED);

  digitalWrite(LEFT_MOTOR_DIR, leftSpeed > 0 ? HIGH : LOW);
  digitalWrite(RIGHT_MOTOR_DIR, rightSpeed > 0 ? HIGH : LOW);

  analogWrite(LEFT_MOTOR_PWM, abs(leftSpeed));
  analogWrite(RIGHT_MOTOR_PWM, abs(rightSpeed));
}

void switchToLandMode() {
  servoOne.write(0);
  servoTwo.write(180);
  Serial.println("Action: Switched to Land Mode.");
}

void switchToWaterMode() {
  servoOne.write(180);
  servoTwo.write(0);
  Serial.println("Action: Switched to Water Mode.");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  servoOne.attach(SERVO_ONE_PIN);
  servoTwo.attach(SERVO_TWO_PIN);
  
  // Connect to Wi-Fi (WPA2-Enterprise)
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, WPA2_AUTH_PEAP, eap_identity, eap_identity, eap_password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Connect to WebSocket server
  webSocket.begin(ws_host, ws_port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop(); // Keep the WebSocket connection alive
}