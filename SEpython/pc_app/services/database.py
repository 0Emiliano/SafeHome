from datetime import datetime
from typing import Iterable

import mysql.connector

from pc_app.config import settings
from pc_app.models.reading import Reading


def connect():
    return mysql.connector.connect(
        host=settings.mysql_host,
        port=settings.mysql_port,
        user=settings.mysql_user,
        password=settings.mysql_password,
        database=settings.mysql_database,
    )


def insert_reading(reading: Reading) -> None:
    sql = """
        INSERT INTO safehome_readings
        (captured_at, temp_aht, humedad, temp_bmp, presion, ldr, pir, device_timestamp)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
    """
    values = (
        reading.captured_at,
        reading.temp_aht,
        reading.humedad,
        reading.temp_bmp,
        reading.presion,
        reading.ldr,
        reading.pir,
        reading.device_timestamp,
    )
    with connect() as conn:
        with conn.cursor() as cursor:
            cursor.execute(sql, values)
        conn.commit()


def fetch_readings(start: datetime, end: datetime) -> Iterable[Reading]:
    sql = """
        SELECT captured_at, temp_aht, humedad, temp_bmp, presion, ldr, pir, device_timestamp
        FROM safehome_readings
        WHERE captured_at BETWEEN %s AND %s
        ORDER BY captured_at ASC
    """
    with connect() as conn:
        with conn.cursor() as cursor:
            cursor.execute(sql, (start, end))
            for row in cursor.fetchall():
                yield Reading(*row)
