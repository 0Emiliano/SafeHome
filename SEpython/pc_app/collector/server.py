from datetime import datetime
import json
import socketserver

from pc_app.config import settings
from pc_app.models.reading import Reading
from pc_app.services.database import insert_reading


class ReadingHandler(socketserver.StreamRequestHandler):
    def handle(self) -> None:
        for raw_line in self.rfile:
            payload = json.loads(raw_line.decode("utf-8"))
            reading = Reading(
                captured_at=datetime.now(),
                temp_aht=float(payload["tempAHT"]),
                humedad=float(payload["humedad"]),
                temp_bmp=float(payload["tempBMP"]),
                presion=float(payload["presion"]),
                ldr=int(payload["ldr"]),
                pir=bool(payload["pir"]),
                device_timestamp=str(payload["timestamp"]),
            )
            insert_reading(reading)


def main() -> None:
    with socketserver.ThreadingTCPServer((settings.tcp_host, settings.tcp_port), ReadingHandler) as server:
        print(f"Servidor TCP escuchando en {settings.tcp_host}:{settings.tcp_port}")
        server.serve_forever()


if __name__ == "__main__":
    main()
