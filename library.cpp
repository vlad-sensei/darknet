#include "library.h"
Library::Library()
{
  debug("initializing Library..");

  //TODO remove this it's for test
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej1:tja1");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej2","hej2:tja2");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej3","hej3:tja3");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej4","hej4:tja4");
  //upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej4","hej5tja");
  // vector<Id>mids;
  // vector<uint64_t>size;
  // vector<string>tags;
  // vector<Id>bids;
  // search("hej",mids);
  // get_all(mids,size,tags,bids);
  // for(unsigned i=0; i<mids.size(); i++){
  //       debug("[Id:%s]\n",mids[i] );
  //debug("[Id:%s][Size:%lu][Tags:%s][Hash:%s]\n",mids[i],size[i],tags[i],bids[i]);
  // }
  // Metahead metahead;
  // if (get_metahead(mids[1],metahead)) debug("found [Id:%s][Size:%lu][Tags:%s][Hash:%s]\n",metahead.mid,metahead.file_size,metahead.tags,metahead.bid);

}



void Library::upload_file(const string& filename, const string& tags){
  /*
   * TODO: do properly, in other words when Inventory is done
   * What we have now is temporary until then
   */

  ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  int size=in.tellg();
  Metahead metahead;
  metahead.file_size=size;
  metahead.bid=hash512_t(filename);
  metahead.tags=tags;
  metahead.mid=hash512_t(filename+to_string(metahead.file_size)+metahead.tags + string((char*)&metahead.bid, sizeof(metahead.bid)));

  add_mid(metahead.mid,metahead.file_size,metahead.tags,metahead.bid);
}

void Library::search(const string& pattern, vector<Id>& mids){
  get_mids(pattern,mids);
}

bool Library::get_metahead(const Id& mid, Metahead& metahead){
  return get_mid(mid,metahead.mid,metahead.file_size,metahead.tags,metahead.bid);
}
