#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// --- DHT Settings ---
#define DHTPIN D4        // DHT22 data pin connected to D4 (GPIO2)
#define DHTTYPE DHT22    // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// --- LCD Settings ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD at I2C address 0x27

// --- Other Sensors ---
#define IR_PIN   D5   // IR Fire Sensor (digital)
#define PIR_PIN  D6   // PIR Motion Sensor (digital)
#define BUZZER   D7   // Buzzer pin

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize I2C with SDA = D2, SCL = D1
  Wire.begin(D2, D1);

  // LCD Init
  lcd.init();
  lcd.backlight();

  // Sensor Pins
  pinMode(IR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);  // buzzer off initially

  lcd.setCursor(0, 0);
  lcd.print("Safety System");
  delay(2000);
  lcd.clear();
}

void loop() {
  // --- Read Sensors ---
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius
  int irState   = digitalRead(IR_PIN);
  int pirState  = digitalRead(PIR_PIN);

  // --- DHT22 Error Check ---
  if (isnan(h) || isnan(t)) {
    Serial.println("DHT22 read error!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT22 Error!");
    delay(2000);
    return;
  }

  // --- Serial Output ---
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" *C, Hum: ");
  Serial.print(h);
  Serial.print(" %, Fire: ");
  Serial.print(irState == HIGH ? "DETECTED" : "Safe");
  Serial.print(", Motion: ");
  Serial.println(pirState == HIGH ? "MOTION" : "None");

  // --- LCD Output ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);
  lcd.print((char)223); // degree symbol
  lcd.print("C H:");
  lcd.print(h);
  lcd.print("%");

  // --- Line 2 combine alerts ---
  lcd.setCursor(0, 1);
  String status = "";

  if (irState == HIGH) {
    if (status != "") status += ", ";
    status += "FIRE";
  }

  if (pirState == HIGH) {
    if (status != "") status += ", ";
    status += "MOTION";
  }

  if (status == "") {
    status = "All Safe :)";
  }

  lcd.print(status);

  // --- Buzzer Logic (only when both fire + motion detected) ---
  if (irState == HIGH || pirState == HIGH) {
    digitalWrite(BUZZER, HIGH);  // buzzer on
  } else {
    digitalWrite(BUZZER, LOW);   // buzzer off
  }

  delay(2000); // Update every 2 sec
}