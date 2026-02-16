#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Wi-Fi
const char* ssid = "orange";
const char* password = "12345678";

// WebSocket
WebSocketsServer webSocket = WebSocketsServer(81);

// Motor pins
#define dir1 8
#define pwm1 9
#define dir2 10
#define pwm2 11

void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(dir1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwm2, OUTPUT);

  // Wi-Fi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // WebSocket setup
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("WebSocket server started on port 81");
}

// Function to move motors
void moveMotors(float x, float y, int power){
  if(power < 10){
    analogWrite(pwm1, 0);
    analogWrite(pwm2, 0);
    return;
  }
  // Compute raw motor speeds
  float motor1 = y + x;
  float motor2 = y - x;

  // Find max magnitude
  float maxVal = max(abs(motor1), abs(motor2));
  if(maxVal > 1.0){ // normalize if needed
    motor1 /= maxVal;
    motor2 /= maxVal;
  }

  // Scale to power (0–100)
  motor1 *= power;
  motor2 *= power;

  // Convert to PWM (0–255)
  int motor1PWM = constrain(motor1 * 2.55, -255, 255);
  int motor2PWM = constrain(motor2 * 2.55, -255, 255);

  // Motor 1 direction
  if(motor1PWM >= 0){
    digitalWrite(dir1, HIGH);
    analogWrite(pwm1, motor1PWM);
  } else {
    digitalWrite(dir1, LOW);
    analogWrite(pwm1, -motor1PWM);
  }

  // Motor 2 direction
  if(motor2PWM >= 0){
    digitalWrite(dir2, HIGH);
    analogWrite(pwm2, motor2PWM);
  } else {
    digitalWrite(dir2, LOW);
    analogWrite(pwm2, -motor2PWM);
  }
}


// WebSocket event
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT){
    Serial.printf("client %d: %s\n", num, payload);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if(!error){
      // Extract joystick data
      float x = doc["x"].as<float>();
      float y = doc["y"].as<float>();
      int power = doc["power"].as<int>();

      moveMotors(x, y, power);
    }
  }
}

void loop() {
  webSocket.loop();
}
