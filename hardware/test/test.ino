#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define SS_PIN 10
#define RST_PIN 5
#define SCAN_MODE_PIN 7
#define REMOVE_MODE_PIN 6
#define BUZZER_PIN 8
#define FAN 9

#define DHT_PIN 4  
#define DHT_TYPE DHT11  
DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

bool scanMode = false;

struct Card {
  byte uid[4];
  char name[20];
};

Card knownCards[] = {
  {{0xAA, 0x2, 0xBF, 0x43}, "AMOXILLIN"},
  {{0x5B, 0x6C, 0xED, 0x3B}, "COARTEM"},
  {{0x23, 0xEA, 0X69, 0XF5}, "PARASTAMOL"}
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
  pinMode(SCAN_MODE_PIN, INPUT);
  pinMode(REMOVE_MODE_PIN, INPUT);
  pinMode(FAN,OUTPUT);
}

void loop() {
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(SCAN_MODE_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    scanMode = !scanMode;
    Serial.println(scanMode ? "Scan Mode ON" : "Scan Mode OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(scanMode ? "Ready To Scan" : "Scan Mode OFF");
    delay(300);
  }
  lastButtonState = buttonState;

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  lcd.setCursor(0, 1);
  if (!isnan(temperature) && !isnan(humidity)) {
    lcd.print("T:"); lcd.print(temperature); lcd.print("C ");
    lcd.print("H:"); lcd.print(humidity); lcd.print("%");
    if(temperature > 28){
      digitalWrite(FAN,HIGH);
   }
  else{
    digitalWrite(FAN,LOW);
  }
  } else {
    lcd.print("Sensor Error!");
  }

  if (!scanMode) return;

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  lcd.setCursor(0, 1); 
  lcd.print("Unknown Card");

  for (int i = 0; i < numCards; i++) {
    if (memcmp(rfid.uid.uidByte, knownCards[i].uid, 4) == 0) {
      Serial.print("Card Owner: ");
      Serial.println(knownCards[i].name);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(knownCards[i].name);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1000);
      digitalWrite(BUZZER_PIN, LOW);

      if (digitalRead(REMOVE_MODE_PIN) == HIGH) {
        removeCard(i);
      }

      break;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void removeCard(int index) {
  Serial.print("Removing Card: ");
  Serial.println(knownCards[index].name);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Removing Card:");
  lcd.setCursor(0, 1);
  lcd.print(knownCards[index].name);
  
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);

  for (int i = index; i < numCards - 1; i++) {
    knownCards[i] = knownCards[i + 1];
  }

  Serial.println("Card Removed!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Removed!");

  delay(1000);
  lcd.clear();
}