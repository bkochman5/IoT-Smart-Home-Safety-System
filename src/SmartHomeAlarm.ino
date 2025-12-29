#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURATION ---
const char* ssid = "***"; 
const char* password = "***";
const char* mqtt_server = "***"; 

const int trigPin = 12;
const int echoPin = 13;
// --- END ---

WiFiClient espClient;
PubSubClient client(espClient);

long duration;
int distance;
bool motionDetected = false;

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Ultrasonic_Alarm")) {
      Serial.println("connected");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Trigger alarm if something is closer than 50cm
  if (distance > 0 && distance < 50) {
    if (!motionDetected) {
      Serial.println("--- OBJECT DETECTED! ---");
      client.publish("home/alarm", "MOTION");
      motionDetected = true;
    }
  } else {
    if (motionDetected) {
      Serial.println("--- Area Clear ---");
      client.publish("home/alarm", "CLEAR");
      motionDetected = false;
    }
  }
  delay(200); 
}
