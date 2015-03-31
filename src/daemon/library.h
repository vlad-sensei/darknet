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
  void upload_file(const string& filename, const string& tags = "");

  bool req_file(const Id& mid);
  vector<Metahead> publish_metaheads(){return published_metaheads;}

  void handle_chunk(const Id& bid, const Chunk& chunk);


private:
  virtual void req_chunks(const Id& bid, const unordered_set<Id>& cids) = 0; //request chunks
  bool get_metahead(const Id& mid, Metahead& Metahead);

  struct {
    unordered_map<Id, unordered_set<Id> > chunk_reqs; // chunk_req_map[mid] == set of chunks we are waiting for for that file
    unordered_set<Id> has_metabody_; //rename?

    inline bool file_req_exists(const Id& bid){ return chunk_reqs.find(bid) != chunk_reqs.end();}
    inline bool chunk_req_exists(const Id& bid,const Id& cid){ return (file_req_exists(bid) && chunk_reqs[bid].find(cid) != chunk_reqs[bid].end() );}
    inline bool has_metabody(const Id& bid){return has_metabody_.find(bid) !=has_metabody_.end();}
  } data;

  Metahead m = Metahead(Id("a"), "Batman");
  Metahead m1 = Metahead(Id("b"), "Spiderman");
  Metahead m2 = Metahead(Id("c"), "Pop eye");
  vector<Metahead> published_metaheads = { m, m1, m2 };
  rw_mutex chunk_reqs_mtx;

};

#endif // LIBRARY_H
