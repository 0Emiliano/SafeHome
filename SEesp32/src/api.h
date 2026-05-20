#ifndef API_H
#define API_H

#include "config.h"

// Inicializa el servidor web asincronico (puerto 80) con las rutas:
//   GET  /            -> sirve index.html desde LittleFS
//   GET  /api/status  -> JSON con sensores, actuadores y configuracion actual
//   POST /api/config  -> actualiza uno o mas parametros de configuracion
//
// Parametros:
//   config        - puntero a la configuracion global (lectura y escritura)
//   lastData      - puntero al ultimo dato de sensores leido (solo lectura)
//   configChanged - bandera que se activa cuando la configuracion es modificada
void initAPI(Config* config, SensorData* lastData, volatile bool* configChanged);

#endif // API_H
