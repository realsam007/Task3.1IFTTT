#include <WiFiNINA.h>     // Include WiFi library for Nano 33 IoT
#define LIGHT_SENSOR 2    // Define the pin for the light sensor

// WiFi credentials
char ssid[] = "A16";      // SSID of the WiFi network
char pass[] = "22222222"; // Password for the WiFi network

WiFiClient client;        // Create a WiFi client object to connect to IFTTT

// IFTTT webhook URL for triggering sunlight event
// Replace the key with your actual IFTTT key
char HOST_NAME[] = "maker.ifttt.com"; 

// Variables to track light sensor state
String light_state = "Off"; // Current state of light (On/Off)
int light_value = 0;        // Light sensor reading
String last_light = "Off";  // Last state of light (On/Off)

// Function to connect to WiFi
bool connectToWiFi(char ssid[], char pass[]) {
  if (WiFi.status() != WL_CONNECTED) { // Check if already connected
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED) { // Try connecting to WiFi
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000); // Wait 5 seconds before retrying
      if (i > 6) { // If it fails after 6 attempts, give up
        Serial.print("WiFi Connection Failed!");
        return false;
        break;
      }
      i++;
    }
    Serial.println("\nConnected."); // Successfully connected
    return true;
  }
  return true; // Already connected, no need to reconnect
}

// Function to read light sensor value
int readLightSensor() {
  light_value = digitalRead(LIGHT_SENSOR); // Read the state of the light sensor
  return light_value;
}

// Function to send a webhook notification to IFTTT
void sendWebhook(String state) {
  // Attempt to connect to IFTTT server
  if (!client.connect(HOST_NAME, 80)) { 
    Serial.println("Connection failed!"); // Failed to connect
    return;
  }

  // If connected, send HTTP request to IFTTT
  Serial.println("Connected to server");
  client.println("GET /trigger/sunlight_enter/with/key/dCzgH4VqCd8yZ_jYfZseMYt86OHL13Bdzn5tjtvUlSZ HTTP/1.1");
  client.println("Host: maker.ifttt.com");
  client.println("Connection: close");
  client.println(); // End of HTTP request

  client.stop(); // Close connection
  Serial.println("Sent notification to IFTTT"); // Confirm webhook sent
}

void setup() {
  Serial.begin(115200);   // Start serial communication at 115200 baud rate
  connectToWiFi(ssid, pass); // Connect to the WiFi network
  while (!Serial);        // Wait for the serial monitor to start

  pinMode(LIGHT_SENSOR, INPUT); // Set light sensor pin as input
}

void loop() {
  int current_light = readLightSensor(); // Get the current light sensor value

  // Update light state based on the sensor reading
  if (current_light > 0) {
    light_state = "On";
  } else {
    light_state = "Off";
  }

  // Send notification if light state has changed or 'c' is pressed in the Serial Monitor
  if (light_state != last_light || Serial.read() == 'c') {
    sendWebhook(light_state); // Trigger IFTTT webhook
    last_light = light_state; // Update last light state to current
  }
  
  delay(1000); // Wait 1 second before checking again (adjust for desired frequency)
}
