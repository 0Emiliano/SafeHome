#include "sensors.h"
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

static Adafruit_AHTX0 aht;
static Adafruit_BMP280 bmp;

// Bandera: indica si cada sensor fue inicializado correctamente
static bool ahtOk = false;
static bool bmpOk = false;

void initSensors() {
    // Configurar pines de entrada
    pinMode(PIN_PIR, INPUT);
    pinMode(PIN_LDR, INPUT);

    // Iniciar bus I2C con los pines definidos en config.h
    Wire.begin(PIN_SDA, PIN_SCL);

    // Inicializar AHT20
    if (aht.begin()) {
        ahtOk = true;
        Serial.println("[Sensores] AHT20 conectado correctamente.");
    } else {
        Serial.println("[Sensores] ERROR: No se encontro el AHT20.");
    }

    // Inicializar BMP280: probar primero 0x76, luego 0x77
    if (bmp.begin(0x76)) {
        bmpOk = true;
        Serial.println("[Sensores] BMP280 conectado (0x76).");
    } else if (bmp.begin(0x77)) {
        bmpOk = true;
        Serial.println("[Sensores] BMP280 conectado (0x77).");
    } else {
        Serial.println("[Sensores] ERROR: No se encontro el BMP280.");
    }
}

SensorData readSensors() {
    SensorData data;

    // --- Leer AHT20 ---
    if (ahtOk) {
        sensors_event_t humEvt, tempEvt;
        aht.getEvent(&humEvt, &tempEvt);
        data.tempAHT = tempEvt.temperature;
        data.humedad = humEvt.relative_humidity;
    } else {
        data.tempAHT = 0.0f;
        data.humedad = 0.0f;
    }

    // --- Leer BMP280 ---
    if (bmpOk) {
        data.tempBMP = bmp.readTemperature();
        data.presion = bmp.readPressure() / 100.0f; // Convertir Pa a hPa
    } else {
        data.tempBMP = 0.0f;
        data.presion = 0.0f;
    }

    // --- Leer PIR y LDR ---
    data.pir = (digitalRead(PIN_PIR) == HIGH);
    data.ldr = analogRead(PIN_LDR);

    // Marca de tiempo basada en millis(); si hubiera NTP se sustituiria aqui
    data.timestamp = String(millis());

    return data;
}
