#ifndef LIBRARY_H
#define LIBRARY_H

/*
 * Libary
 *
 * Library represents the programs file data. It is
 * responsible for keeping file data and metadata
 * up to date, operating at SQLite backend.
 * It provides API for seach, upload and download
 * queries.
 */

#include "glob.h"
#include "database.h"

class Library: public Database {
public:
=======
#include <sqlite3.h>




class Library;
typedef unique_ptr<Library> Library_ptr;

class Library {

  bool open_db();
  bool sql_exec(string command,int (*callback)(void *NotUsed, int argc, char **argv, char **azColName));
  int creat(void *NotUsed, int argc, char **argv, char **azColName);
  void creat_table(string command, string name);

  sqlite3 *metadata_db;
  void close_db(){
      sqlite3_close(metadata_db);
  };

public:
  Library();
  ~Library();

  void upload_file(const string& filename);
};

#endif // LIBRARY_H
