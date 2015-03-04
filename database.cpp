#include "database.h"


Database::Database():DATABASE_PATH(DEFAULT_DATABASE_PATH){
  debug("creating database..");
  init_db();
}

Database::~Database(){
  debug("destroying database..");
  shutdown_db();
}

void Database::init_db(){
  debug("initializing database..");
  if(SQLITE_OK!=sqlite3_initialize())
    debug(" *** could not initialize database");
  //create tables
  exec_s(s_create_table);
}

void Database::shutdown_db(){
  sqlite3_shutdown();
}

void Database::get_data(const string& Tag_ref, vector<const string>& Id, vector<uint64_t>& Size,vector<string>& Tags, vector<hash_t>& Hash_vl){
  Result_ptr res = exec_q(q_items,Tag_ref);

  while(res->next()) {
    Id.emplace_back(res->get_string(0));
    Size.emplace_back(res->get_uint64_t(1));
    Tags.emplace_back(res->get_string(2));
    //Hash_vl.emplace_back(res->get_string(3));
  }
}

