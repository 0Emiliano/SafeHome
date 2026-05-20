# Pendientes para funcionalidad completa y rubrica

Esta lista resume lo que falta para que SafeHome quede 100% funcional y listo para entrega, tomando como base la rubrica del proyecto final de Sistemas Empotrados.

## ESP32 y hardware

- [ ] Confirmar cableado real en protoboard.
- [ ] AHT20 y BMP280 conectados por I2C: `SDA 21`, `SCL 22`.
- [ ] PIR conectado a `GPIO 13`.
- [ ] LDR conectado a `GPIO 34`.
- [ ] Relay conectado a `GPIO 26`.
- [ ] Servo conectado a `GPIO 27`.
- [ ] Probar lectura real de AHT20.
- [ ] Probar lectura real de BMP280.
- [ ] Probar lectura real de PIR.
- [ ] Probar lectura real de LDR.
- [ ] Probar activacion real del relay con `umbralLuz`.
- [ ] Probar apertura y cierre del servo con PIR y `tiempoServo`.
- [ ] Confirmar si el profesor exige estrictamente la biblioteca `NoDelay`.
- [ ] Si se exige `NoDelay`, migrar la temporizacion basada en `millis()` a `NoDelay`.
- [ ] Subir archivos LittleFS al ESP32:
  - `data/index.html`
  - `data/style.css`
- [ ] Compilar firmware.
- [ ] Cargar firmware al ESP32.
- [ ] Probar la interfaz web desde navegador.
- [ ] Probar `GET /api/status`.
- [ ] Probar `POST /api/config`.

## Parametros configurables

- [ ] Validar `umbralLuz` en hardware.
- [ ] Validar `umbralTemp` en hardware.
- [ ] Validar `tiempoServo` en hardware.
- [ ] Validar `intervaloSensado` en hardware.
- [ ] Confirmar que los parametros se puedan modificar desde la web del ESP32.
- [ ] Confirmar que los parametros se puedan modificar desde la GUI Python.
- [ ] Confirmar que los cambios de parametros afecten el comportamiento real del sistema.

## Subsistema PC y Python

- [ ] Instalar dependencias de `SEpython/requirements.txt`.
- [ ] Crear base de datos MySQL con `SEpython/sql/schema.sql`.
- [ ] Configurar variables de entorno o `.env`:
  - IP del ESP32.
  - Host MySQL.
  - Puerto MySQL.
  - Usuario MySQL.
  - Password MySQL.
  - Base de datos `safehome`.
  - Puerto TCP `9000`.
- [ ] Probar recolector TCP:
  - `python -m pc_app.collector.server`
- [ ] Probar envio TCP desde ESP32 hacia la PC.
- [ ] Confirmar que las lecturas lleguen al recolector.
- [ ] Confirmar que las lecturas se guarden en MySQL.
- [ ] Probar GUI:
  - `python -m pc_app.gui.app`
- [ ] Validar que la GUI muestre sensores.
- [ ] Validar que la GUI muestre actuadores.
- [ ] Validar que la GUI modifique parametros por API REST.
- [ ] Validar que la GUI muestre una representacion grafica del sistema fisico.
- [ ] Validar que la GUI genere graficas por periodo seleccionado.

## Base de datos

- [ ] Probar inserciones reales en `safehome_readings`.
- [ ] Revisar si se usara `config_history`.
- [ ] Si se usa `config_history`, implementar guardado de cambios de configuracion.
- [ ] Preparar diagrama entidad-relacion final.
- [ ] Preparar descripcion de tablas para el manual.

## Documentacion requerida

- [ ] Crear manual de usuario en PDF.
- [ ] Incluir modelo conceptual de SafeHome.
- [ ] Incluir esquematico del circuito en TinkerCad o Fritzing.
- [ ] Incluir diagrama de estados del ESP32.
- [ ] Incluir diagrama entidad-relacion.
- [ ] Incluir estructura y descripcion de tablas.
- [ ] Incluir codigo ESP32:
  - HTML y CSS.
  - API REST.
  - `setup()` y `loop()`.
- [ ] Incluir codigo Python:
  - Recolector TCP.
  - Aplicacion GUI.
- [ ] Incluir instrucciones detalladas de reconstruccion.
- [ ] Incluir instrucciones detalladas de ejecucion.
- [ ] Agregar evidencia o capturas de:
  - Web del ESP32.
  - GUI Python.
  - MySQL con datos.
  - Prototipo armado.

## Video demostrativo

- [ ] Explicar funcionamiento general.
- [ ] Mostrar circuito en protoboard.
- [ ] Mostrar arranque del recolector TCP.
- [ ] Mostrar arranque de la GUI.
- [ ] Mostrar compilacion y carga al ESP32.
- [ ] Mostrar web del ESP32.
- [ ] Cambiar parametros y demostrar efecto real.
- [ ] Mostrar datos guardandose en MySQL.
- [ ] Incluir participacion de todos los integrantes.

## Entrega final

- [ ] Confirmar nombre corto del proyecto: `SafeHome`.
- [ ] Confirmar matriculas de integrantes.
- [ ] Comprimir en `.rar` con el formato:
  - `proyFinalSE_SafeHome_IDs.rar`
- [ ] Verificar que el `.rar` incluya codigo ESP32.
- [ ] Verificar que el `.rar` incluya codigo Python.
- [ ] Verificar que el `.rar` incluya script SQL.
- [ ] Verificar que el `.rar` incluya manual PDF.
- [ ] Verificar que el `.rar` incluya archivos LittleFS.
- [ ] Verificar que no falten librerias o instrucciones de instalacion.

## Riesgos de cumplimiento

- SafeHome usa JavaScript embebido en la interfaz web. Si el profesor exige estrictamente solo HTML, CSS, C/C++ y Python, esta parte debe cambiarse.
- SafeHome usa `millis()` para temporizacion no bloqueante y no usa `delay()`. Si el profesor exige estrictamente la biblioteca `NoDelay`, se debe migrar esa temporizacion.
