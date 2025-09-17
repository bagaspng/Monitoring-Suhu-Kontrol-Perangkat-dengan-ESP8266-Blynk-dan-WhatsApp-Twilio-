// === Blynk Config ===
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""
// === Include library setelah definisi ===
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"


char ssid[] = "";
char pass[] = "";

// Twilio
const char* TWILIO_ACCOUNT_SID = "AXXXXXXXXX"; // GANTI
const char* TWILIO_AUTH_TOKEN   = "XXXXXXXX"; // GANTI
String TWILIO_FROM = "whatsapp:+"; // Sandbox default (atau nomor WA Twilio-mu)
String TWILIO_TO   = "whatsapp:"; // Nomor WA kamu
// =======================================

// DHT11
#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Lampu indikator suhu
#define LAMPU1 D0
#define LAMPU2 D1
#define LAMPU3 D2

// Pin perangkat
#define KULKAS D7
#define TV     D8  // hati2: D8 harus LOW saat boot

int statusKulkas = 0;
int statusTV     = 0;

// Ambang & cooldown notifikasi
float AMBANG_SUHU = 27.0;
unsigned long NOTIF_COOLDOWN_MS = 5UL * 60UL * 1000UL; // 5 menit
unsigned long lastNotifMs = 0;

// ===== Kirim WA via Twilio =====
bool sendWhatsApp(const String &bodyMsg) {
  WiFiClientSecure client;
  client.setTimeout(15000);
  client.setInsecure(); // DEMO: non-verify TLS. Produksi: gunakan sertifikat/Fingerprint!

  const char* host = "api.twilio.com";
  const int httpsPort = 443;

  if (!client.connect(host, httpsPort)) {
    Serial.println("[Twilio] Koneksi gagal");
    return false;
  }

  // Basic Auth: base64(AccountSID:AuthToken)
  // Trik sederhana: Twilio juga menerima header Authorization: Basic ...,
  // tapi di banyak contoh lebih mudah kirim credential di URL dengan client.print.
  // Di sini kita pakai header Authorization sesuai standar.

  // Buat credential "sid:token" lalu Base64 manual minimalis
  String creds = String(TWILIO_ACCOUNT_SID) + ":" + String(TWILIO_AUTH_TOKEN);

  // Base64 encoder kecil
  String b64 = "";
  static const char PROGMEM *b64alphabet =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int val = 0, valb = -6;
  for (uint8_t c : creds) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      b64 += b64alphabet[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }
  if (valb > -6) b64 += b64alphabet[((val << 8) >> (valb + 8)) & 0x3F];
  while (b64.length() % 4) b64 += '=';

  // Endpoint Twilio
  String url = "/2010-04-01/Accounts/" + String(TWILIO_ACCOUNT_SID) + "/Messages.json";

  // x-www-form-urlencoded body
  // Pastikan di-URL-encode jika ada karakter khusus
  String body = "To=" + TWILIO_TO +
                "&From=" + TWILIO_FROM +
                "&Body=" + String(bodyMsg);

  // Header
  String request =
      String("POST ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Authorization: Basic " + b64 + "\r\n" +
      "Content-Type: application/x-www-form-urlencoded\r\n" +
      "Content-Length: " + String(body.length()) + "\r\n" 
      "Connection: close\r\n\r\n" +
      body;

  client.print(request);

  // Baca response singkat
  String line;
  int statusCode = -1;
  if (client.connected()) {
    // Baris status, contoh: HTTP/1.1 201 CREATED
    line = client.readStringUntil('\n');
    // Ambil kode status
    int sp1 = line.indexOf(' ');
    int sp2 = line.indexOf(' ', sp1 + 1);
    if (sp1 > 0 && sp2 > sp1) {
      statusCode = line.substring(sp1 + 1, sp2).toInt();
    }
  }

  // Dump beberapa baris untuk debug (opsional)
  while (client.connected()) {
    String h = client.readStringUntil('\n');
    if (h == "\r") break; // end header
  }
  String resp = client.readString(); // body json
  Serial.println("[Twilio] Status: " + String(statusCode));
  Serial.println("[Twilio] Resp: " + resp);

  // Twilio kembalikan 201 saat sukses buat message
  return (statusCode == 201);
}

// ===== Notifikasi suhu tinggi (dengan cooldown) =====
void maybeNotifyHighTemp(float suhu, float kelembaban) {
  unsigned long now = millis();
  if (suhu > AMBANG_SUHU && (now - lastNotifMs >= NOTIF_COOLDOWN_MS)) {
    String pesan = "Peringatan! Suhu di atas ambang.\n"
                   "Suhu: " + String(suhu, 1) + "°C\n"
                   "RH: " + String(kelembaban, 1) + "%\n"
                   "Kulkas: " + String(statusKulkas ? "Hidup" : "Mati") + ", TV: " + String(statusTV ? "Hidup" : "Mati");
    if (sendWhatsApp(pesan)) {
      lastNotifMs = now;
      Serial.println("[WA] Notifikasi terkirim.");
    } else {
      Serial.println("[WA] Gagal kirim notifikasi.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  dht.begin();

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(LAMPU1, OUTPUT);
  pinMode(LAMPU2, OUTPUT);
  pinMode(LAMPU3, OUTPUT);

  pinMode(KULKAS, OUTPUT);
  pinMode(TV, OUTPUT);
  digitalWrite(KULKAS, LOW);
  digitalWrite(TV, LOW);

  Serial.println("Sistem monitoring suhu & kelembaban + kontrol Kulkas/TV + WA Twilio");
}

void loop() {
  Blynk.run();

  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 2000) {
    lastMillis = millis();

    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    if (isnan(kelembaban) || isnan(suhu)) {
      Serial.println("Gagal membaca data dari sensor DHT11!");
      return;
    }

    Serial.print("Suhu: "); Serial.print(suhu);
    Serial.print(" °C | Kelembaban: "); Serial.print(kelembaban);
    Serial.print(" % | Kulkas: "); Serial.print(statusKulkas ? "Hidup" : "Mati");
    Serial.print(" | TV: "); Serial.println(statusTV ? "Hidup" : "Mati");

    // Kirim ke Blynk
    Blynk.virtualWrite(V0, suhu);
    Blynk.virtualWrite(V1, kelembaban);

    // Indikator lampu
    if (suhu < 25) {
      digitalWrite(LAMPU1, HIGH); digitalWrite(LAMPU2, LOW);  digitalWrite(LAMPU3, LOW);
      Blynk.virtualWrite(V2, 1);  Blynk.virtualWrite(V3, 0);  Blynk.virtualWrite(V4, 0);
    } else if (suhu <= 27) {
      digitalWrite(LAMPU1, LOW);  digitalWrite(LAMPU2, HIGH); digitalWrite(LAMPU3, LOW);
      Blynk.virtualWrite(V2, 0);  Blynk.virtualWrite(V3, 1);  Blynk.virtualWrite(V4, 0);
    } else {
      digitalWrite(LAMPU1, LOW);  digitalWrite(LAMPU2, LOW);  digitalWrite(LAMPU3, HIGH);
      Blynk.virtualWrite(V2, 0);  Blynk.virtualWrite(V3, 0);  Blynk.virtualWrite(V4, 1);
    }

    // V5 status ringkas
    String pesan = "Suhu: " + String(suhu) + "°C\n" +
                   "Kelembaban: " + String(kelembaban) + "%\n" +
                   "Kulkas: " + String(statusKulkas ? "Hidup" : "Mati") + "\n" +
                   "TV: " + String(statusTV ? "Hidup" : "Mati");
    Blynk.virtualWrite(V5, pesan);

    // === Trigger notifikasi WA jika perlu ===
    maybeNotifyHighTemp(suhu, kelembaban);
  }
}

// Blynk Switch Kulkas (V6)
BLYNK_WRITE(V6) {
  statusKulkas = param.asInt();
  digitalWrite(KULKAS, statusKulkas);
}

// Blynk Switch TV (V7)
BLYNK_WRITE(V7) {
  statusTV = param.asInt();
  digitalWrite(TV, statusTV);
}
