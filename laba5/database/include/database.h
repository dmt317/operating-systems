#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

#define DB_PATH "../database/temperature.db"
#define SQL_FILE "../database/scripts/queries.sql"

#define TEMPERATURE_TABLE "temperature"
#define AVG_TEMPERATURE_HOUR_TABLE "avg_temperature_hour"
#define AVG_TEMPERATURE_DAY_TABLE "avg_temperature_day"

void insert_db(sqlite3 *db, const char *table_name, char* timestamp, double temperature);

int init_database(void);

#endif // DATABASE_H
