CREATE TABLE IF NOT EXISTS temperature (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME,
    temperature REAL
);

CREATE TABLE IF NOT EXISTS avg_temperature_hour (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME,
    temperature REAL
);

CREATE TABLE IF NOT EXISTS avg_temperature_day (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME,
    temperature REAL
);