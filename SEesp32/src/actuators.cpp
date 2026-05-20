#include "actuators.h"
#include <ESP32Servo.h>

static Servo miServo;

// Estado interno de los actuadores
static bool relayActivo      = false;
static bool servoAbierto     = false;
static bool servoTimerActivo = false;
static unsigned long servoInicioMs = 0;

void initActuators() {
    // Configurar relay en estado apagado
    pinMode(PIN_RELE, OUTPUT);
    digitalWrite(PIN_RELE, LOW);
    relayActivo = false;

    // Configurar servo en posicion cerrada (0 grados)
    miServo.setPeriodHertz(50);
    miServo.attach(PIN_SERVO, 500, 2400);
    miServo.write(0);
    servoAbierto     = false;
    servoTimerActivo = false;

    Serial.println("[Actuadores] Relay y servo inicializados en estado seguro.");
}

void updateActuators(const SensorData& data, const Config& config) {
    // --- Control del relay: activar si la luz ambiente es baja ---
    bool debeActivarRelay = (data.ldr < config.umbralLuz);
    if (debeActivarRelay != relayActivo) {
        relayActivo = debeActivarRelay;
        digitalWrite(PIN_RELE, relayActivo ? HIGH : LOW);
        Serial.print("[Actuadores] Relay: ");
        Serial.println(relayActivo ? "ACTIVADO (luz baja)" : "APAGADO");
    }

    // --- Alerta de temperatura ---
    if (data.tempAHT >= config.umbralTemp) {
        Serial.print("[Actuadores] ALERTA: Temperatura elevada -> ");
        Serial.print(data.tempAHT);
        Serial.println(" C");
    }

    // --- Control del servo: abrir al detectar movimiento ---
    // Solo se abre si el servo esta cerrado y no hay un temporizador en curso
    if (data.pir && !servoAbierto && !servoTimerActivo) {
        miServo.write(90);
        servoAbierto     = true;
        servoTimerActivo = true;
        servoInicioMs    = millis();
        Serial.println("[Actuadores] Servo: ABIERTO (movimiento detectado).");
    }
}

void tickActuators(const Config& config) {
    // Cerrar el servo cuando vence el tiempo configurado
    if (servoTimerActivo) {
        if (millis() - servoInicioMs >= (unsigned long)config.tiempoServo) {
            miServo.write(0);
            servoAbierto     = false;
            servoTimerActivo = false;
            Serial.println("[Actuadores] Servo: CERRADO (temporizador vencido).");
        }
    }
}

bool getRelayState() {
    return relayActivo;
}

bool getServoState() {
    return servoAbierto;
}
