#include <DHT.h>
//include library untuk Wifi
#include "WiFi.h"
#include "HTTPClient.h"
// Pin konfigurasi
#define DHTPIN 26      // Pin data DHT11
#define DHTTYPE DHT11  // Tipe sensor DHT
#define TDS_PIN 32     // Pin analog untuk sensor TDS

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Variabel konversi
float tdsVoltage = 0, tdsValue = 0;
float temperature = 25.0; // Suhu default untuk penghitungan TDS

//siapkan variabel untuk Wifi 
const char* ssid= "Electeical 5G";
const char* pass= "Electrical12";

//siapkan variabel host / sever yang menampung aplikasi web
const char* host = "192.168.100.125";

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(TDS_PIN, INPUT);
  Serial.println("Sistem Monitoring Air");

  //koneksi WiFi
  WiFi.begin(ssid, pass);
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  //berhasil terkoneksi
  Serial.println("Connected");
}

void loop() {
  // Membaca data dari DHT11
  float suhu = dht.readTemperature(); // Membaca suhu
  float kelembapan = dht.readHumidity();    // Membaca kelembaban

if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca data dari sensor DHT11! Memastikan koneksi...");
  } else {
    temperature = suhu; // Memperbarui suhu untuk perhitungan TDS
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C, Kelembaban: ");
    Serial.print(kelembapan);
    Serial.println(" %");
  }


 // Membaca data dari sensor TDS
  tdsVoltage = analogRead(TDS_PIN) * (3.3 / 4095.0); // Konversi tegangan (ESP32 ADC 12-bit, 3.3V)
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); // Koefisien kompensasi suhu
  float compensationVoltage = tdsVoltage / compensationCoefficient; // Tegangan setelah kompensasi
  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage -
              255.86 * compensationVoltage * compensationVoltage +
              857.39 * compensationVoltage) * 0.5; // Formula TDS
  Serial.print("Nilai TDS: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  //kirim data ke server
  WiFiClient client ;
  //inisialisasi port web server 80
  const int httpPort = 80;
  if( !client.connect(host, httpPort) )
  {
    Serial.println("Connection Failed");
    return;
  }

  //kondisi pasti terkoneksi
  //kirim data sensor ke database / web
String Link ;
  HTTPClient http ;

  Link = "http://" + String(host) + "/multisensor/kirimdata.php?tds=" + String(tdsValue) + "&suhu=" + String(suhu) + "&kelembapan=" + String(kelembapan);
  //eksekusi alamat link
  http.begin(Link);
  http.GET();

  //baca respon setelah berhasil nilai sensor
  String respon = http.getString();
  Serial.println(respon);
  http. end();

  // Tunggu 2 detik sebelum membaca ulang
 delay(10000);
}
