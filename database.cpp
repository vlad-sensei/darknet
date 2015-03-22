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
  //exec_s(C_SESSION);
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

