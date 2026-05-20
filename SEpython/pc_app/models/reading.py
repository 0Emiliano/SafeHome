from dataclasses import dataclass
from datetime import datetime


@dataclass
class Reading:
    captured_at: datetime
    temp_aht: float
    humedad: float
    temp_bmp: float
    presion: float
    ldr: int
    pir: bool
    device_timestamp: str
