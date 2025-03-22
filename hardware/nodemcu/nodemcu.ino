#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Balance";
const char* password = "balance1234";

// Server details
const char* serverUrl = "http://192.168.43.83:8000/api";

void setup() {
  // Start Serial communication
  Serial.begin(9600);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    processSerialData(data);
  }
}

void processSerialData(String data) {
  if (data.startsWith("CARD:")) {
    // Process card data
    // Format: CARD:uid,action
    data = data.substring(5); // Remove "CARD:" prefix
    
    int firstComma = data.indexOf(',');
    
    if (firstComma > 0) {
      String uid = data.substring(0, firstComma);
      String action = data.substring(firstComma + 1);
      
      sendCardData(uid, action);
    }
  } 
  else if (data.startsWith("ENV:")) {
    // Process environment data
    // Format: ENV:temperature,humidity
    data = data.substring(4); // Remove "ENV:" prefix
    
    int comma = data.indexOf(',');
    if (comma > 0) {
      float temperature = data.substring(0, comma).toFloat();
      float humidity = data.substring(comma + 1).toFloat();
      
      sendEnvironmentData(temperature, humidity);
    }
  }
}

void sendCardData(String uid, String action) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // Start HTTP connection
    http.begin(client, String(serverUrl) + "/status");
    http.addHeader("Content-Type", "application/json");
    

    // Create JSON document
    DynamicJsonDocument doc(1024);
    doc["uid"] = uid;
    doc["action"] = action;
    
    // Create JSON document or error
    DynamicJsonDocument doc2(1024);
    doc2["status"] = "0";
    doc2["message"] = "Data send fail";
    
    // Serialize JSON to string
    String jsonData;
    serializeJson(doc, jsonData);

    // Serialize JSON to string
    String errorJsonData;
    serializeJson(doc2, errorJsonData);
    
    // Send POST request
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.println(errorJsonData);
    }
    
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void sendEnvironmentData(float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, String(serverUrl) + "/status");
    http.addHeader("Content-Type", "application/json");
    
    DynamicJsonDocument doc(256);
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      // String response = http.getString();
      // Serial.println("Response: " + response);
      // Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    
    http.end();
  }
}