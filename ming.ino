#include <WiFi.h>
#include <WiFiClient.h>
#include <MQTTClient.h>

const char* ssid = " ";
const char* password = " ";

const char* broker = "192.168.1.10";
const int port = 1883;
const char* pubTopic = "ruqaya/sensors/env";
const char* subTopic = "ruqaya/control/fan";

WiFiClient net;
MQTTClient client(1024);

const int motorPin = 5;

void connectWiFi() {
  Serial.print("connecting to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("wifi connected");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("message arrived " + topic + ": " + payload);
  if (topic == subTopic) {
    if (payload == "on") {
      digitalWrite(motorPin, HIGH);
      Serial.println("fan on");
    } else if (payload == "off") {
      digitalWrite(motorPin, LOW);
      Serial.println("fan off");
    }
  }
}

void connectMQTT() {
  client.begin(broker, port, net);
  client.onMessage(messageReceived);
  Serial.print("connecting to mqtt broker");
  while (!client.connect("esp32-client")) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("mqtt connected");
  client.subscribe(subTopic);
}

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
  connectWiFi();
  connectMQTT();
}

void loop() {
  client.loop();
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 3000) {
    float temp = random(250, 350) / 10.0;
    float humidity = random(400, 800) / 10.0;
    String payload = "{\"temp\":" + String(temp) + ",\"humidity\":" + String(humidity) + "}";
    Serial.println("sending: " + payload);
    bool result = client.publish(pubTopic, payload);
    Serial.println(result ? "published" : "failed");
    lastSend = millis();
  }
}
