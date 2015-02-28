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
  void upload_file(const string& filename);
};

#endif // LIBRARY_H
