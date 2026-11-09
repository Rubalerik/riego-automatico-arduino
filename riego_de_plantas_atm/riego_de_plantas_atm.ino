/*
 * SISTEMA DE RIEGO AUTOMÁTICO - VIDEO 1
 * Canal: [Tu Canal]
 * 
 * COMPONENTES:
 * - Arduino UNO
 * - Sensor humedad suelo (A0)
 * - Transistor (Pin 7)
 * - LED indicador (Pin 3)
 */

// ========== CONFIGURACIÓN DE PINES ==========
#define PIN_TRANSISTOR  9      // Control de la bomba de agua
#define PIN_SENSOR A0     // Sensor de humedad de suelo
#define PIN_LED    3      // LED indicador (opcional)

// ========== CONFIGURACIÓN DE UMBRALES ==========
#define UMBRAL_SECO     850   // Valor cuando la tierra está SECA (ajustar)
#define UMBRAL_HUMEDO   400   // Valor cuando la tierra está HÚMEDA (ajustar)
#define TIEMPO_RIEGO_MS 8000  // 8 segundos de riego
#define TIEMPO_ESPERA_MS 5000 // 5 segundos entre lecturas

// ========== VARIABLES DEL SISTEMA ==========
int humedad;                    // Valor actual del sensor
bool bombaActivada = false;     // Estado de la bomba
unsigned long tiempoRiegoInicio = 0; // Para controlar tiempo de riego

// ========== SETUP ==========
void setup() {
  Serial.begin(9600);
  Serial.println("=== SISTEMA DE RIEGO AUTOMÁTICO INICIADO ===");
  
  pinMode(PIN_TRANSISTOR, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SENSOR, INPUT);
  
  // INICIALMENTE APAGADO
  apagarBomba();
  
  Serial.println("Listo para monitorear humedad...");
  Serial.println();
}

// ========== LOOP PRINCIPAL ==========
void loop() {
  // 1. LEER SENSOR
  humedad = analogRead(PIN_SENSOR);
  
  // 2. MOSTRAR EN MONITOR SERIAL
  mostrarLectura(humedad);
  
  // 3. VERIFICAR SI ESTAMOS REGANDO ACTUALMENTE
  if (bombaActivada) {
    gestionarRiegoEnCurso();
  } else {
    // 4. TOMAR DECISIÓN BASADA EN HUMEDAD
    tomarDecision(humedad);
  }
  
  // 5. PEQUEÑA PAUSA SIN BLOQUEAR
  delay(100);
}

// ========== FUNCIONES AUXILIARES ==========

void mostrarLectura(int valor) {
  Serial.print("Humedad: ");
  Serial.print(valor);
  Serial.print(" | Estado: ");
  
  if (valor > UMBRAL_SECO) {
    Serial.print("SECO");
  } else if (valor < UMBRAL_HUMEDO) {
    Serial.print("HÚMEDO");
  } else {
    Serial.print("ÓPTIMO");
  }
  Serial.println();
}

void tomarDecision(int humedad) {
  if (humedad > UMBRAL_SECO) {
    // TIERRA MUY SECA -> REGAR
    Serial.println("¡TIERRA SECA! Activando riego...");
    iniciarRiego();
    
  } else if (humedad < UMBRAL_HUMEDO) {
    // TIERRA MUY HÚMEDA -> NO REGAR
    Serial.println("Tierra húmeda. Bomba APAGADA.");
    apagarBomba();
    
  } else {
    // HUMEDAD ÓPTIMA -> MANTENER
    Serial.println("Humedad óptima. Todo en orden.");
    apagarBomba();
  }
}

void iniciarRiego() {
  digitalWrite(PIN_RELAY, HIGH);   // Relay ACTIVE LOW -> ENCIENDE
  digitalWrite(PIN_LED, HIGH);    // LED encendido como indicador
  bombaActivada = true;
  tiempoRiegoInicio = millis();
}

void apagarBomba() {
  digitalWrite(PIN_RELAY, LOW);  // Relay ACTIVE LOW -> APAGA
  digitalWrite(PIN_LED, LOW);     // LED apagado
  bombaActivada = false;
}

void gestionarRiegoEnCurso() {
  unsigned long tiempoActual = millis();
  unsigned long tiempoTranscurrido = tiempoActual - tiempoRiegoInicio;
  
  if (tiempoTranscurrido >= TIEMPO_RIEGO_MS) {
    // TIEMPO DE RIEGO COMPLETADO
    Serial.println("Riego completado. Apagando bomba...");
    apagarBomba();
    Serial.println("Esperando " + String(TIEMPO_ESPERA_MS/1000) + " segundos...");
    delay(TIEMPO_ESPERA_MS); // Breve espera para absorción
  } else {
    // MOSTRAR CUENTA REGRESIVA
    int segundosRestantes = (TIEMPO_RIEGO_MS - tiempoTranscurrido) / 1000;
    Serial.print("Regando... Tiempo restante: ");
    Serial.print(segundosRestantes);
    Serial.println(" segundos");
  }
}
