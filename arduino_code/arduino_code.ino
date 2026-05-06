#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include <DHT.h>

// ===== WIFI =====
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"

// ===== FIREBASE =====
#define DATABASE_URL "FirebaseDatabaseURL" // e.g. https://your-project.firebaseio.com/

// ===== SENSOR PINS =====
#define PH_PIN 34
#define DHTPIN 23
#define DHTTYPE DHT22

// ===== RELAY PINS =====
#define RELAY_DOSING 26
#define RELAY_CIRC 27

DHT dht(DHTPIN, DHTTYPE);

// ===== CALIBRATION =====
float calibration_value = 2.5;

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ===== DOSING CONTROL =====
unsigned long dosingStart = 0;
bool dosingActive = false;
unsigned long lastDoseTime = 0;

#define DOSE_DURATION 3000   // 3 sec ON
#define COOLDOWN 15000       // 15 sec wait

// ===== READ PH VOLTAGE =====
float readPHVoltage() {
  long sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(PH_PIN);
    delay(10);
  }
  float avg = sum / 20.0;
  return avg * (3.3 / 4095.0);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // ===== RELAY SETUP =====
  pinMode(RELAY_DOSING, OUTPUT);
  pinMode(RELAY_CIRC, OUTPUT);

  digitalWrite(RELAY_DOSING, HIGH); // OFF
  digitalWrite(RELAY_CIRC, HIGH);   // OFF

  // ===== WIFI =====
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi Connected ✅");
  Serial.println(WiFi.localIP());

  // ===== TIME SYNC =====
  configTime(0, 0, "pool.ntp.org");

  Serial.print("Syncing time");
  time_t now = time(nullptr);

  while (now < 100000) {
    Serial.print(".");
    delay(500);
    now = time(nullptr);
  }

  Serial.println("\nTime synced ✅");

  // ===== FIREBASE =====
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = "your_firebase_database_secret"; 

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase Initialized ✅");
}

void loop() {

  // ===== SENSOR READ =====
  float voltage = readPHVoltage();
  float ph = 7 + ((calibration_value - voltage) / 0.18);

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT Error ❌");
    return;
  }

  // ===== SEND TO FIREBASE =====
  Firebase.RTDB.setFloat(&fbdo, "/sensor/ph", ph);
  Firebase.RTDB.setFloat(&fbdo, "/sensor/temp", temp);
  Firebase.RTDB.setFloat(&fbdo, "/sensor/humidity", hum);
  Firebase.RTDB.setFloat(&fbdo, "/sensor/voltage", voltage);

  // ===== READ CONTROL =====
  String pred, dosing, circulation;

  if (Firebase.RTDB.getString(&fbdo, "/control/prediction"))
    pred = fbdo.stringData();

  if (Firebase.RTDB.getString(&fbdo, "/control/dosing"))
    dosing = fbdo.stringData();

  if (Firebase.RTDB.getString(&fbdo, "/control/circulation"))
    circulation = fbdo.stringData();

  // ===== SAFE DOSING LOGIC =====
  unsigned long currentMillis = millis();

  if (dosing == "ON" && !dosingActive && (currentMillis - lastDoseTime > COOLDOWN)) {
    digitalWrite(RELAY_DOSING, LOW);   // ON
    dosingStart = currentMillis;
    dosingActive = true;
    lastDoseTime = currentMillis;
    Serial.println("🔵 Dosing START");
  }

  if (dosingActive && (currentMillis - dosingStart > DOSE_DURATION)) {
    digitalWrite(RELAY_DOSING, HIGH);  // OFF
    dosingActive = false;
    Serial.println("🔴 Dosing STOP");
  }

  // ===== CIRCULATION =====
  if (circulation == "ON") {
    digitalWrite(RELAY_CIRC, LOW);
  } else {
    digitalWrite(RELAY_CIRC, HIGH);
  }

  // ===== DEBUG =====
  Serial.print("pH: ");
  Serial.print(ph);
  Serial.print(" | Temp: ");
  Serial.print(temp);
  Serial.print(" | Hum: ");
  Serial.print(hum);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);

  Serial.print(" || Pred: ");
  Serial.print(pred);
  Serial.print(" | Dosing: ");
  Serial.print(dosing);
  Serial.print(" | Circulation: ");
  Serial.println(circulation);

  Serial.println("----------------------");

  delay(2000);
}