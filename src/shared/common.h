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

const size_t CHUNK_SIZE = 1 << 19;


#define MAX_CIDS_PER_METABODY 2000
#define MAX_BIDS_PER_METABODY 30

hash512_t hash512(const string& value);

/* Prints a hash_t using the debug function.*/
//void debug_hash512(const hash_t&);

//TODO: make cid/data private
struct Chunk {
  string data;
  Id cid;
  Chunk(string& data_):data(move(data_)), cid(data){}
  bool set_data(string& data_);
  inline bool verify(const Id& cid_) const { return cid_ ==cid;}
  inline size_t size() const {return data.size();}
  Chunk() = default;
private:

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
  Metabody(const Id& bid_):bid(bid_){}
  Metabody() = default;
  Id bid_next(){
    debug("*** give the next bid to add for the metabody finishid");
    return bid;
  }
  //void update_Id(){bid=Id(string((char*)this,sizeof(Metabody)));}
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

/* List contains (enum name,full command name, short command name)
I thought that it's not neccessary with more than two names per command
That is why I have explicitly specified a short and full name.
*/
#define CMD_LIST \
  HANDLE_CMD(CMD_DOWNLOAD,1,"download","do") \
  HANDLE_CMD(CMD_UPLOAD,2,"upload","up") \
  HANDLE_CMD(CMD_CONNECT,3,"connect","co") \
  HANDLE_CMD(CMD_BROADCAST,4,"broadcast","br") \
  HANDLE_CMD(CMD_EXIT,5,"exit", "quit") \
  HANDLE_CMD(CMD_ASSEMBLE,6,"assemble", "as") \
  HANDLE_CMD(CMD_SYNCH,7,"synch", "sy") \
  HANDLE_CMD(CMD_SEARCH,8,"search", "se")




#endif // COMMON_H
