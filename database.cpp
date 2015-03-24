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
  exec_s(C_METAHEADS);
}

void Database::get_all_metaheads(vector<Metahead>& metaheads){
  debug("get all");
  Result_ptr res = exec_q(Q_ALL_METAHEADS);
  while(res->next()) {
    Metahead tmp(res->get_id(2),res->get_string(1));
    metaheads.emplace_back(tmp);
  }
}

bool Database::get_metahead_from_db(const Id& mid, vector<Metahead>& metahead){
  debug("getting metahead..");
  Result_ptr res = exec_q(Q_METAHEAD,mid);
  if(!res->next())return false;
  const Id& bid = res->get_id(2);
  const string& tags = res->get_string(1);
  metahead.emplace_back(Metahead(bid,tags));
  return true;
}


void Database::get_metaheads(const string& pattern, vector<Id>& mids){
  Result_ptr res = exec_q(Q_MIDS_BY_TAG_PATTERN,pattern);
  while(res->next()) mids.emplace_back(res->get_id(0));
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

