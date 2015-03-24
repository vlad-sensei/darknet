#ifndef DATABASE_H
#define DATABASE_H

/*
 * Database
 *
 * NOTE: not all types are emplemented,
 * implement them in sqlite3_base.h on demand
 *
 * uses :
 * exec_s(prepared_statement, args...);
 * Result_ptr res = exec_q(prepared_statement, args...);
 * while(res->next()){ some_var = res->get_some_var_type(column_index);}
 *
 */

#include <vector>

#include "glob.h"
#include "sqlite3_base.h"


class Database:protected Sqlite3_base {
public:
  Database();
  ~Database();

private:
  /*
   * all sql squeries go here
   * format:
   * C_<statement>
   * I_<insert statement>
   * U_<update statement>
   * Q_<(select) query>
   */
  const string

  /*
   * remove once there is some code avaible
   * EXAMPLES:
   */
  //create statements
  //C_SESSION = "CREATE TABLE session (uid UNSIGNED BIGINT PRIMARY KEY, auth_token BLOB NOT NULL, users_mod_at BIGINT DEFAULT NULL, posts_mod_at BIGINT DEFAULT NULL, rooms_mod_at BIGINT DEFAULT NULL)",

  C_MIDS ="CREATE TABLE IF NOT EXISTS mids (mid BLOB,size UNSIGNED BIGINT,tags BLOB,bid BLOB);",


  //IDX_CHAT_MESSAGES_RID = "CREATE INDEX chat_messages_rid_index ON chat_messages (rid)",

  //insert statement

  //I_SESSION = "INSERT INTO session (uid, auth_token) VALUES (?,?);",
  I_MID = "INSERT INTO mids (mid, size,tags,bid) VALUES (?,?,?,?);",

  //update

  U_USER = "UPDATE users SET name = ?,mod_at=? WHERE uid=?",

  //queries (select)

  //Q_SESSION = "SELECT uid, auth_token FROM session;",

  Q_TAGS = "SELECT mid FROM mids WHERE tags LIKE '%' || ? || '%';",
  Q_ALL = "SELECT * FROM mids;",
  Q_MID = "SELECT * FROM mids WHERE mid=?;",
  EMPTY_STRING ="";

protected:
  void init_db();

  //  inline void add_session(const Id& uid, const string& auth_token){exec_s(I_SESSION,uid,auth_token);}


public:
  //write
  //void add_data(const string& value1, const uint64_t& value2) {exec_s(i_items,value1,value2);}

  void add_mid(Id mid, const uint64_t& size,const string& tags,Id bid) {exec_s(I_MID,mid,size,tags,bid);}

  //read
  //void get_data(const uint64_t& ref_id, vector<uint64_t>& id, vector<string>& value1, vector<uint64_t>& value2);
  void get_all(vector<Id>& mid,vector<uint64_t>& size,vector<string>& tags,vector<Id>& bid);

  bool get_mid(const Id& ref,Id& mid,uint64_t& size,string& tags,Id& bid);

  void get_mids(const string& ref_pettern, vector<Id>& id);

};

#endif // DATABASE_H
