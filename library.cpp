#include "library.h"
Library::Library() {
  debug("initializing Library..");
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
