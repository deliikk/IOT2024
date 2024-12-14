#include "DHT.h"

// Konfigurasi pin dan variabel
// Sensor TDS
const int tdsPin = 32; // Pin ADC untuk sensor TDS
const float vRef = 3.3; // Tegangan referensi ADC (3.3V)
const int adcResolution = 4095; // Resolusi ADC ESP32
const float tdsFactor = 0.5; // Faktor kalibrasi TDS
const float tdsLowThreshold = 300; // Batas bawah TDS dalam ppm
const float tdsHighThreshold = 1000; // Batas atas TDS dalam ppm

// Sensor pH
const int phPin = 35; // Pin ADC untuk sensor pH
const float phCalibration = 0.0; // Faktor kalibrasi pH (sesuaikan setelah uji)
const float phLowThreshold = 6.5; // Batas bawah pH
const float phHighThreshold = 7.5; // Batas atas pH

// Sensor DHT11
#define DHTPIN 26       // Pin untuk sensor DHT11
#define DHTTYPE DHT11   // Jenis sensor
DHT dht(DHTPIN, DHTTYPE);

// Relay
const int relayTDS = 19;       // Relay 1 untuk TDS
const int relayPH = 18;        // Relay 2 untuk pH
const int relayTemp = 5;      // Relay 3 untuk Suhu
const int relayHumidity = 4;  // Relay 4 untuk Kelembapan

// Ambang batas suhu dan kelembapan
const float tempHighThreshold = 35.0; // Suhu maksimum dalam °C
const float tempLowThreshold = 20.0; // Suhu minimum dalam °C
const float humidityHighThreshold = 80.0; // Kelembapan maksimum dalam %
const float humidityLowThreshold = 30.0; // Kelembapan minimum dalam %

void setup() {
  Serial.begin(115200);
  pinMode(tdsPin, INPUT);
  pinMode(phPin, INPUT);

  // Konfigurasi relay sebagai OUTPUT
  pinMode(relayTDS, OUTPUT);
  pinMode(relayPH, OUTPUT);
  pinMode(relayTemp, OUTPUT);
  pinMode(relayHumidity, OUTPUT);

  dht.begin();
  Serial.println("Sistem Monitoring Sensor Dimulai...");
}

void loop() {
  // === Sensor TDS ===
  int tdsAnalog = analogRead(tdsPin);
  float tdsVoltage = (tdsAnalog / float(adcResolution)) * vRef;
  float tdsValue = (tdsVoltage * 1000 / tdsFactor); // Nilai dalam ppm

  // === Sensor pH ===
  int phAnalog = analogRead(phPin);
  float phVoltage = (phAnalog / float(adcResolution)) * vRef;
  float phValue = 7 + ((2.5 - phVoltage) / 0.18); // Rumus perkiraan pH

  // === Sensor DHT11 ===
  float temperature = dht.readTemperature(); // Temperatur dalam °C
  float humidity = dht.readHumidity();       // Kelembapan dalam %

  // === Kontrol Relay 1 (TDS) ===
  if (tdsValue < tdsLowThreshold || tdsValue > tdsHighThreshold) {
    digitalWrite(relayTDS, HIGH); // Aktifkan relay TDS
    Serial.println("Relay TDS ON: Kondisi tidak normal!");
  } else {
    digitalWrite(relayTDS, LOW); // Matikan relay TDS
    Serial.println("Relay TDS OFF: Kondisi normal.");
  }

  // === Kontrol Relay 2 (pH) ===
  if (phValue < phLowThreshold || phValue > phHighThreshold) {
    digitalWrite(relayPH, HIGH); // Aktifkan relay pH
    Serial.println("Relay pH ON: Kondisi tidak normal!");
  } else {
    digitalWrite(relayPH, LOW); // Matikan relay pH
    Serial.println("Relay pH OFF: Kondisi normal.");
  }

  // === Kontrol Relay 3 (Temperature) ===
  if (temperature < tempLowThreshold || temperature > tempHighThreshold) {
    digitalWrite(relayTemp, HIGH); // Aktifkan relay suhu
    Serial.println("Relay Temp ON: Suhu tidak normal!");
  } else {
    digitalWrite(relayTemp, LOW); // Matikan relay suhu
    Serial.println("Relay Temp OFF: Suhu normal.");
  }

  // === Kontrol Relay 4 (Humidity) ===
  if (humidity < humidityLowThreshold || humidity > humidityHighThreshold) {
    digitalWrite(relayHumidity, HIGH); // Aktifkan relay kelembapan
    Serial.println("Relay Humidity ON: Kelembapan tidak normal!");
  } else {
    digitalWrite(relayHumidity, LOW); // Matikan relay kelembapan
    Serial.println("Relay Humidity OFF: Kelembapan normal.");
  }

  // Tampilkan hasil di Serial Monitor
  Serial.println("=====================================");
  Serial.print("TDS: ");
  Serial.print(tdsValue, 2);
  Serial.println(" ppm");

  Serial.print("pH: ");
  Serial.print(phValue, 2);
  Serial.println("");

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Gagal membaca dari sensor DHT11!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
  }

  Serial.println("=====================================\n");

  delay(2000); // Delay 2 detik
}
