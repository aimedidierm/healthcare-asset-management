#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Pin definitions
#define SS_PIN 10
#define RST_PIN 5
#define STOCK_IN_PIN 7   // Previously SCAN_MODE_PIN
#define STOCK_OUT_PIN 6  // Previously REMOVE_MODE_PIN
#define BUZZER_PIN 8
#define FAN 9
#define DHT_PIN 4  
#define DHT_TYPE DHT11  

// Initialize objects
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

// Mode flags
bool scanMode = false;
bool isStockIn = true;  // Default mode is stock in

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  dht.begin();  
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STOCK_IN_PIN, INPUT_PULLUP);
  pinMode(STOCK_OUT_PIN, INPUT_PULLUP);
  pinMode(FAN, OUTPUT);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
}

void loop() {
  // Check for stock in/out button press
  checkStockButtons();
  
  // Monitor temperature and humidity
  updateEnvironmentInfo();
  
  // If scan mode is off, don't proceed with RFID scanning
  if (!scanMode) return;
  
  // Check for new RFID card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
  
  // Process the scanned card
  processCard();
  
  // Stop RFID
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void checkStockButtons() {
  static bool lastInButtonState = HIGH;
  static bool lastOutButtonState = HIGH;
  
  // Check Stock In button
  bool inButtonState = digitalRead(STOCK_IN_PIN);
  if (inButtonState == LOW && lastInButtonState == HIGH) {
    scanMode = true;
    isStockIn = true;
    updateLcdStatus();
    delay(300);
  }
  lastInButtonState = inButtonState;
  
  // Check Stock Out button
  bool outButtonState = digitalRead(STOCK_OUT_PIN);
  if (outButtonState == LOW && lastOutButtonState == HIGH) {
    scanMode = true;
    isStockIn = false;
    updateLcdStatus();
    delay(300);
  }
  lastOutButtonState = outButtonState;
}

void updateLcdStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (scanMode) {
    lcd.print(isStockIn ? "Stock IN Mode" : "Stock OUT Mode");
  } else {
    lcd.print("Scan Mode OFF");
  }
  
  Serial.print("MODE:");
  Serial.println(isStockIn ? "IN" : "OUT");
}

void updateEnvironmentInfo() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  lcd.setCursor(0, 1);
  if (!isnan(temperature) && !isnan(humidity)) {
    lcd.print("T:"); lcd.print(temperature); lcd.print("C ");
    lcd.print("H:"); lcd.print(humidity); lcd.print("%");
    
    if (temperature > 28) {
      digitalWrite(FAN, HIGH);
    } else {
      digitalWrite(FAN, LOW);
    }
    
    static unsigned long lastSent = 0;
    if (millis() - lastSent > 10000) {
      Serial.print("ENV:");
      Serial.print(temperature);
      Serial.print(",");
      Serial.println(humidity);
      lastSent = millis();
    }
  } else {
    lcd.print("Sensor Error!");
  }
}

void processCard() {
  // Print UID to serial
  String uidString = "";
  
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidString += String(rfid.uid.uidByte[i], HEX);
  }

  beep(2);

  // Send data to ESP8266
  Serial.print("CARD:");
  Serial.print(uidString);
  Serial.print(",");
  Serial.println(isStockIn ? "ADD" : "REMOVE");

  String response = "";

  while (Serial.available()) {
    response += (char)Serial.read();  // Read response from ESP8266
  }

  if (response.length() > 0) {

    // Parse the JSON response from the ESP8266 (which is from the server)
    DynamicJsonDocument responseDoc(1024);
    DeserializationError error = deserializeJson(responseDoc, response);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.f_str());
      return;
    }

    // Retrieve values from the JSON response
    const char* status = responseDoc["status"];
    const char* message = responseDoc["message"];

    if (strcmp(status, "1") == 0) {
      handleResponseMessage(message);
    } else if (strcmp(status, "0") == 0) {
      handleResponseError(message);
    }
  }

  delay(2000); // Delay before sending the next request

  updateLcdStatus();
}

void handleResponseError(String message) {
  beep(6);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Error");
  lcd.setCursor(0, 1);
  lcd.println(message);
  delay(3000);
  restartCircuit();
}

void handleResponseMessage(String message) {
  beep(4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println(message);
  delay(3000);
  restartCircuit();
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < times - 1) delay(200);
  }
}

void restartCircuit() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Restarting ...");
  delay(1000);
  asm volatile ("  jmp 0");
}