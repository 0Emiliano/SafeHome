#include "api.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "actuators.h"

static AsyncWebServer server(80);

// Punteros a los datos compartidos con el loop principal
static Config*        _config        = nullptr;
static SensorData*    _lastData      = nullptr;
static volatile bool* _configChanged = nullptr;

// Agrega encabezados CORS a una respuesta para permitir acceso desde cualquier origen
static void addCORS(AsyncWebServerResponse* res) {
    res->addHeader("Access-Control-Allow-Origin",  "*");
    res->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

void initAPI(Config* config, SensorData* lastData, volatile bool* configChanged) {
    _config        = config;
    _lastData      = lastData;
    _configChanged = configChanged;

    // --- Preflight CORS para todas las rutas ---
    server.on("/*", HTTP_OPTIONS, [](AsyncWebServerRequest* req) {
        AsyncWebServerResponse* res = req->beginResponse(204);
        addCORS(res);
        req->send(res);
    });

    // --- GET / -> pagina principal desde LittleFS ---
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!LittleFS.exists("/index.html")) {
            req->send(404, "text/plain", "index.html no encontrado en LittleFS.");
            return;
        }
        AsyncWebServerResponse* res =
            req->beginResponse(LittleFS, "/index.html", "text/html");
        addCORS(res);
        req->send(res);
    });

    // Archivos estaticos (style.css, etc.) desde LittleFS
    server.serveStatic("/", LittleFS, "/");

    // --- GET /api/status -> JSON con el estado completo del sistema ---
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;

        // Datos de los sensores
        JsonObject sensores = doc["sensores"].to<JsonObject>();
        sensores["tempAHT"]   = _lastData->tempAHT;
        sensores["humedad"]   = _lastData->humedad;
        sensores["tempBMP"]   = _lastData->tempBMP;
        sensores["presion"]   = _lastData->presion;
        sensores["ldr"]       = _lastData->ldr;
        sensores["pir"]       = _lastData->pir;
        sensores["timestamp"] = _lastData->timestamp;

        // Estado actual de los actuadores
        JsonObject actuadores = doc["actuadores"].to<JsonObject>();
        actuadores["relay"] = getRelayState();
        actuadores["servo"] = getServoState();

        // Configuracion vigente
        JsonObject cfg = doc["config"].to<JsonObject>();
        cfg["umbralLuz"]        = _config->umbralLuz;
        cfg["umbralTemp"]       = _config->umbralTemp;
        cfg["tiempoServo"]      = _config->tiempoServo;
        cfg["intervaloSensado"] = _config->intervaloSensado;

        String body;
        serializeJson(doc, body);

        AsyncWebServerResponse* res =
            req->beginResponse(200, "application/json", body);
        addCORS(res);
        req->send(res);
    });

    // --- POST /api/config -> actualiza parametros en memoria ---
    // El handler de body usa la forma lambda de tres argumentos de ESPAsyncWebServer
    server.on(
        "/api/config",
        HTTP_POST,
        [](AsyncWebServerRequest* req) {},   // onRequest (vacio, se procesa en onBody)
        nullptr,                             // onUpload
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len,
           size_t index, size_t total) {

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, data, len);

            if (err) {
                AsyncWebServerResponse* res =
                    req->beginResponse(400, "application/json",
                                       "{\"error\":\"JSON invalido\"}");
                addCORS(res);
                req->send(res);
                return;
            }

            // Actualizar solo los campos que llegaron en el JSON
            JsonVariant v;

            v = doc["umbralLuz"];
            if (!v.isNull()) _config->umbralLuz = v.as<int>();

            v = doc["umbralTemp"];
            if (!v.isNull()) _config->umbralTemp = v.as<float>();

            v = doc["tiempoServo"];
            if (!v.isNull()) _config->tiempoServo = v.as<int>();

            v = doc["intervaloSensado"];
            if (!v.isNull()) _config->intervaloSensado = v.as<int>();

            // Notificar al loop principal que debe pasar a estado CONFIGURING
            *_configChanged = true;

            AsyncWebServerResponse* res =
                req->beginResponse(200, "application/json", "{\"ok\":true}");
            addCORS(res);
            req->send(res);
        }
    );

    server.begin();
    Serial.println("[API] Servidor web iniciado en puerto 80.");
}
