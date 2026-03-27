#include <Arduino.h>
#include <pico/util/queue.h>
#include <WiFi.h>


//  DATA STRUCTURE & QUEUE SETUP
// Define the shape of the data you want to pass between cores
typedef struct {
  int sensorValue;
  unsigned long timestamp;
} CoreMessage;

// Create the global queue so both cores can access it
queue_t coreQueue;


//  WI-FI CONFIGURATION (Easy to swap!)
// i dont think having this in the main file is the startest way to do this tbh though.
const char* WIFI_SSID = "YOUR_WIFI_SSID"; // AsusLAN
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD"; // ISCISCLAN6


// CORE 0: Handles Wi-Fi and Network Sockets

void setup() {
  Serial.begin(115200);
  delay(2000); // Give serial monitor a moment to connect

  // Initialize the queue: (pointer to queue, size of one item, max items)
  queue_init(&coreQueue, sizeof(CoreMessage), 10);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Ready for socket connections...");
}

void loop() {
  CoreMessage incomingMsg;
  
  // Try to grab data from the queue (non-blocking!)
  if (queue_try_remove(&coreQueue, &incomingMsg)) {
    Serial.print("[Core 0] Data received from Core 1 -> Value: ");
    Serial.print(incomingMsg.sensorValue);
    Serial.print(" | Time: ");
    Serial.println(incomingMsg.timestamp);
    
    // SOCKET PROGRAMMING GOES HERE 
    // You would take 'incomingMsg' and push it over your TCP/UDP socket.
  }
  
  // Core 0 handles other non-blocking network tasks here
  // e.g., checking for incoming socket clients
}


// CORE 1: Handles Hardware, Sensors, and Heavy Math

void setup1() {
  // Setup hardware for Core 1 (e.g., I2C sensors, SPI devices)
  // Note: setup1() runs automatically alongside setup()
}

void loop1() {
  //  Generate or read data
  CoreMessage newMsg;
  newMsg.sensorValue = random(0, 1024); // Simulating reading an ADC or Sensor
  newMsg.timestamp = millis();

  //  Push data to the queue (non-blocking!)
  if (queue_try_add(&coreQueue, &newMsg)) {
    // Data successfully sent to Core 0
  } else {
    // The queue is full (Core 0 is busy). 
    // You can handle overflow/dropped packets here.
  }

  // Simulate doing work every 2 seconds
  delay(2000); 
}