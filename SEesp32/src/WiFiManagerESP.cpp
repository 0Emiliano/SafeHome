#include "WiFiManagerESP.h"

WiFiManagerESP::WiFiManagerESP(const char* ssid, const char* password,
                                const char* user, const char* pass)
    : _ssid(ssid), _password(password), _user(user), _pass(pass) {}

void WiFiManagerESP::connectToWiFi() {
    Serial.print("[WiFi] Conectando a: ");
    Serial.println(_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    // Esperar hasta conectar sin usar delay(): usar millis() y yield()
    unsigned long inicio      = millis();
    unsigned long ultimoPunto = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
        if (millis() - ultimoPunto >= 500) {
            Serial.print(".");
            ultimoPunto = millis();
        }
        yield(); // Permite al scheduler del ESP32 atender otras tareas
    }

    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] ERROR: No se pudo conectar.");
        return;
    }

    Serial.print("[WiFi] Conectado. IP asignada: ");
    Serial.println(WiFi.localIP());

    // Si no hay acceso a internet, asumir portal cautivo y autenticar
    if (!checkInternetAccess()) {
        Serial.println("[WiFi] Portal cautivo detectado. Autenticando...");
        if (authenticateCaptivePortal()) {
            Serial.println("[WiFi] Autenticacion en portal cautivo exitosa.");
        } else {
            Serial.println("[WiFi] ERROR: Fallo la autenticacion en el portal cautivo.");
        }
    } else {
        Serial.println("[WiFi] Acceso a internet disponible.");
    }
}

bool WiFiManagerESP::checkInternetAccess() {
    HTTPClient http;
    // generate_204 responde exactamente HTTP 204 cuando hay acceso real a internet
    http.begin("http://clients3.google.com/generate_204");
    int code = http.GET();
    http.end();
    return (code == 204);
}

bool WiFiManagerESP::authenticateCaptivePortal() {
    WiFiClientSecure tls;
    // El portal ITSON usa un certificado autofirmado; omitir la verificacion
    tls.setInsecure();

    HTTPClient http;
    http.begin(tls, "https://captiveportalnav.itson.edu.mx:8090/login.xml");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Construir el cuerpo del POST con las credenciales del portal cautivo
    // El campo 'a' es una marca de tiempo en ms que el portal requiere
    String body  = "mode=191";
    body        += "&username=" + String(_user);
    body        += "&password=" + String(_pass);
    body        += "&a="        + String(millis());
    body        += "&producttype=0";

    int code = http.POST(body);
    Serial.print("[WiFi] Portal cautivo respondio con codigo HTTP: ");
    Serial.println(code);
    http.end();

    return (code == 200 || code == 204);
}
