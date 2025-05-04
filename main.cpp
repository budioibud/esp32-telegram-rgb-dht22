#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ctype.h>  // <-- Tambahkan ini

// === Konfigurasi DHT dan LED ===
#define DHTPIN 4
#define DHTTYPE DHT22
#define RED_PIN    27
#define GREEN_PIN  26
#define BLUE_PIN   25


DHT dht(DHTPIN, DHTTYPE);

// === Info WiFi dan Telegram ===
const char* ssid = "____";
const char* password = "___";
String botToken = "____";
int64_t chatID = ____;


WiFiClientSecure client;
int lastUpdateId = 0;
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// === Deklarasi fungsi ===
void kirimPesanTelegram(String pesan);
void bacaSensorDanKirim();
void cekPerintahTelegram();
String urlencode(String str);  // Tambahkan ini

void setup() {
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Terhubung!");
  client.setInsecure();  // Nonaktifkan verifikasi sertifikat SSL
  kirimPesanTelegram("📡 Sistem siap! Gunakan /start untuk melihat perintah.");
}

void loop() {
  if (millis() - lastTime > timerDelay) {
    bacaSensorDanKirim();
    lastTime = millis();
  }
  cekPerintahTelegram();
  delay(3000);
}

void bacaSensorDanKirim() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) return;

  String pesan = "🌡 Suhu: " + String(temp, 1) + " °C\n💧 Kelembapan: " + String(hum, 1) + " %";
  kirimPesanTelegram(pesan);
}

void kirimPesanTelegram(String pesan) {
  if (client.connect("api.telegram.org", 443)) {
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    String encodedPesan = urlencode(pesan);
    String body = "chat_id=" + String(chatID) + "&text=" + encodedPesan;
    String request = "POST /bot" + botToken + "/sendMessage HTTP/1.1\r\n" +
                     "Host: api.telegram.org\r\n" +
                     "Content-Type: application/x-www-form-urlencoded\r\n" +
                     "Content-Length: " + String(body.length()) + "\r\n\r\n" +
                     body;

    client.print(request);
    delay(100);
    while (client.available()) {
      String line = client.readStringUntil('\n');
      if (line.startsWith("{\"ok\"")) {
        Serial.println("🟢 Respon Telegram:\n" + line);
      }
    }
    client.stop();
  } else {
    Serial.println("❌ Gagal konek ke Telegram (sendMessage)");
  }
}

void cekPerintahTelegram() {
  if (!client.connect("api.telegram.org", 443)) {
    Serial.println("❌ Gagal konek saat getUpdates()");
    return;
  }

  String url = "/bot" + botToken + "/getUpdates?offset=" + String(lastUpdateId + 1);
  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: api.telegram.org");
  client.println("Connection: close");
  client.println();

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  String payload = client.readString();
  client.stop();

  Serial.println("🟢 Payload Telegram:");
  Serial.println(payload);

  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("❌ Gagal parse JSON");
    return;
  }

  JsonArray result = doc["result"];
  if (result.size() == 0) return;

  for (JsonObject update : result) {
    lastUpdateId = update["update_id"]; // penting!
    if (!update.containsKey("message")) continue;
    String command = update["message"]["text"];
    command.trim();  // Hapus spasi/tabs/newline di awal/akhir
    int64_t senderID = update["message"]["chat"]["id"];
    if (senderID != chatID) continue;




    Serial.println("🔍 Command diterima: " + command);

    if (command == "/start") {
      kirimPesanTelegram(
        "📘 *Panduan Penggunaan*\n\n"
        "/on → Nyalakan semua warna\n"
        "/off → Matikan semua\n"
        "/red_on /red_off\n"
        "/green_on /green_off\n"
        "/blue_on /blue_off"
      );
    } else if (command == "/on") {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      kirimPesanTelegram("💡 Semua lampu dinyalakan.");
    } else if (command == "/off") {
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      kirimPesanTelegram("🔌 Semua lampu dimatikan.");
    } else if (command == "/red_on") {
      digitalWrite(RED_PIN, LOW);
      kirimPesanTelegram("🔴 Lampu merah nyala.");
    } else if (command == "/red_off") {
      digitalWrite(RED_PIN, HIGH);
      kirimPesanTelegram("🔴 Lampu merah mati.");
    } else if (command == "/green_on") {
      digitalWrite(GREEN_PIN, LOW);
      kirimPesanTelegram("🟢 Lampu hijau nyala.");
    } else if (command == "/green_off") {
      digitalWrite(GREEN_PIN, HIGH);
      kirimPesanTelegram("🟢 Lampu hijau mati.");
    } else if (command == "/blue_on") {
      digitalWrite(BLUE_PIN, LOW);
      kirimPesanTelegram("🔵 Lampu biru nyala.");
    } else if (command == "/blue_off") {
      digitalWrite(BLUE_PIN, HIGH);
      kirimPesanTelegram("🔵 Lampu biru mati.");
    }
    
  }
}


// Fungsi encode URL untuk spasi dan simbol
String urlencode(String str) {
  String encoded = "";
  char c;
  char code0, code1;
  char code[] = "0123456789ABCDEF";

  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      code0 = code[(c >> 4) & 0xF];
      code1 = code[c & 0xF];
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  return encoded;
}
