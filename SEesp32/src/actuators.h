#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "config.h"

// Inicializa relay y servo en estado seguro (relay LOW, servo 0 grados).
// Debe llamarse una sola vez en setup().
void initActuators();

// Evalua los datos de sensores contra los umbrales de config
// y activa o desactiva relay y servo segun corresponda.
void updateActuators(const SensorData& data, const Config& config);

// Maneja el temporizador del servo sin usar delay().
// Debe llamarse en cada iteracion del loop principal.
void tickActuators(const Config& config);

// Retorna true si el relay esta actualmente activado.
bool getRelayState();

// Retorna true si el servo esta actualmente en posicion abierta (90 grados).
bool getServoState();

#endif // ACTUATORS_H
