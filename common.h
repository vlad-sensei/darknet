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
 *  body chunk protocol
 *  size(int): bid_count N
 *  size(int): cid_count M
 *  size(Id): bid_1
 *  .
 *  .
 *  .
 *  size(Id): bid_N
 *  size(Id): cid_1
 *  .
 *  .
 *  .
 *  size(Id): cid_M
 *
 *
 */
//.torrent
struct Metabody {
  Id bid; //hash of the content.
  vector<Id> bids,cids; //hash of bids and chunk IDs
  bool append_from_chunk(const Chunk& chunk); //returns false once last chunk
  deque<Chunk> create_body_chunks();
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
