#include "sqlite3_base.h"

Sqlite3_base::Sqlite3_base(const string& db_path): SQLITE3_PATH(db_path){
  sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
  if(SQLITE_OK!=sqlite3_initialize())
    debug(" *** could not initialize database");
}

Sqlite3_base::~Sqlite3_base(){
  sqlite3_shutdown();
}

Sqlite3_base::Result::~Result(){
  sqlite3_finalize(stmt_);
  sqlite3_close(db_);
}

bool Sqlite3_base::Result::next(){
  int ret = sqlite3_step(stmt_);
  if(ret==SQLITE_ROW) return true;
  if(ret!=SQLITE_DONE){
    debug(" *** QUERY ERROR: %s", sqlite3_errmsg(db_));
    debug(" *** errc : %d",ret);
  }
  return false;
}

string Sqlite3_base::Result::get_string(const unsigned &pos){
  return string((char*)sqlite3_column_blob(stmt_,pos),sqlite3_column_bytes(stmt_,pos));
}

Id Sqlite3_base::Result::get_id(const unsigned& pos){
  Id res;
  memcpy((char*)&res,(char*)sqlite3_column_blob(stmt_,pos),sqlite3_column_bytes(stmt_,pos));
  return res;
}

ts_t Sqlite3_base::Result::get_ts_t(const unsigned &pos){
  return sqlite3_column_int64(stmt_,pos);
}

size_t Sqlite3_base::Result::get_size_t(const unsigned &pos){
  return (size_t)sqlite3_column_int64(stmt_,pos);
}
