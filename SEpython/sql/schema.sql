CREATE DATABASE IF NOT EXISTS safehome;
USE safehome;

CREATE TABLE IF NOT EXISTS safehome_readings (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  captured_at DATETIME NOT NULL,
  temp_aht DOUBLE NOT NULL,
  humedad DOUBLE NOT NULL,
  temp_bmp DOUBLE NOT NULL,
  presion DOUBLE NOT NULL,
  ldr INT NOT NULL,
  pir BOOLEAN NOT NULL,
  device_timestamp VARCHAR(32) NOT NULL,
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (id),
  INDEX idx_safehome_readings_captured_at (captured_at)
);

CREATE TABLE IF NOT EXISTS config_history (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  umbral_luz INT NOT NULL,
  umbral_temp DOUBLE NOT NULL,
  tiempo_servo INT NOT NULL,
  intervalo_sensado INT NOT NULL,
  changed_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (id)
);
