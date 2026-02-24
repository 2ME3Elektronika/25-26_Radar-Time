#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ====================== WIFI ======================
const char* ssid = "ETT IKASLEAK";
const char* password = "1Kasl3@na1z";

// ====================== TELEGRAM ======================
#define BOT_TOKEN "8488708158:AAF_cnimnj7TIzDfBK4pnRR9YUxKCXN5Kp8"
#define CHAT_ID "5824323979"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastTime = 0;
const unsigned long interval = 20000; // 20 segundos

void setup() {
  Serial.begin(115200);

  // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  client.setInsecure(); // necesario para HTTPS
  delay(2000);          // espera un poco antes de enviar mensajes
}

void loop() {
  unsigned long now = millis();

  if (now - lastTime > interval) {
    lastTime = now;

    String mensaje = "Mezua bidalita";
    bool enviado = bot.sendMessage(CHAT_ID, mensaje, "");

    Serial.print("Mensaje enviado a Telegram: ");
    Serial.println(mensaje);

    Serial.print("Resultado del envío: ");
    Serial.println(enviado ? "OK" : "FALLO");
  }
}