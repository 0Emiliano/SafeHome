from dataclasses import dataclass
import os


@dataclass(frozen=True)
class Settings:
    esp32_base_url: str = os.getenv("ESP32_BASE_URL", "http://192.168.1.50")
    tcp_host: str = os.getenv("TCP_HOST", "0.0.0.0")
    tcp_port: int = int(os.getenv("TCP_PORT", "9000"))
    mysql_host: str = os.getenv("MYSQL_HOST", "127.0.0.1")
    mysql_port: int = int(os.getenv("MYSQL_PORT", "3306"))
    mysql_user: str = os.getenv("MYSQL_USER", "root")
    mysql_password: str = os.getenv("MYSQL_PASSWORD", "")
    mysql_database: str = os.getenv("MYSQL_DATABASE", "safehome")


settings = Settings()
