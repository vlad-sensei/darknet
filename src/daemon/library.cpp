#include "library.h"

#include <boost/filesystem.hpp>

Library::Library() {
  debug("initializing Library..");
}

bool Library::upload_file(const string& file_path, const string& tags,Id& mid){
  Metahead metahead;
  if(!Inventory::upload_file(file_path,metahead,tags))return false;
  mid=metahead.mid;
  add_metahead(metahead);
  return true;
}

void Library::search(const string& pattern, vector<Id>& mids){
  get_mids_by_tag_pattern(pattern,mids);
}

bool Library::get_metahead(const Id& mid, Metahead& metahead){
  return Database::get_metahead(mid,metahead);
}

vector<Metahead> Library::publish_metaheads(){
  vector<Metahead> res;
  Database::get_all_metaheads( res);
  return res;
}
