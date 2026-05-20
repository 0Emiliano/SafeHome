# Contexto del proyecto, rubrica y producto generado

Este documento identifica el contexto del proyecto a partir del manual/rubrica de evaluacion proporcionado para el proyecto final de Sistemas Empotrados, y lo relaciona con el producto generado en esta carpeta.

## Contexto general del manual

El proyecto final solicita desarrollar un sistema embebido funcional basado en ESP32 y una aplicacion de PC. El sistema debe monitorear y controlar un prototipo fisico mediante sensores y actuadores, utilizando tecnologias web, API REST, base de datos, comunicacion TCP y programacion concurrente.

El proyecto debe dividirse en dos subsistemas:

- Subsistema ESP32.
- Subsistema PC.

El sistema debe incluir como minimo:

- 3 sensores.
- 2 actuadores.
- 2 parametros configurables.
- Servidor web en el ESP32.
- API REST en el ESP32.
- Sensado periodico sin `delay()`.
- Aplicacion Python con GUI.
- Recolector Python independiente.
- Base de datos MySQL.
- Comunicacion TCP entre ESP32 y PC.
- Documentacion tecnica y manual de usuario.

## Interpretacion del caso SafeHome

El proyecto generado se orienta al concepto **SafeHome**, un sistema de seguridad y monitoreo para hogar.

SafeHome permite:

- Medir temperatura y humedad ambiental.
- Medir presion barometrica.
- Detectar movimiento.
- Medir nivel de luz ambiental.
- Activar iluminacion mediante relay cuando la luz es baja.
- Abrir temporalmente un servo cuando se detecta movimiento.
- Configurar umbrales y tiempos desde una interfaz web o una GUI en PC.
- Enviar datos desde ESP32 a una PC por TCP.
- Guardar lecturas en MySQL para consulta y graficas.

## Requisitos del manual y correspondencia con SafeHome

| Requisito del manual/rubrica | Implementacion en SafeHome | Estado |
| --- | --- | --- |
| Plataforma principal ESP32 | Firmware en `SEesp32` | Base integrada |
| Lenguaje C/C++ en ESP32 | Codigo SafeHome original en Arduino C/C++ | Integrado |
| HTML y CSS en ESP32 | `SEesp32/data/index.html` y `SEesp32/data/style.css` | Integrado |
| Python en PC | `SEpython/pc_app` | Integrado |
| Minimo 3 sensores | AHT20, BMP280, PIR, LDR | Cumple |
| Minimo 2 actuadores | Relay y servo | Cumple |
| Parametros configurables | `umbralLuz`, `umbralTemp`, `tiempoServo`, `intervaloSensado` | Cumple |
| Servidor web ESP32 | `ESPAsyncWebServer` en `api.cpp` | Integrado |
| API REST ESP32 | `GET /api/status`, `POST /api/config` | Integrado |
| Archivos web en LittleFS | Carpeta `SEesp32/data` | Integrado |
| Sensado periodico | `intervaloSensado` con temporizacion no bloqueante | Integrado |
| No usar `delay()` | SafeHome usa `millis()` y `yield()` | Cumple parcialmente |
| Usar exclusivamente `NoDelay` | SafeHome no usa biblioteca `NoDelay` | Pendiente si el profesor lo exige |
| Conservar ultima lectura | `ultimaLectura` en firmware ESP32 | Cumple |
| Maquina de estados ESP32 | Estados `IDLE`, `SENSING`, `ALERTING`, `CONFIGURING` | Cumple |
| GUI Python | `SEpython/pc_app/gui/app.py` | Base generada |
| Recolector independiente | `SEpython/pc_app/collector/server.py` | Base generada |
| Comunicacion TCP | ESP32 envia JSON; PC escucha puerto `9000` | Base generada |
| MySQL | `SEpython/sql/schema.sql` | Base generada |
| Estadisticas con graficas | GUI con Matplotlib y selector de periodo | Base generada |
| Representacion grafica del sistema fisico | Canvas en GUI Python | Base generada |

## Producto generado

La carpeta del proyecto quedo organizada de la siguiente manera:

```text
SE/
  SEesp32/
    platformio.ini
    data/
      index.html
      style.css
    src/
      SafeHome.ino
      config.h
      sensors.h
      sensors.cpp
      actuators.h
      actuators.cpp
      api.h
      api.cpp
      tcp_client.h
      tcp_client.cpp
      WiFiManagerESP.h
      WiFiManagerESP.cpp
  SEpython/
    requirements.txt
    .env.example
    pc_app/
      collector/
      gui/
      models/
      services/
    sql/
      schema.sql
  docs/
    arquitectura.md
    checklist_entrega.md
    pendientes_funcionalidad_rubrica.md
    contexto_manual_rubrica_producto.md
```

## Producto ESP32 generado

El apartado ESP32 conserva el codigo original de SafeHome en forma literal. Los archivos se copiaron desde el proyecto funcional proporcionado y se verifico que fueran identicos.

El firmware realiza:

- Inicializacion de sensores.
- Inicializacion de actuadores.
- Conexion WiFi.
- Montaje de LittleFS.
- Servidor web.
- API REST.
- Lectura periodica de sensores.
- Evaluacion de umbrales.
- Control de relay.
- Control de servo.
- Envio TCP de lecturas.

### Sensores ESP32

- `AHT20`: temperatura y humedad.
- `BMP280`: temperatura y presion.
- `PIR`: deteccion de movimiento.
- `LDR`: luz ambiental.

### Actuadores ESP32

- `Relay`: se activa cuando el valor de luz cae por debajo de `umbralLuz`.
- `Servo`: se abre cuando el PIR detecta movimiento y se cierra despues de `tiempoServo`.

### API ESP32

`GET /api/status` devuelve sensores, actuadores y configuracion.

`POST /api/config` permite actualizar parametros configurables.

## Producto PC generado

El subsistema PC fue adaptado para trabajar con el protocolo de SafeHome.

Incluye:

- Recolector TCP independiente.
- Insercion de lecturas en MySQL.
- Cliente REST para consultar y modificar configuracion del ESP32.
- GUI Python con estado del sistema.
- Representacion grafica simple del prototipo.
- Graficas de sensores por periodo seleccionado.

## Base de datos generada

El script SQL crea la base `safehome` y la tabla principal `safehome_readings`.

La tabla guarda:

- Fecha y hora de recepcion en PC.
- Temperatura AHT20.
- Humedad.
- Temperatura BMP280.
- Presion.
- LDR.
- PIR.
- Timestamp enviado por el ESP32.

Tambien se incluye `config_history` como tabla prevista para historial de cambios de configuracion. Actualmente esta tabla existe en el script, pero todavia no se alimenta automaticamente desde la aplicacion.

## Riesgos identificados frente a la rubrica

### Uso de JavaScript en la interfaz web

El manual menciona HTML, CSS, C/C++ y Python como tecnologias. El firmware SafeHome funcional incluye JavaScript embebido dentro de `index.html` para consumir la API REST y actualizar la interfaz.

Si el profesor interpreta la restriccion como prohibicion total de JavaScript, se debe reemplazar esa parte por una solucion basada en HTML/CSS y respuestas generadas desde C++.

### Uso de `millis()` en lugar de `NoDelay`

SafeHome no usa `delay()` y su temporizacion es no bloqueante, pero utiliza `millis()` directamente. El manual pide utilizar exclusivamente la biblioteca `NoDelay`.

Si este punto se evalua de forma estricta, se debe migrar el temporizador de sensado y, posiblemente, el temporizador del servo a `NoDelay`.

### Validacion en hardware

Aunque el codigo base esta integrado, todavia falta demostrar funcionamiento completo con el prototipo fisico:

- Lecturas reales de sensores.
- Activacion real de actuadores.
- Comunicacion TCP real.
- Insercion real en MySQL.
- Uso real de GUI.

## Conclusion

El producto generado ya tiene una arquitectura alineada con el manual y esta adaptado al firmware SafeHome funcional proporcionado. La parte ESP32 conserva el codigo original del prototipo, y la parte PC fue ajustada para consumir sus datos, persistirlos y mostrarlos.

Para cerrar el proyecto como entrega final, faltan pruebas en hardware, evidencia documental, manual PDF, video demostrativo y resolver los dos posibles riesgos de rubrica: JavaScript en la web y uso de `millis()` en lugar de `NoDelay`.
