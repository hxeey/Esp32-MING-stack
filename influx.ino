#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>

const char* ssid = "Zayed333";
const char* password = "TrIpLeRrR";

const char* host = "us-east-1-1.aws.cloud2.influxdata.com";
const int httpsPort = 443;
const char* org = "g";
const char* bucket = "esp32_ruqaya";
const char* token = "usyCho6tWo_CRSbPkR1Foy0Zm9Fqz-udQQpKRREC7rIGDvBf9izm6oyx5MhYeud2nNx42PArsS2FtAji72n9wA==";

WiFiClientSecure client;

unsigned long lastSend = 0;
const unsigned long interval = 1000;

const char* groupTag = "group1";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setInsecure();

  IPAddress influxIP;
  if (!WiFi.hostByName(host, influxIP)) {
    return;
  }

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 100000) {
    delay(500);
    now = time(nullptr);
  }
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSend >= interval) {
    lastSend = currentTime;

    float temp = random(250, 350) / 10.0;
    float humidity = random(400, 700) / 10.0;

    String data = "sensor_data,device=esp32,group=" + String(groupTag) + " temperature=" + String(temp) + ",humidity=" + String(humidity);
    String url = "/api/v2/write?org=" + String(org) + "&bucket=" + String(bucket) + "&precision=s";

    if (client.connect(host, httpsPort)) {
      client.println("POST " + url + " HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Authorization: Token " + String(token));
      client.println("Content-Type: text/plain");
      client.println("Content-Length: " + String(data.length()));
      client.println("Connection: close");
      client.println();
      client.println(data);

      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
      }

      client.stop();
    }
  }
}
