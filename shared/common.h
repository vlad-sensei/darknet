#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <deque>

#include "glob.h"

// ~~ Keccak
/* Hash function that uses SHA-3, implemented using libcrypto++.
 * The function allocates a 64 byte on the heap using unique_ptr
 * the result is moved into the return value.
*/

#define CHUNK_SIZE 524288
#define MAX_CIDS_PER_METABODY 2000
#define MAX_BIDS_PER_METABODY 30

hash512_t hash512(const string& value);

/* Prints a hash_t using the debug function.*/
//void debug_hash512(const hash_t&);

struct Chunk {
  string data;
  Id cid;
  Chunk(string& data_):data(move(data_)), cid(data){}
  inline bool verify(const Id& cid_)const { return cid_ ==cid;}
  inline size_t size(){return data.size();}
private:
  Chunk();
};

/*
 *  metabody chunk protocol
 * +--------------------+
 * | BID_COUNT [4]      |
 * +--------------------+
 * | CID_COUNT [4]      |
 * +--------------------+
 * | BID_1 [64]         |
 * +--------------------+
 *    ...
 * +--------------------+
 * | BID_BID_COUNT [64] |
 * +--------------------+
 * | CID_1 [64]         |
 * +--------------------+
 *    ...
 * +--------------------+
 * | CID_CID_COUNT [64] |
 * +--------------------+
 */
//.torrent
struct Metabody {
  Id bid; //hash of the content.
  vector<Id> bids,cids; //hash of bids and chunk IDs
  bool append_from_chunk(const Chunk& chunk); //returns false if trolling
  deque<Chunk> to_chunks();
private:
  size_t cids_offs=0; //offset for append chunk
};

//magnet link
struct Metahead {
  Id mid, bid;

  string tags;
  Metahead() = default;
  Metahead(const Id& bid_,const string& tags_):
     mid(string((char*)&bid_, sizeof(bid_))+tags_),  bid(bid_), tags(tags_){}

};



#endif // COMMON_H
