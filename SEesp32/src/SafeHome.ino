// SafeHome - Sistema de seguridad para hogar basado en ESP32
// Proyecto final - Sistemas Empotrados - ITSON
//
// Hardware:
//   AHT20  -> I2C SDA=21, SCL=22 (temperatura + humedad)
//   BMP280 -> I2C SDA=21, SCL=22 (presion)
//   PIR    -> pin 13 (deteccion de movimiento)
//   LDR    -> pin 34 (nivel de luz, ADC1)
//   Relay  -> pin 26 (control de iluminacion)
//   Servo  -> pin 27 (control de acceso)

#include <Arduino.h>
#include <LittleFS.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "api.h"
#include "tcp_client.h"
#include "WiFiManagerESP.h"

// ---------------------------------------------------------------------------
// Maquina de estados del sistema
// ---------------------------------------------------------------------------
enum Estado {
    IDLE,        // Esperando el intervalo de sensado o un cambio de config
    SENSING,     // Leyendo todos los sensores
    ALERTING,    // Evaluando umbrales y actualizando actuadores
    CONFIGURING  // Procesando un cambio de parametros recibido via API
};

// ---------------------------------------------------------------------------
// Variables globales compartidas entre modulos
// (se pasan como punteros a initAPI para evitar acoplar los modulos)
// ---------------------------------------------------------------------------
Config config = {
    DEFAULT_UMBRAL_LUZ,
    DEFAULT_UMBRAL_TEMP,
    DEFAULT_TIEMPO_SERVO,
    DEFAULT_INTERVALO_SENSADO
};

const char* WIFI_SSID     = "INFINITUM910D";
const char* WIFI_PASSWORD = "BvcVHwz2kN";
const char* WIFI_USER     = "";
const char* WIFI_PASS     = "";

SensorData ultimaLectura;

// La API corre en un task de FreeRTOS; volatile garantiza visibilidad entre tareas
volatile bool configCambiada = false;

// ---------------------------------------------------------------------------
// Variables de control del ciclo principal
// ---------------------------------------------------------------------------
static Estado         estadoActual        = IDLE;
static unsigned long  tiempoUltimoSensado = 0;

// Gestor de WiFi con credenciales definidas en config.h
WiFiManagerESP wifiManager(WIFI_SSID, WIFI_PASSWORD, WIFI_USER, WIFI_PASS);

// ---------------------------------------------------------------------------
// setup()
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println("\n=== SafeHome v1.0 ===");

    // Inicializar hardware
    initSensors();
    initActuators();

    // Montar sistema de archivos para servir la interfaz web
    if (!LittleFS.begin(true)) {
        Serial.println("[Sistema] ERROR: No se pudo montar LittleFS.");
        Serial.println("[Sistema] Sube los archivos de /data con 'Upload Filesystem Image'.");
    } else {
        Serial.println("[Sistema] LittleFS montado correctamente.");
    }

    // Conectar al WiFi y autenticar portal cautivo ITSON
    wifiManager.connectToWiFi();

    // Inicializar la API REST (servidor web en puerto 80)
    initAPI(&config, &ultimaLectura, &configCambiada);

    // Lectura inicial para que /api/status tenga datos desde el primer request
    ultimaLectura = readSensors();

    Serial.println("[Sistema] SafeHome listo. Iniciando ciclo principal...\n");
}

// ---------------------------------------------------------------------------
// loop() - maquina de estados no bloqueante
// ---------------------------------------------------------------------------
void loop() {
    unsigned long ahora = millis();

    // El tick de actuadores debe ejecutarse en cada iteracion del loop
    // para manejar el temporizador del servo sin usar delay()
    tickActuators(config);

    switch (estadoActual) {

        // --- IDLE: esperar el intervalo de sensado o un cambio de configuracion ---
        case IDLE:
            if (configCambiada) {
                estadoActual = CONFIGURING;
            } else if (ahora - tiempoUltimoSensado >= (unsigned long)config.intervaloSensado) {
                estadoActual = SENSING;
            }
            break;

        // --- SENSING: leer todos los sensores ---
        case SENSING:
            ultimaLectura        = readSensors();
            tiempoUltimoSensado  = ahora;

            Serial.println("--- Lectura de sensores ---");
            Serial.print("  Temp AHT20 : "); Serial.print(ultimaLectura.tempAHT, 1); Serial.println(" C");
            Serial.print("  Humedad    : "); Serial.print(ultimaLectura.humedad,  1); Serial.println(" %");
            Serial.print("  Temp BMP280: "); Serial.print(ultimaLectura.tempBMP,  1); Serial.println(" C");
            Serial.print("  Presion    : "); Serial.print(ultimaLectura.presion,  1); Serial.println(" hPa");
            Serial.print("  LDR        : "); Serial.println(ultimaLectura.ldr);
            Serial.print("  PIR        : "); Serial.println(ultimaLectura.pir ? "MOVIMIENTO DETECTADO" : "Sin movimiento");

            estadoActual = ALERTING;
            break;

        // --- ALERTING: evaluar umbrales y actualizar actuadores ---
        case ALERTING:
            updateActuators(ultimaLectura, config);
            sendDataTCP(ultimaLectura);
            estadoActual = IDLE;
            break;

        // --- CONFIGURING: acuse de recibo del cambio de configuracion ---
        case CONFIGURING:
            configCambiada = false;
            Serial.println("--- Configuracion actualizada via API ---");
            Serial.print("  umbralLuz        : "); Serial.println(config.umbralLuz);
            Serial.print("  umbralTemp       : "); Serial.println(config.umbralTemp, 1);
            Serial.print("  tiempoServo      : "); Serial.println(config.tiempoServo);
            Serial.print("  intervaloSensado : "); Serial.println(config.intervaloSensado);
            estadoActual = IDLE;
            break;
    }
}
