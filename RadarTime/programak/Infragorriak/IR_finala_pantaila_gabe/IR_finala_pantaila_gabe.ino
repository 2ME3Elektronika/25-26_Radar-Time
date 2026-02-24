const int sentsore1Pin = 35;
const int sentsore2Pin = 34;
const float distantzia = 0.235;


// Aumentamos el margen. El ESP32 llega a 4095.
// Si hay mucho ruido, sube este valor a 1000 o 1500.
const int UMBRAL_DISPARO = 800;


unsigned long hasieraDenbora = 0;
bool s1Aktibatuta = false;


// Función para leer el sensor de forma limpia (promediada)
int leerLimpio(int pin) {
  long suma = 0;
  for(int i=0; i<20; i++) {
    suma += analogRead(pin);
  }
  return suma / 20;
}


void setup() {
  Serial.begin(115200);
 
  // Configuración específica para ESP32 (bajar sensibilidad al ruido)
  analogSetAttenuation(ADC_11db);
 
  Serial.println("--- ESTABILIZANDO SENSORES ---");
  delay(2000); // Tiempo para que el sensor Sharp se cargue
  Serial.println("SISTEMA LISTO. Esperando coche...");
}


void loop() {
  int v1 = leerLimpio(sentsore1Pin);
  int v2 = leerLimpio(sentsore2Pin);


  // Debug: Descomenta la línea de abajo para ver los valores reales en el monitor
  // Serial.printf("S1: %d | S2: %d\n", v1, v2);


  // Lógica de detección
  if (!s1Aktibatuta && v1 > UMBRAL_DISPARO) {
    hasieraDenbora = millis();
    s1Aktibatuta = true;
    Serial.println(">>> S1 ACTIVADO");
    // Bloqueamos un instante para evitar rebotes del mismo coche
    delay(50);
  }


  if (s1Aktibatuta) {
    if (v2 > UMBRAL_DISPARO) {
      unsigned long denboraMs = millis() - hasieraDenbora;
     
      // Filtro de velocidad absurda (menos de 10ms es error)
      if (denboraMs > 10) {
        float abiadura = (distantzia / (denboraMs / 1000.0)) * 3.6;
        Serial.print("\n***************************");
        Serial.print("\nABIADURA: "); Serial.print(abiadura); Serial.println(" km/h");
        
        // GEHITUTAKO MEZUA:
        if (abiadura > 10.0) {
          Serial.println("Kontuz abiadura murriztu");
        }
        
        Serial.println("***************************\n");
      }
     
      s1Aktibatuta = false;
      delay(1500); // Pausa tras detectar
    }


    // Timeout: Si en 2 segundos no llega al S2, reset.
    if (millis() - hasieraDenbora > 2000) {
      s1Aktibatuta = false;
      Serial.println("Reset: Detección fallida o demasiado lenta.");
    }
  }
}
