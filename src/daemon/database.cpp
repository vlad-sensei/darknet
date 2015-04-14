#include "database.h"

Database::Database(): Sqlite3_base(get_database_path()){
//Database::Database(): Sqlite3_base(DEFAULT_DATABASE_PATH){
  debug("creating database [%s]..",get_database_path());
  init_db();
}

//Needed by unitests to work
Database::Database(const string& path): Sqlite3_base(path){
//Database::Database(): Sqlite3_base(DEFAULT_DATABASE_PATH){
  debug("creating database [%s]..",path);
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
  exec_s(C_CHUNKS);
  exec_s(IDX_CHUNKS_BID);
  exec_s(IDX_CHUNKS_BID_CID);
}

void Database::get_all_metaheads(vector<Metahead>& metaheads){
  debug("getting all..");
  Result_ptr res = exec_q(Q_ALL_METAHEADS);
  while(res->next()) {
    Metahead tmp(res->get_id(2),res->get_string(1));
    metaheads.emplace_back(tmp);
  }
}

bool Database::get_metahead(const Id& mid, Metahead& metahead){
  debug("getting metahead..");
  Result_ptr res = exec_q(Q_METAHEAD,mid);
  if(!res->next())return false;
  metahead.mid = mid;
  metahead.tags = res->get_string(0);
  metahead.bid = res->get_id(1);
  return true;
}

void Database::get_mids_by_tag_pattern(const string& pattern, vector<Id>& mids){
  Result_ptr res = exec_q(Q_MIDS_BY_TAG_PATTERN,pattern);
  while(res->next()) mids.emplace_back(res->get_id(0));
}

bool Database::get_chunk(const Id &bid, const Id &cid, size_t &size, size_t &slot){
  Result_ptr res = exec_q(Q_CHUNK, bid, cid);
  if(!res->next()) return false;
  size = res->get_size_t(0);
  slot = res->get_size_t(1);
  return true;
}

void Database::get_random_metaheads(const int n, vector<Metahead>& metaheads){
  Result_ptr res = exec_q(Q_RANDOM_METAHEADS, n);
  while(res->next()) {
    metaheads.emplace_back(res->get_id(2), res->get_string(1));
  }
}
