#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

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

// Card structure
struct Card {
  byte uid[4];
  char name[20];
  int quantity;  // Added quantity field
};

// Database of known cards
Card knownCards[] = {
  {{0xAA, 0x2, 0xBF, 0x43}, "AMOXILLIN", 10},
  {{0x5B, 0x6C, 0xED, 0x3B}, "COARTEM", 5},
  {{0x23, 0xEA, 0X69, 0XF5}, "PARASTAMOL", 8}
};

const int numCards = sizeof(knownCards) / sizeof(Card);

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
  
  // Log to serial for ESP8266
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
    
    // Control fan based on temperature
    if (temperature > 28) {
      digitalWrite(FAN, HIGH);
    } else {
      digitalWrite(FAN, LOW);
    }
    
    // Send environment data to ESP8266 every 10 seconds
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
    if (i < rfid.uid.size - 1) uidString += ":";
  }
  
  // Check if card is recognized
  int cardIndex = -1;
  for (int i = 0; i < numCards; i++) {
    if (memcmp(rfid.uid.uidByte, knownCards[i].uid, 4) == 0) {
      cardIndex = i;
      break;
    }
  }
  
  // Process the card
  if (cardIndex >= 0) {
    // Known card
    if (isStockIn) {
      // Stock In operation
      knownCards[cardIndex].quantity++;
      displayCardInfo(cardIndex, "IN");
      beep(1);
    } else {
      // Stock Out operation
      if (knownCards[cardIndex].quantity > 0) {
        knownCards[cardIndex].quantity--;
        displayCardInfo(cardIndex, "OUT");
        beep(2);
      } else {
        // Out of stock
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(knownCards[cardIndex].name);
        lcd.setCursor(0, 1);
        lcd.print("OUT OF STOCK!");
        beep(3);
      }
    }
    
    // Send data to ESP8266
    Serial.print("CARD:");
    Serial.print(uidString);
    Serial.print(",");
    Serial.print(knownCards[cardIndex].name);
    Serial.print(",");
    Serial.print(isStockIn ? "IN" : "OUT");
    Serial.print(",");
    Serial.println(knownCards[cardIndex].quantity);
    
  } else {
    // Unknown card
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unknown Card");
    lcd.setCursor(0, 1);
    lcd.print(uidString);
    beep(3);
    
    // Send unknown card data
    Serial.print("UNKNOWN:");
    Serial.println(uidString);
  }
  
  delay(2000);
  updateLcdStatus();
}

void displayCardInfo(int index, String action) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(knownCards[index].name);
  lcd.setCursor(0, 1);
  lcd.print(action);
  lcd.print(" | Qty: ");
  lcd.print(knownCards[index].quantity);
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < times - 1) delay(200);
  }
}