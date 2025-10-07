#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// --- GANTI KREDENSIAL WIFI DI BAWAH INI ---
const char* ssid = "waw123";
const char* password = "kwekkwek";
// ---------------------------------------------

const char* mqtt_server = "10.65.251.71"; 

// Konfigurasi DHT11
#define DHTPIN 4      // Pin DATA DHT11 terhubung ke pin D4 di ESP32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Inisialisasi MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
// Siapkan buffer untuk mengubah float ke string
char tempString[8];
char humString[8];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  Serial.print("IP address ESP32: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Mencoba koneksi ke MQTT Broker...");
    if (client.connect("ESP32_DHT11_Client_V2")) { 
      Serial.println("terhubung ke broker lokal!");
    } else {
      Serial.print("gagal, kode error rc=");
      Serial.print(client.state());
      Serial.println(" | Coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Gagal membaca data dari sensor DHT11!");
      return;
    }

    // 1. Ubah nilai float suhu ke string, lalu kirim ke topic "temp"
    dtostrf(temperature, 1, 2, tempString); // (float, min width, precision, buffer)
    Serial.printf("Kirim ke topic 'temp': %s \n", tempString);
    client.publish("temp", tempString);

    // 2. Ubah nilai float kelembapan ke string, lalu kirim ke topic "hum"
    dtostrf(humidity, 1, 2, humString);
    Serial.printf("Kirim ke topic 'hum': %s \n", humString);
    client.publish("hum", humString);
    // -------------------------
  }
}