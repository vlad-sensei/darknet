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

void Database::get_all(vector<Id>& mid,vector<uint64_t>& size,vector<string>& tags,vector<Id>& bid){
    debug("get all");
    Result_ptr res = exec_q(Q_ALL,"test");
    while(res->next()) {
    mid.emplace_back(res->get_id(0));
    size.emplace_back(res->get_ts_t(1));
    tags.emplace_back(res->get_string(2));
    bid.emplace_back(res->get_id(3));
    }
}

bool Database::get_mid(const Id& ref,Id& mid,uint64_t& size,string& tags,Id& bid){
    debug("get metahead");
    Result_ptr res = exec_q(Q_MID,ref);
    if(res->next()) {
    mid=res->get_id(0);
    size=res->get_ts_t(1);
    tags=res->get_string(2);
    bid=res->get_id(3);
    return true;
    }else return false;
}


void Database::get_mids(const string& ref_pattern, vector<Id>& id){
    Result_ptr res = exec_q(Q_TAGS,ref_pattern);
    while(res->next()) {
    id.emplace_back(res->get_id(0));
    }
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

