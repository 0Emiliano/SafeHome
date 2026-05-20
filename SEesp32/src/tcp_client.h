#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "config.h"

// Serializa los datos de sensores a JSON y los envia al servidor TCP
// configurado en TCP_SERVER_IP:TCP_SERVER_PORT.
// Si no hay conexion WiFi o el servidor no responde, imprime el error en Serial
// y retorna sin bloquear.
void sendDataTCP(const SensorData& data);

#endif // TCP_CLIENT_H
