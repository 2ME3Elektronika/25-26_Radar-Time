#include <SPI.h>
#include <MFRC522.h>

// PINES REALES DE TU WPB109
#define SS_PIN   21   // SDA del RC522
#define RST_PIN  22   // RST del RC522

#define SCK_PIN  18
#define MISO_PIN 19
#define MOSI_PIN 23

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== DEBUG RC522 ESP32 WPB109 ===");

  // SPI CON PINES EXPLÍCITOS
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  Serial.println("SPI iniciado");

  mfrc522.PCD_Init();
  Serial.println("RC522 inicializado");

  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print("Version RC522: 0x");
  Serial.println(v, HEX);

  if (v == 0x00 || v == 0xFF) {
    Serial.println("❌ RC522 NO detectado");
  } else {
    Serial.println("✅ RC522 detectado");
  }

  Serial.println("Acerque una tarjeta...");
}

void loop() {

  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(200);
    return;
  }

  Serial.println("📛 Tarjeta detectada");

  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("❌ No se pudo leer UID");
    return;
  }

  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(2000);
}
