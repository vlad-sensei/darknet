#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include "glob.h"

// ~~ Keccak
/* Hash function that uses SHA-3, implemented using libcrypto++.
 * The function allocates a 64 byte on the heap using unique_ptr
 * the result is moved into the return value.
*/
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

//.torrent
struct Metabody {
  Id bid; //hash of the body and next
  vector<Id> bids,chunks; //hash of bids and chunks
  bool append_from_chunk(const Chunk& chunk); //returns false once last chunk
  vector<Chunk> to_chunks();
};

//magnet link
struct Metahead {
  const Id bid, mid;
  const string tags;
  //Metahead(Metahead&& rhv): mid(move(rhv.mid)),bid(move(rhv.bid)),tags(move(rhv.tags)){}
  Metahead(const Id& bid_,const string& tags_):
      bid(bid_),mid(string((char*)&bid, sizeof(bid_))+tags_),  tags(tags_){}

private:
  Metahead();
};



#endif // COMMON_H
