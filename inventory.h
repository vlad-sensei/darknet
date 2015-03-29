#ifndef INVENTORY_H
#define INVENTORY_H

#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "glob.h"
#include "common.h"
#include "database.h"

class Inventory: protected Database {
public:
  Inventory(){}

  /*
   * to be used by a search function
   * create a metabody,
   * create temp file from chunks, verifying each one
   * if successful move to dest_path ant return true
   */
  bool get_file(const Id& bid, const string& dest_path);

protected:

  /*
   * interate through the file adding chunks sequentially to arena
   * and building metabody in progress. Then add metabody to arena.
   * finally compute and return metahead
   */
  void upload_file(const string& filename, Metahead& Metahead);

  bool get_metabody(const Id& bid, Metabody& metabody);
  bool add_metabody(const Metabody& metabody);

  //write chunk to file, chunk_map and database
  //TODO: (optimization) queue ~1000 cids in memory before putting them into database
  bool get_chunk(const Id& bid, const Id& cid, Chunk &chunk);
  void add_chunk(const Id& bid, const Chunk& chunk);  //take size into account

private:

  struct {
    unordered_map<Id, unordered_map<Id, size_t> > chunk_map; //inmemory map[bid][cid] == chunk_offs
    unordered_set<size_t> free_slots;
    fstream arena;
  } data;
  rw_mutex chunk_map_mtx, arena_mtx;
};

#endif // INVENTORY_H
