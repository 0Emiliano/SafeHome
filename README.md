# SafeHome

Base integrada del proyecto final de Sistemas Empotrados. Esta version toma como fuente funcional el firmware `SafeHome` provisto para las conexiones en protoboard.

## Concepto

SafeHome es un sistema de seguridad y monitoreo para hogar basado en ESP32. Mide condiciones ambientales y de presencia, activa actuadores y envia datos a una aplicacion de PC.

## Hardware definido

- ESP32 como plataforma principal.
- AHT20 por I2C: temperatura y humedad.
- BMP280 por I2C: temperatura y presion barometrica.
- PIR en GPIO 13: deteccion de movimiento.
- LDR en GPIO 34: nivel de luz por ADC.
- Relay en GPIO 26: control de iluminacion.
- Servo en GPIO 27: control de acceso.
- Bus I2C: SDA 21, SCL 22.

## Parametros configurables

- `umbralLuz`: valor ADC bajo el cual se activa el relay.
- `umbralTemp`: temperatura en C que genera alerta.
- `tiempoServo`: tiempo en ms que el servo permanece abierto.
- `intervaloSensado`: periodo en ms entre lecturas.

## Estructura

```text
SE/
  SEesp32/
    data/             Interfaz web SafeHome para LittleFS
    src/              Firmware modular ESP32 original de SafeHome
  SEpython/
    pc_app/
      collector/      Servidor TCP independiente
      gui/            Aplicacion de usuario
      models/         Entidades compartidas
      services/       API REST y base de datos
    sql/              Script MySQL
  docs/               Documentacion del proyecto
```

## Flujo

1. El ESP32 lee AHT20, BMP280, PIR y LDR.
2. La maquina de estados pasa por `IDLE`, `SENSING`, `ALERTING` y `CONFIGURING`.
3. Los actuadores se actualizan segun los parametros configurables.
4. La interfaz web se sirve desde LittleFS y consulta `/api/status`.
5. La PC recibe lecturas por TCP en el puerto 9000 y las guarda en MySQL.
6. La GUI Python consulta el ESP32 por REST y grafica datos historicos desde MySQL.

## Endpoints ESP32

- `GET /`: interfaz web.
- `GET /api/status`: sensores, actuadores y configuracion actual.
- `POST /api/config`: actualiza `umbralLuz`, `umbralTemp`, `tiempoServo` e `intervaloSensado`.

## Programas Python

- Recolector TCP:
  `python -m pc_app.collector.server`
- GUI:
  `python -m pc_app.gui.app`

Ejecutar los comandos desde la carpeta `SEpython` y cargar primero el script SQL en MySQL.

## Observaciones de cumplimiento

- El apartado ESP32 conserva el codigo SafeHome original de forma literal; `platformio.ini` solo agrega el contenedor de compilacion y dependencias.
- El firmware SafeHome funcional usa temporizacion con `millis()` y no usa `delay()`.
- El enunciado escolar pide exclusivamente la biblioteca `NoDelay`; queda pendiente decidir si se conserva SafeHome intacto o se migra el temporizador de sensado a `NoDelay`.
- La interfaz web funcional de SafeHome incluye JavaScript embebido para consumir la API REST. Si el profesor exige estrictamente solo HTML/CSS/C/Python, esta parte debe cambiarse a una vista generada desde C++ o formularios HTML.
