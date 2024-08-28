#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>

#define LIGHT_SENSOR_ADDR 0x23 

char ssid[] = "Sam's Iphone";
char pass[] = "helliwifi";

WiFiClient client;
BH1750 lightMeter;

char HOST_NAME[] = "maker.ifttt.com";

String lstate = "Off";
float lvalue = 0;
String llight = "Off";

void setup() {
  Serial.begin(115200);
  connectToWiFi(ssid, pass);
  
  // Initialize I2C communication and BH1750 sensor
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 sensor initialized.");
  } else {
    Serial.println("Error initializing BH1750 sensor.");
    while (1);
  }

  while (!Serial);
}

void loop() {
  float ligt_cur = readLightSensor();

  if (ligt_cur > 250.0) { 
    lstate = "On";
  } else {
    lstate = "Off";
  }

  if (lstate != llight) {
    sendWebhook(lstate);
    llight = lstate;
  }

  delay(1000);
}

void sendWebhook(String state) {
  if (!client.connect(HOST_NAME, 80)) {
    Serial.println("Connection failed!");
    return;
  }

  Serial.println("Connected to server");
  client.println("GET /trigger/sunlight_detected/with/key/fstsI_QP2lSGOIxivS5Qbnqq--CSpK6oD3nQylvCiD8 HTTP/1.1");
  client.println("Host: maker.ifttt.com");
  client.println("Connection: close");
  client.println(); 

  client.stop();
  Serial.println("Sent notification to IFTTT");
}

float readLightSensor() {
  lvalue = lightMeter.readLightLevel();
  Serial.print("Light level: ");
  Serial.print(lvalue);
  Serial.println(" lx");
  return lvalue;
}

bool connectToWiFi(char ssid[], char pass[]) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
      if (i > 6) {
        Serial.println("Wifi Connection Failed!");
        return false;
        break;
      }
      i++;
    }
    Serial.println("\nConnected.");
    return true;
  }
  return true; 
}