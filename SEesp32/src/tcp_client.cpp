#include "tcp_client.h"
#include <WiFi.h>
#include <ArduinoJson.h>

void sendDataTCP(const SensorData& data) {
    // Verificar conectividad antes de intentar la conexion TCP
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[TCP] Sin conexion WiFi, omitiendo envio.");
        return;
    }

    WiFiClient client;

    if (!client.connect(TCP_SERVER_IP, TCP_SERVER_PORT)) {
        Serial.print("[TCP] No se pudo conectar a ");
        Serial.print(TCP_SERVER_IP);
        Serial.print(":");
        Serial.println(TCP_SERVER_PORT);
        return;
    }

    // Serializar datos de sensores a JSON
    JsonDocument doc;
    doc["tempAHT"]   = data.tempAHT;
    doc["humedad"]   = data.humedad;
    doc["tempBMP"]   = data.tempBMP;
    doc["presion"]   = data.presion;
    doc["ldr"]       = data.ldr;
    doc["pir"]       = data.pir;
    doc["timestamp"] = data.timestamp;

    String payload;
    serializeJson(doc, payload);
    payload += "\n"; // Delimitador de fin de mensaje

    client.print(payload);
    client.stop();

    Serial.print("[TCP] Datos enviados: ");
    Serial.println(payload);
}
