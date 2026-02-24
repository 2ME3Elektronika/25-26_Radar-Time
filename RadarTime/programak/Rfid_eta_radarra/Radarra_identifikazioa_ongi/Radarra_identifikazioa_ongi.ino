#include <SPI.h>
#include <MFRC522.h>

// --- CONFIGURACIÓN RFID ---
#define SS_PIN   21
#define RST_PIN  22
#define SCK_PIN  18
#define MISO_PIN 19
#define MOSI_PIN 23

MFRC522 mfrc522(SS_PIN, RST_PIN);

struct Persona {
  byte uid[4];
  const char* nombre;
};

Persona personas[] = {
  {{0xB5, 0x3A, 0xC9, 0xE5}, "Itzurun Aristi Zarate"}
};
const int totalPersonas = sizeof(personas) / sizeof(personas[0]);
String nombreConductor = "Desconocido";

// --- CONFIGURACIÓN VELOCIDAD ---
const int sentsore1Pin = 35;
const int sentsore2Pin = 34;
const float distantzia = 0.235;
const int UMBRAL_DISPARO = 2000; 

unsigned long hasieraDenbora = 0;
bool s1Aktibatuta = false;

// --- FUNCIONES AUXILIARES ---
int leerLimpio(int pin) {
  long suma = 0;
  for(int i=0; i<15; i++) { suma += analogRead(pin); }
  return suma / 15;
}

bool compararUID(byte *uid1, byte *uid2, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  
  // Iniciar SPI y RFID
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();
  
  // Iniciar Sensores
  analogSetAttenuation(ADC_11db);
  
  Serial.println("=== SISTEMA UNIFICADO LISTO ===");
  Serial.println("Identifíquese con su tarjeta RFID...");
}

void loop() {
  // --- PARTE 1: LECTURA RFID ---
  // Revisamos si hay una tarjeta nueva sin bloquear el código
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    bool encontrada = false;
    for (int i = 0; i < totalPersonas; i++) {
      if (compararUID(mfrc522.uid.uidByte, personas[i].uid, mfrc522.uid.size)) {
        nombreConductor = personas[i].nombre;
        Serial.print("\n👤 Conductor actual: ");
        Serial.println(nombreConductor);
        encontrada = true;
      }
    }
    if (!encontrada) {
      nombreConductor = "Desconocido";
      Serial.println("\n❌ Tarjeta no registrada");
    }
    mfrc522.PICC_HaltA(); // Detener lectura de la misma tarjeta
  }

  // --- PARTE 2: CÁLCULO DE VELOCIDAD ---
  int v1 = leerLimpio(sentsore1Pin);
  
  if (!s1Aktibatuta && v1 > UMBRAL_DISPARO) {
    delay(5); // Pequeño filtro de ruido
    if(leerLimpio(sentsore1Pin) > UMBRAL_DISPARO) {
      hasieraDenbora = millis();
      s1Aktibatuta = true;
      Serial.println(">>> S1 ACTIVADO");
    }
  }

  if (s1Aktibatuta) {
    int v2 = leerLimpio(sentsore2Pin);
    
    if (v2 > UMBRAL_DISPARO) {
      unsigned long denboraMs = millis() - hasieraDenbora;
     
      if (denboraMs > 20) {
        float abiadura = (distantzia / (denboraMs / 1000.0)) * 3.6;
        
        Serial.print("\n***************************");
        Serial.print("\nCONDUCTOR: "); Serial.println(nombreConductor);
        Serial.print("VELOCIDAD: "); Serial.print(abiadura); Serial.println(" km/h");
        
        if (abiadura > 10.0) {
          Serial.print("¡PELIGRO "); 
          Serial.println("! Kontuz abiadura murriztu");
        }
        Serial.println("***************************\n");
      }
     
      s1Aktibatuta = false;
      delay(1500); // Pausa para evitar doble detección
    }

    // Timeout de 3 segundos
    if (millis() - hasieraDenbora > 3000) {
      s1Aktibatuta = false;
      Serial.println("Reset: Detección de velocidad cancelada.");
    }
  }
}
