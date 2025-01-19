#include "database.h"

char *read_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);

  char *content = (char *)malloc(length + 1);
  if (!content) {
    fprintf(stderr, "Error allocating memory for a file: %s\n", filename);
    fclose(file);
    return NULL;
  }

  fread(content, 1, length, file);
  content[length] = '\0';
  fclose(file);
  return content;
}

void insert_db(sqlite3 *db, const char *table_name, char* timestamp, double temperature) {
  char sql[128];
  snprintf(sql, sizeof(sql), "INSERT INTO %s (timestamp, temperature) VALUES (?, ?);", table_name);
  sqlite3_stmt *stmt;

  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Query preparation error: %s\n", sqlite3_errmsg(db));
    return;
  }

  rc = sqlite3_bind_text(stmt, 1, timestamp, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Timestamp binding error: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return;
  }

  rc = sqlite3_bind_double(stmt, 2, temperature);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Temperature binding error: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return;
  }

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Query execution error: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return;
  }

  printf("Temperature %.2f with timestamp '%s' was inserting into table '%s'.\n", temperature, timestamp, table_name);

  sqlite3_finalize(stmt);
}

sqlite3 *init_database() {
  sqlite3 *db;
  char *err_msg = 0;

  int rc = sqlite3_open(DB_PATH, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  char *sql = read_file(SQL_FILE);
  if (!sql) {
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  free(sql);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Queries execution error %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "Database initialized and tables created successfully.\n");
  return db;
}