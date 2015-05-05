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

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "glob.h"
#include "common.h"
#include "inventory.h"


class Library: public Inventory {
public:

  Library();
  //pattern ex Batman%movie%...
  //TODO: right now you need to search the tags in the same order in which it was written.
  void search(const string& pattern, vector<Id>& mids);
  bool upload_file(const string& file_path, const string& tags, Id& mid);
  inline void add_metahead(const Metahead & metahead) {Database::add_metahead(metahead);}
  bool get_metahead(const Id& mid, Metahead& metahead);
  vector<Metahead> publish_metaheads();

private:
  virtual void req_chunks(const Id& bid, const unordered_set<Id>& cids) = 0; //request chunks


  struct {
    unordered_set<Id> avaible_metabodies; //rename?
  } data;

  rw_mutex avaible_metabody_mutex;
};

#endif // LIBRARY_H
