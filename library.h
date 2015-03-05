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
  unique_ptr<Database> db;

public:
  Library();
  ~Library();

  void upload_file(const string& filepath,const string& Tags);
  void search(const string& Tag);
};

#endif // LIBRARY_H
