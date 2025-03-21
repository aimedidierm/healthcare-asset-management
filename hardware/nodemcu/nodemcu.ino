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
    // Format: CARD:uid,name,action,quantity
    data = data.substring(5); // Remove "CARD:" prefix
    
    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);
    int thirdComma = data.indexOf(',', secondComma + 1);
    
    if (firstComma > 0 && secondComma > 0 && thirdComma > 0) {
      String uid = data.substring(0, firstComma);
      String name = data.substring(firstComma + 1, secondComma);
      String action = data.substring(secondComma + 1, thirdComma);
      int quantity = data.substring(thirdComma + 1).toInt();
      
      sendCardData(uid, name, action, quantity);
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
  else if (data.startsWith("UNKNOWN:")) {
    // Process unknown card
    String uid = data.substring(8); // Remove "UNKNOWN:" prefix
    sendUnknownCardData(uid);
  }
  else if (data.startsWith("MODE:")) {
    // Just log the mode change, no need to send to server
    Serial.print("Mode changed to: ");
    Serial.println(data.substring(5));
  }
}

void sendCardData(String uid, String name, String action, int quantity) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // Start HTTP connection
    http.begin(client, String(serverUrl) + "/status");
    http.addHeader("Content-Type", "application/json");
    
    // Create JSON document
    DynamicJsonDocument doc(1024);
    doc["uid"] = uid;
    doc["name"] = name;
    doc["action"] = action;
    doc["quantity"] = quantity;
    doc["timestamp"] = millis(); // Use server timestamp in production
    
    // Serialize JSON to string
    String jsonData;
    serializeJson(doc, jsonData);
    
    // Send POST request
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      String response = http.getString();
      Serial.println("Response: " + response);
      Serial.println("Error on sending POST: " + String(httpResponseCode));
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
    doc["timestamp"] = millis(); // Use server timestamp in production
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Env data sent, code: " + String(httpResponseCode));
    } else {
      String response = http.getString();
      Serial.println("Response: " + response);
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    
    http.end();
  }
}

void sendUnknownCardData(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, String(serverUrl) + "/status");
    http.addHeader("Content-Type", "application/json");
    
    DynamicJsonDocument doc(256);
    doc["uid"] = uid;
    doc["timestamp"] = millis();
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Unknown card data sent, code: " + String(httpResponseCode));
    }
    
    http.end();
  }
}