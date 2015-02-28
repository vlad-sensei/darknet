#include "sqlite3_base.h"

string Sqlite3_base::Result::get_string(const unsigned &pos){
  return string((char*)sqlite3_column_blob(stmt_,pos),sqlite3_column_bytes(stmt_,pos));
}

uint64_t Sqlite3_base::Result::get_uint64_t(const unsigned& pos){
  return sqlite3_column_int64(stmt_,pos);
}
