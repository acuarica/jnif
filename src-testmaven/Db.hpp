
#ifndef JNIF_DB_HPP
#define JNIF_DB_HPP

#include <iostream>
#include <sqlite3.h>

#include <jnif.hpp>

class DbException {
public:

  DbException(const std::string& message, const std::string&) :
    message(message)
  {
  }

  std::string message;

};

std::ostream& operator<<(std::ostream& os, const DbException& ex) {
	os << "Error: DB Exception: " << ex.message << " @" << std::endl;
	return os;
}

class DbError: public jnif::Error<DbException> {
};

typedef int (*Callback)(void*, int, char**, char**);

class Db {
  friend class Stmt;
public:
  Db(const char* dbName) {
    int rc = sqlite3_open(dbName, &_db);
    if (rc != SQLITE_OK) {
      sqlite3_close(_db);
      throw DbException(sqlite3_errmsg(_db), "<no stack trace>");
    }

    sqlite3_exec(_db, "PRAGMA journal_mode = OFF", NULL, NULL, NULL);
    sqlite3_exec(_db, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
    sqlite3_exec(_db, "PRAGMA locking_mode = EXCLUSIVE", NULL, NULL, NULL);
    sqlite3_exec(_db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL);
  }

  ~Db() {
    sqlite3_close(_db);
  }

  long lastInsertRowid() {
    return sqlite3_last_insert_rowid(_db);
  }

  void exec(const char* sql, Callback callback, void* args) {
    char *errMsg = NULL;
    int rc = sqlite3_exec(_db, sql, callback, args, &errMsg);

    if (rc != SQLITE_OK) {
      sqlite3_free(errMsg);
      throw DbException("error on exec", "");
    }
  }

  const char* errMsg() const {
    return sqlite3_errmsg(_db);
  }

private:
  sqlite3 *_db;
};

class Stmt {
public:

  Stmt(Db& db, const char* sql) : _db(db), _sql(sql) {
    int rc = sqlite3_prepare_v2(_db._db, _sql, -1, &_stmt, NULL);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "%d : %s", rc, sql);
      throw DbException("Preparing stmt:", "");
    }
  }

  ~Stmt() {
    sqlite3_finalize(_stmt);
  }

  void bindNull(int column) {
    sqlite3_bind_null(_stmt, column);
  }

  void bindText(int column, const char* text) {
    sqlite3_bind_text(_stmt, column, text, -1, SQLITE_STATIC);
  }

  void bindInt(int column, int value) {
    sqlite3_bind_int(_stmt, column, value);
  }

  void bindLong(int column, long value) {
    sqlite3_bind_int64(_stmt, column, value);
  }

  int step() {
    int rc = sqlite3_step(_stmt);
    return rc;
  }

  void reset() {
    sqlite3_reset(_stmt);
  }

  int getLong(int column) {
    return sqlite3_column_int64(_stmt, column);
  }

  void exec() {
    int rc = sqlite3_step(_stmt);
    if (rc != SQLITE_DONE) {
      DbError::raise("Error exec: ", rc, _db.errMsg(), _sql);
    }

    sqlite3_reset(_stmt);
    DbError::check(rc == SQLITE_DONE, "Error on exec/reset: ", rc, _sql);
  }

private:

  const Db& _db;
  const char* _sql;
  sqlite3_stmt* _stmt;
};
#endif
