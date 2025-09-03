// Include necessary libraries
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ESP32Servo.h>

// --- Configuration ---
// WiFi Credentials for WPA2-Enterprise Network
const char* ssid = "IITR_WIFI_HIGHSPEED";
const char* eap_identity = "24116013"; // <-- YOUR USER ID GOES HERE
const char* eap_password = "Motoe4-Plus@"; // <-- YOUR PASSWORD GOES HERE

// WebSocket Server Details -- THIS IS YOUR COMPUTER'S IP
const char* ws_host = "10.81.60.157";
const uint16_t ws_port = 3000;

// Motor Pin Definitions (Update with your actual GPIO pins)
#define LEFT_MOTOR_PWM 23
#define LEFT_MOTOR_DIR 22
#define RIGHT_MOTOR_PWM 19
#define RIGHT_MOTOR_DIR 18
#define SERVO_ONE_PIN 13
#define SERVO_TWO_PIN 12

// Motor Control Settings
const int MOTOR_SPEED = 200;
const int TURN_SPEED = 150;

// --- Global Objects ---
WebSocketsClient webSocket;
Servo servoOne;
Servo servoTwo;

// ... (All the motor control functions like moveForward(), turnLeft(), etc., are the same as before) ...
// --- Function Prototypes ---
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopMotors();
void transformRobot();
void revertRobot();


void setup() {
  Serial.begin(115200);
  delay(10);

  // --- Initialize Motors ---
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  servoOne.attach(SERVO_ONE_PIN);
  servoTwo.attach(SERVO_TWO_PIN);
  revertRobot();

  // --- Connect to WiFi using WPA2-Enterprise ---
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  // Set the ESP32 to station mode
  WiFi.mode(WIFI_STA);
  
  // The key part for enterprise Wi-Fi
  // It takes SSID, Username, Password, and Identity
  WiFi.begin(ssid, WPA2_AUTH_PEAP, eap_identity, eap_identity, eap_password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // --- Connect to WebSocket Server ---
  webSocket.begin(ws_host, ws_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();
}


// --- WebSocket Event Handler ---
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected.");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected.");
      break;
    case WStype_TEXT:
      char command[length + 1];
      memcpy(command, payload, length);
      command[length] = '\0';
      
      Serial.printf("Received command: %s\n", command);

      // --- Command Handling ---
      if (strcmp(command, "forward") == 0) moveForward();
      else if (strcmp(command, "backward") == 0) moveBackward();
      else if (strcmp(command, "left") == 0) turnLeft();
      else if (strcmp(command, "right") == 0) turnRight();
      else if (strcmp(command, "stop") == 0) stopMotors();
      else if (strcmp(command, "transform") == 0) transformRobot();
      else if (strcmp(command, "revert") == 0) revertRobot();
      
      break;
  }
}

// --- Motor Control Functions ---
void moveForward() {
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTOR_PWM, MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_PWM, MOTOR_SPEED);
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTOR_PWM, MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_PWM, MOTOR_SPEED);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_DIR, LOW); 
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTOR_PWM, TURN_SPEED);
  analogWrite(RIGHT_MOTOR_PWM, TURN_SPEED);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTOR_PWM, TURN_SPEED);
  analogWrite(RIGHT_MOTOR_PWM, TURN_SPEED);
}

void stopMotors() {
  analogWrite(LEFT_MOTOR_PWM, 0);
  analogWrite(RIGHT_MOTOR_PWM, 0);
}

void transformRobot() {
  servoOne.write(180);
  servoTwo.write(0);
}

void revertRobot() {
  servoOne.write(0);
  servoTwo.write(180);
}
