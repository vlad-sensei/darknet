#include "library.h"
Library::Library() {
  debug("initializing Library..");
  Metabody metahead;
  chunkFile("tjena",metahead);
//  for(Chunk chunk:metahead.create_body_chunks()){
//      debug("adding chunk [hash %s]",chunk.cid);
//      add_chunk(metahead.bid,chunk);
//  }
//  Metabody new_bid;
//  Chunk chunk;
//  get_chunk(metahead.bid,metahead.bid,chunk);
//  new_bid.append_from_chunk(chunk);


//  debug("metahead %s \n new_bid %s",metahead.bid,new_bid.bid);
//  for(Id bids:metahead.bids){
//      debug("reding chunk...");
//      get_chunk(metahead.bid,bids,chunk);
//      new_bid.append_from_chunk(chunk);
//  }


  joinFile(metahead,"pdf");
}


void Library::upload_file(const string& filename, const string& tags){
  /*
   * TODO: do properly, in other words when Inventory is done
   * What we have now is temporary until then
   */

  ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  Metahead metahead(hash512_t(filename),tags);
  add_metahead(metahead);
}

void Library::search(const string& pattern, vector<Id>& mids){
  get_mids_by_tag_pattern(pattern,mids);
}

bool Library::get_metahead(const Id& mid, Metahead& metahead){
  return get_metahead(mid,metahead);
}
