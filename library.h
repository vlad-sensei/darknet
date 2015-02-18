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

#include <memory>

#include "glob.h"

class Library;
typedef unique_ptr<Library> Library_ptr;

class Library {
public:
  Library();

  void upload_file(const string& filename);

};

#endif // LIBRARY_H
