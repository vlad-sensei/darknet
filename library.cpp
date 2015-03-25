#include "library.h"
Library::Library() {
  debug("initializing Library..");

  //TODO remove this it's for test
  upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej1:tja1");

  upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej2","hej2:tja2");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej3","hej3:tja3");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej4","hej4:tja4");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej4","hej5tja");
   vector<Metahead> metaheads;
   Metahead metahead;
  // vector<uint64_t>size;
  // vector<string>tags;
  // vector<Id>bids;
  // search("hej",mids);
   get_all_metaheads(metaheads);
   for(const Metahead& mh: metaheads){
    debug("[Id:%s][Tags:%s][Hash:%s]\n",mh.mid,mh.tags,mh.bid);
   }
  // Metahead metahead;
  if (Database::get_metahead(metaheads[0].mid,metahead)) debug("get [Id:%s][Tags:%s][Hash:%s]\n",metahead.mid,metahead.tags,metahead.bid);

}



void Library::upload_file(const string& filename, const string& tags){
  /*
   * TODO: do properly, in other words when Inventory is done
   * What we have now is temporary until then
   */

  ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  //int size=in.tellg();
  Metahead metahead(hash512_t(filename),tags);
//  metahead.file_size=size;
//  metahead.bid=hash512_t(filename);
//  metahead.tags=tags;
//  metahead.mid=hash512_t(filename+to_string(metahead.file_size)+metahead.tags + string((char*)&metahead.bid, sizeof(metahead.bid)));


  add_metahead(metahead);
}

void Library::search(const string& pattern, vector<Id>& mids){
  get_mids_by_tag_pattern(pattern,mids);
}

bool Library::get_metahead(const Id& mid, Metahead& metahead){
  return get_metahead(mid,metahead);
}
