#include "library.h"
Library::Library() {
  debug("initializing Library..");
//  Metabody metabody;
//  chunkFile("tjena",metabody);
//  deque<Chunk> chunks=metabody.create_body_chunks();
//  rename_bid_table(Id("tmp"),metabody.bid);
//  for(Chunk chunk:chunks){
//      debug("adding chunk\n[hash %s]\n[metahead %s]",chunk.cid,metabody.bid);
//      add_chunk(metabody.bid,chunk);
//  }
//  Metabody new_bid(metabody.bid);
//  Chunk chunk;
//  get_chunk(metabody.bid,metabody.bid,chunk);
//  new_bid.append_from_chunk(chunk);
//  debug("size %s",new_bid.cids.size());

//  joinFile(new_bid,"pdf");
  debug("size %s",sizeof(Id));
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
