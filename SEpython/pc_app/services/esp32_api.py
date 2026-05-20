import requests

from pc_app.config import settings


def get_state() -> dict:
    response = requests.get(f"{settings.esp32_base_url}/api/status", timeout=5)
    response.raise_for_status()
    return response.json()


def update_config(
    umbral_luz: int,
    umbral_temp: float,
    tiempo_servo: int,
    intervalo_sensado: int,
) -> dict:
    response = requests.post(
        f"{settings.esp32_base_url}/api/config",
        json={
            "umbralLuz": umbral_luz,
            "umbralTemp": umbral_temp,
            "tiempoServo": tiempo_servo,
            "intervaloSensado": intervalo_sensado,
        },
        timeout=5,
    )
    response.raise_for_status()
    return response.json()
