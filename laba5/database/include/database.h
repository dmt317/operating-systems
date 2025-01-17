#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

void delete_oldest_record(sqlite3 *db, const char *table_name);

void insert_db(sqlite3 *db, const char *table_name, char* timestamp, double temperature);

int init_database(void);

#endif // DATABASE_H
