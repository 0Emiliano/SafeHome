#ifndef WIFI_MANAGER_ESP_H
#define WIFI_MANAGER_ESP_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Gestiona la conexion a la red WiFi ITSON y la autenticacion
// en el portal cautivo de la universidad.
class WiFiManagerESP {
public:
    // Constructor: recibe credenciales de red y del portal cautivo.
    //   ssid     - nombre de la red WiFi (ej. "ITSON")
    //   password - contrasena del WiFi (puede ser vacia)
    //   user     - matricula para el portal cautivo
    //   pass     - contrasena para el portal cautivo
    WiFiManagerESP(const char* ssid, const char* password,
                   const char* user,  const char* pass);

    // Conecta a la red WiFi y, si se detecta portal cautivo,
    // autentica automaticamente con las credenciales proporcionadas.
    void connectToWiFi();

    // Verifica acceso real a internet mediante GET a generate_204.
    // Retorna true si la respuesta es HTTP 204.
    bool checkInternetAccess();

    // Realiza la autenticacion en el portal cautivo ITSON via POST HTTPS.
    // Usa tls.setInsecure() ya que el certificado del portal es autofirmado.
    // Retorna true si la autenticacion fue exitosa.
    bool authenticateCaptivePortal();

private:
    const char* _ssid;
    const char* _password;
    const char* _user;
    const char* _pass;
};

#endif // WIFI_MANAGER_ESP_H
