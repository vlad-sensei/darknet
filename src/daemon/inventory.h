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
  Inventory();

  /*
   * to be used by a search function
   * create a metabody,
   * create temp file from chunks, verifying each one
   * if successful move to dest_path ant return true
   */

  bool get_file(const Id& bid, const string& dest_path);

  //TODO: (optimization) queue ~1000 cids in memory before putting them into database
  bool get_chunk(const Id& bid, const Id& cid, Chunk &chunk);
protected:

  /*
   * interate through the file adding chunks sequentially to arena
   * and building metabody in progress. Then add metabody to arena.
   * finally compute and return metahead
   */
  bool upload_file(const string& filename, Metahead& metahead,const string tags);

  bool get_metabody(const Id& bid, Metabody& metabody){
    //TODO: do this right
    debug("*** get_metabody more then one");
    Chunk chunk;
    //TODO: get more then one chunk to metabody
    metabody.bid=bid;

    if (!get_chunk(bid,bid,chunk)) {
      debug("*** get_metabody no chunk");
      return false;
    }
    metabody.append_from_chunk(chunk);
    return true;
  }
  bool add_metabody(const Metabody& metabody);

  //write chunk to file, chunk_map and database
  bool add_chunk(const Id& bid, const Chunk& chunk);  //take size into account

private:
  bool add_new_arena_slots(const size_t& num = 1);
  bool write_to_arena_slot(const string& data, size_t& idx);
  bool read_from_arena_slot(const size_t& idx, const size_t& chunk_size, Chunk& chunk);

private:
  struct {
    unordered_set<size_t> free_slots;
    size_t arena_slots_size = 0;
    ofstream arena; //for writing only
  } data;
  rw_mutex arena_mtx;
};

#endif // INVENTORY_H
