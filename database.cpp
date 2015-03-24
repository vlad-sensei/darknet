#include "database.h"

Database::Database(): Sqlite3_base(DEFAULT_DATABASE_PATH){
  debug("creating database..");
  init_db();
}

Database::~Database(){
  debug("destroying database..");
}

void Database::init_db(){
  debug("initializing database..");
  remove_db(); // for clear for testing

  //create tables
  //creat table for mids
  exec_s(C_MIDS);
}

void Database::get_all_metaheads(vector<Metahead>& metaheads){
  debug("get all");
  Result_ptr res = exec_q(Q_ALL_METAHEADS);
  while(res->next()) {
    const Id& bid = res->get_id(0);
    const string& tags = res->get_id(1);
    metaheads.emplace_back(bid,tags);
  }
}

bool Database::get_metahead(const Id& mid,Metahead& metahead){
  debug("getting metahead..");
  Result_ptr res = exec_q(Q_MID,ref);
  if(!res->next())return false;
  metahead.mid=mid;
  metahead.tags=res->get_string(0);
  metahead.bid=res->get_id(1);
  return true;
}


void Database::get_mids_by_tag_pattern(const string& pattern, vector<Id>& mids){
  Result_ptr res = exec_q(Q_MIDS_BY_TAG_PATTERN,ref_pattern);
  while(res->next()) id.emplace_back(res->get_id(0));
}



/*
 * EXAMPLE
bool Database::get_session(Id &uid, string &auth_token){
  Result_ptr res = exec_q(Q_SESSION);
  if(!res->next()) return false;
  uid = res->get_id(0);
  auth_token = res->get_string(1);
  return true;
}
*/

