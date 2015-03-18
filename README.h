# darknet

#if __cplusplus < 201300L
// pre c++14
#include <boost/thread.hpp>
typedef boost::shared_mutex rw_mutex;
typedef boost::shared_lock<boost::shared_mutex> r_lock;
typedef boost::unique_lock<boost::shared_mutex> w_lock;
#else
// c++14
#include <shared_mutex>
typedef std::shared_timed_mutex rw_mutex;
typedef std::shared_lock<std::shared_timed_mutex> r_lock;
typedef std::unique_lock<std::shared_timed_mutex> w_lock;
#endif // end of c++14 check


class UI {
  
}


class Peer {
  //synch with peer
  // ~~~~~~~~~~~~~~~~~~~~~~~~ Vidar Herman ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void synch();
  
  
}



class Core: public Library {
  
  // ~~~~~~~~~~~~~~~~~~~~~~~~ Anton Christoffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  //synch with all peers
  void synch_all();

  void req_chunks(const Id& mid, const unordered_set<Id>& cids); //request chunks
  
  void download_file(const Id& mid, const string& dest_path = DEFAULT_DEST_PATH, const string& file_name = ""){
    if(get_file(mid,dest_path_generated)) return;
    req_file(mid);
  }
  
  void cancel_req_file(const Id& mid);
}



//handles metadata&tags&search

// ~~~~~~~~~~~~~~~~~~~~~~~~ Andreas ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Library : public Inventory {
public:
  
  virtual void req_chunks(const Id& mid, const unordered_set<Id>& cids) = 0; //request chunks
  
  void handle_chunk(const Id& mid, const Chunk& chunk){
    w_lock l(data.chunk_reqs_mtx);  
    if (!data.chunk_req_exists(mid,cid)) return;
    data.chunk_reqs[mid].erase(chunk.cid);
    wl.unlock();
    add_chunk(mid,chunk); // TODO: no space!!! (handle disk space errors)
    l.lock()
    if(data.file_req_exists(mid)) return;
    //build metabody OR we are done
    if(data.has_medabody(mid)){
      data.chunk_reqs.erase(mid);
      wl.unlock();
      download_file(mid);
      return;
    }
    l.unlock();
    
    // build metabody
    Metahead metahead;
    if(!get_metahead(mid, metahead)){
      //something is wrong already here
      l.lock();
      data.chunk_reqs.erase(mid);
      return;
    }
    
    Metabody metabody;
    
    if(!get_metabody(mid,metahead.bid,metabody)){
      l.lock();
      data.chunk_reqs[mid].insert(metabody.bid_next);
      req_chunks(mid,data.chunk_reqs[mid]);
      return;
    }
    
    l.lock()
    for(const Chunk& chunk:metabody.chunks)
      data.chunk_reqs[mid].emplace(chunk.cid);
    req_chunks(mid,data.chunk_reqs[mid]);
  }

  //upload file and trigger synch_all
  void upload_file(const string& filename, const string& tags = ""){
    upload_file(const string& filename, Metahead& metahead);
    synch_all();
  }

  void search(const string& pattern, vector<hast_t>& mids);
  bool get_metahead(const Id& mid, Metahead& metahead);
  
  
  //process file request
  bool req_file(const Id& mid){
    w_lock l(data.chunk_reqs_mtx);
    if(data.file_req_exists(mid)) return true;
    
    //TODO: verify what we have already!
    Metahead metahead;
    if(!get_metahead(mid,metahead)) return false; //make sure no deadlock!
    data.chunk_reqs[mid].insert(bid);
    req_chunks(mid,data.chunk_reqs[mid]);
    return true;
  }
  
  struct Data {
    //unordered_set<Id> file_reqs; //files download
    
    
    unordered_map<Id, <unordered_set<Id> > chunk_reqs; // chunk_req_map[mid] == set of chunks we are waiting for for that file 
    unordered_set<Id> has_metabody_; //rename?
    
    inline bool file_req_exists(const Id& mid){ return chunks_reqs.find(mid) != chunks_reqs.end();}; 
    inline bool chunk_req_exists(const Id& mid,const Id& cid){ return (file_req_exists(mid) && chunk_reqs[mid].find(cid) != chunk_reqs[mid].end() );}
    inline bool has_metabody(const Id& mid){return has_metabody_.find(mid) !=has_metadbody_.end();}
    
    rw_mutex chunk_reqs_mtx; 
  } data;


private:

  virtual void synch_all() = 0;
}

//has chunk table and metadata table
// ~~~~~~~~~~~~~~~~~~~~~~~~ Robert ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Inventory : protected Database {
public:
  /* to be used by a search function
     * create a metabody,
     * create temp file from chunks, verifying each one
     * if successful move to dest_path ant return true
     */
  bool get_file(const Id& mid, const string& dest_path);

protected:

  /* interate through the file adding chunks sequentially to arena
     * and building metabody in progress. Then add metabody to arena.
     * finally compute and return metahead
     */
  void upload_file(const string& filename, Metahead& metahead);

  //TODO: trolls can make infinitely big lists
  bool get_metabody(const Id& mid, const Id& bid, Metabody& metabody){
    Chunk chunk;
    do{
      if(!get_chunk(mid,bid,chunk)) return false;
    } while(medabody.append_from_chunk(chunk));
  }

  void add_metabody(const Metabody& metabody){
    const vector<Chunk>& chunks = metabody.to_chunks();
    for(const Chunk& chunk : chunks) add_chunk(chunk);
  }

  //write chunk to file, chunk_map and database
  //TODO: (optimization) queue ~1000 cids in memory before putting them into database
  bool get_chunk(const Id& mid, const Id& cid, Chunk &chunk);
  void add_chunk(const Id& mid, const Chunk& chunk);  //take size into account

private:

  struct {
    unordered_map<Id, unordered_map<Id, size_t> > chunk_map; //inmemory map[mid][cid] == chunk_offs 
    fstream arena;
  } data;
  rw_mutex chuck_mtx; //mutex

}


//magnet link 
struct Metahead {
  Id mid, bid;
  size_t file_size;
  string tags;
}

//.torrent
struct Metabody {
  Id bid, bid_next = 0; //hash of the body and next
  vector<Id> chunks; //hash of chunks
  bool append_from_chunk(const Chunk& chunk); //returns false once last chunk
  vector<Chunk> to_chunks();
}

struct Chunck {
  Id cid;
  string data;
  Chunk(string& data_):data(move(data_)), cid(hash512(data_)){}
  inline bool verify(const Id& cid_){ return cid_ ==cid;}
  inline size_t size(){return data.size();}
}

