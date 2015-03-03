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
#include<memory>
//#include <sqlite3.h>



class Library;
typedef unique_ptr<Library> Library_ptr;


class Library: public Database {
public:

  bool open_db();

  bool sql_exec_no_return(char * command);
  bool sql_exec(string command);


  void search_and_get_row(string table,string sword,string cstring,string& ID,string& file_ID,string& tags);

  void get_data_row(string& ID,string& file_ID,string& tags);
  void search_tags_and_get_rows(string table,string cstring,string& ID,string& file_ID,string& tags);

  int creat(void *NotUsed, int argc, char **argv, char **azColName);



  void close_db(){
      sqlite3_close(metadata_db);
  };
  bool prepared_done;
  bool prepared_reset;

  sqlite3_stmt* prepared;

  sqlite3 *metadata_db;
public:
  Library();
  ~Library();

  void upload_file(const string& filename);
};

#endif // LIBRARY_H
