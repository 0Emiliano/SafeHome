#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- Pines del hardware ---
#define PIN_PIR   13   // PIR digital INPUT
#define PIN_LDR   34   // LDR analogico ADC1
#define PIN_RELE  26   // Relay OUTPUT
#define PIN_SERVO 27   // Servo PWM

// --- Pines bus I2C ---
#define PIN_SDA 21
#define PIN_SCL 22

// --- Servidor TCP al que se envian los datos ---
#define TCP_SERVER_IP   "192.168.1.96"
#define TCP_SERVER_PORT 9000

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* WIFI_USER;
extern const char* WIFI_PASS;

// --- Valores por defecto de los parametros configurables ---
#define DEFAULT_UMBRAL_LUZ        60    // ADC por debajo del cual se activa el relay
#define DEFAULT_UMBRAL_TEMP       30.0f   // Temperatura en C que activa una alerta
#define DEFAULT_TIEMPO_SERVO      3000    // Ms que el servo permanece abierto
#define DEFAULT_INTERVALO_SENSADO 2000    // Ms entre ciclos de lectura

// ---------------------------------------------------------------------------
// Estructura de configuracion dinamica
// Puede modificarse en tiempo de ejecucion via POST /api/config
// ---------------------------------------------------------------------------
struct Config {
    int   umbralLuz;        // Valor ADC bajo el cual la luz se considera escasa
    float umbralTemp;       // Temperatura en C que genera una alerta
    int   tiempoServo;      // Duracion en ms del servo en posicion abierta (90 grados)
    int   intervaloSensado; // Periodo en ms entre cada ciclo de lectura de sensores
};

// ---------------------------------------------------------------------------
// Estructura con todos los datos leidos en un ciclo de sensado
// Compartida entre sensores, actuadores y la API
// ---------------------------------------------------------------------------
struct SensorData {
    float  tempAHT;    // Temperatura del AHT20 en C
    float  humedad;    // Humedad relativa del AHT20 en %
    float  tempBMP;    // Temperatura del BMP280 en C
    float  presion;    // Presion barometrica en hPa
    int    ldr;        // Valor ADC del fotoresistor (0-4095)
    bool   pir;        // true si hay movimiento detectado
    String timestamp;  // Marca de tiempo: millis() como String
};

#endif // CONFIG_H
