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


void Database::get_data(const uint64_t &ref_id, vector<uint64_t> &id, vector<string> &value1, vector<uint64_t> &value2){
  Result_ptr res = exec_q(q_items,ref_id);
  while(res->next()) {
    id.emplace_back(res->get_uint64_t(0));
    value1.emplace_back(res->get_string(1));
    value2.emplace_back(res->get_uint64_t(2));
  }
}

