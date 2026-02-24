const int pinSensor = 4;
volatile int contadorVuelcos = 0;
int ultimoContador = -1; // Para saber si el número ha cambiado

// CONFIGURACIÓN
const int UMBRAL_LLUVIA = 20; 

void IRAM_ATTR detectarVuelco() {
  static unsigned long ultimaInterrupcion = 0;
  unsigned long tiempoInterrupcion = millis();
  if (tiempoInterrupcion - ultimaInterrupcion > 200) {
    contadorVuelcos++;
    ultimaInterrupcion = tiempoInterrupcion;
  }
}

void setup() {
  Serial.begin(115200);
  // Usamos INPUT_PULLUP para asegurar que el pin no detecte basura estática
  pinMode(pinSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSensor), detectarVuelco, FALLING);
  
  Serial.println("Monitor de Pluviómetro Iniciado...");
  Serial.println("Esperando primer vuelco...");
}

void loop() {
  // 1. Mostrar progreso cada vez que hay un vuelco nuevo
  if (contadorVuelcos != ultimoContador && contadorVuelcos < UMBRAL_LLUVIA && contadorVuelcos > 0) {
    Serial.print("Vuelcos acumulados: ");
    Serial.println(contadorVuelcos);
    ultimoContador = contadorVuelcos;
  }

  // 2. Condición de ALERTA (solo al llegar al umbral)
  if (contadorVuelcos >= UMBRAL_LLUVIA) {
    float lluviaTramo = contadorVuelcos * 0.2794;

    Serial.println("\n**********************************");
    Serial.println("ESTA LLOVIENDO, MODERA TU VELOCIDAD");
    Serial.print("Agua acumulada: ");
    Serial.print(lluviaTramo);
    Serial.println(" mm");
    Serial.println("**********************************\n");
   
    // Reiniciamos variables
    contadorVuelcos = 0; 
    ultimoContador = 0;
  }
}
