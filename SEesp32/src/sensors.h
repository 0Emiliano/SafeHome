#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"

// Inicializa AHT20, BMP280 y configura los pines PIR y LDR.
// Debe llamarse una sola vez en setup().
void initSensors();

// Lee todos los sensores en un solo ciclo y retorna un struct SensorData.
// No usa delay(); es seguro llamarla en cada iteracion del loop.
SensorData readSensors();

#endif // SENSORS_H
