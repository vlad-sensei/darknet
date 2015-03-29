#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include "glob.h"
#include <stdint.h>

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
  Id mid; //64 byte
  Id bid; //64 byte
  uint32_t file_size; //Not size_t becuase it should work on both raspberry pi and x86
  string tags; //
};

//the type of the serialized metahead
const int MID_WIDTH = sizeof(Id);
const int BID_WIDTH = sizeof(Id);
const int FILE_WIDTH = sizeof(uint32_t);
const int TAGS_WIDTH = 892;
const int METAHEAD_SIZE = MID_WIDTH + BID_WIDTH + FILE_WIDTH + TAGS_WIDTH;
const int N_METAHEADS = 5;
typedef array<byte, METAHEAD_SIZE> metahead_ser_t;

metahead_ser_t serialize_metahead(const Metahead& metahead);
Metahead deserialize_metahead(const metahead_ser_t& meta_ser);

#endif // COMMON_H
