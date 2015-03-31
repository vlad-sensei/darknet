#include "library.h"
Library::Library() {
  debug("initializing Library..");


}

void Library::run_test_uploader(){
  upload_file("tjena","tjena:pdf");
  vector<Id> mids;
  search("tjena",mids);
  Metahead metahead;
  get_metahead(mids[0],metahead);
  debug("[bid %s]",metahead.bid);
}


void Library::run_test_downloader(){
  Id bid;
  bid.tmp_set_data();
  data.chunk_reqs[bid]={bid};
  req_chunks(bid,data.chunk_reqs[bid]);

}

void Library::upload_file(const string& filename, const string& tags){
  /*
   * TODO: do properly, in other words when Inventory is done
   * What we have now is temporary until then
   */

  ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  Metahead metahead(hash512_t(filename),tags);
  Inventory::upload_file(filename,metahead);
  add_metahead(metahead);
}

void Library::search(const string& pattern, vector<Id>& mids){
  get_mids_by_tag_pattern(pattern,mids);
}

bool Library::get_metahead(const Id& mid, Metahead& metahead){
  return Database::get_metahead(mid,metahead);
}



void Library::handle_chunk(const Id& bid, const Chunk& chunk){
  w_lock l(chunk_reqs_mtx);
  if (!data.chunk_req_exists(bid,chunk.cid)){
    debug("ther is no reques for this:\n[bid %s]\n[chunk.cid %s]\n",bid,chunk.cid);
    return;
  }
  data.chunk_reqs[bid].erase(chunk.cid);
  l.unlock();
  add_chunk(bid,chunk); // TODO: no space!!! (handle disk space errors)
  l.lock();
  if(data.file_req_exists_and_not_empty(bid)){
    //debug("*** have more to get from the network");
    return;
  }
  //build metabody OR we are done
  if(data.has_metabody(bid)){
    data.chunk_reqs.erase(bid);
    l.unlock();
    get_file(bid,"TODO_get_name_of_file");
    return;
  }
  l.unlock();

  Metabody metabody;
  if(!get_metabody(bid,metabody)){
    debug("*** need more chunks for a metabody");
    l.lock();
    data.chunk_reqs[bid].insert(metabody.bid_next());
    req_chunks(bid,data.chunk_reqs[bid]);
    return;
  }else{
    data.has_metabody_.insert(bid);
  }
  l.lock();

  for(Id& cid:metabody.cids){
    data.chunk_reqs[bid].emplace(cid);
  }
  req_chunks(bid,data.chunk_reqs[bid]);
}

bool Library::req_file(const Id& mid){
  Metahead metahead;
  if(!get_metahead(mid,metahead)){
    debug("*** no mid found");
    return false;
  }
  //debug("req_file with:\n[mid %s]\n[bid %s]",mid,metahead.bid);
  data.chunk_reqs[metahead.bid]={metahead.bid};
  debug("*** TODO: check if file is on local pc");
  req_chunks(metahead.bid,data.chunk_reqs[metahead.bid]);
  return true;
}



