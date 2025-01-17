import sqlite3
import random
from datetime import datetime, timedelta

DB_PATH = "../database/temperature.db"

hourly_start_date = datetime(2024, 12, 15)
hourly_end_date = datetime(2025, 1, 15)

daily_start_date = datetime(2024, 1, 15)
daily_end_date = datetime(2025, 1, 15)

def setup_database():
    connection = sqlite3.connect(DB_PATH)
    cursor = connection.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS avg_temperature_hour (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME,
            temperature REAL
        );
    """)

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS avg_temperature_day (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME,
            temperature REAL
        );
    """)

    connection.commit()
    connection.close()

def generate_random_temperature(min_temp=-20, max_temp=32):
    return round(random.uniform(min_temp, max_temp), 2)

def generate_smooth_temperature(prev_temp, min_change=-1.0, max_change=1.0, min_temp=-20, max_temp=32):
    new_temp = prev_temp + random.uniform(min_change, max_change)
    return round(max(min(new_temp, max_temp), min_temp), 2)

def fill_avg_temperature_hour():
    connection = sqlite3.connect(DB_PATH)
    cursor = connection.cursor()

    current_date = hourly_start_date
    previous_temp = generate_random_temperature()

    while current_date <= hourly_end_date:
        average_temp = generate_smooth_temperature(previous_temp)
        cursor.execute("""
            INSERT INTO avg_temperature_hour (timestamp, temperature)
            VALUES (?, ?);
        """, (current_date.strftime("%Y-%m-%d %H:%M:%S"), average_temp))
        previous_temp = average_temp
        current_date += timedelta(hours=1)

    connection.commit()
    connection.close()

def fill_avg_temperature_day():
    connection = sqlite3.connect(DB_PATH)
    cursor = connection.cursor()

    current_date = daily_start_date
    previous_temp = generate_random_temperature()

    while current_date <= daily_end_date:
        average_temp = generate_smooth_temperature(previous_temp, min_change=-2.0, max_change=2.0)
        cursor.execute("""
            INSERT INTO avg_temperature_day (timestamp, temperature)
            VALUES (?, ?);
        """, (current_date.strftime("%Y-%m-%d"), average_temp))
        previous_temp = average_temp
        current_date += timedelta(days=1)

    connection.commit()
    connection.close()

setup_database()
fill_avg_temperature_hour()
fill_avg_temperature_day()

print(f"Данные успешно сгенерированы и записаны в {DB_PATH}.")
