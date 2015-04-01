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
#include "common.h"
#include "sqlite3_base.h"
#include <sstream>

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
  C_METAHEADS ="CREATE TABLE IF NOT EXISTS metaheads (mid BLOB PRIMARY KEY,tags BLOB,bid BLOB);",
  C_CHUNKS = "CREATE TABLE IF NOT EXISTS chunks (bid BLOB, cid BLOB, size UNSIGNED BIGINT, slot UNSIGNED BIGINT);",

  IDX_CHUNKS_BID = "CREATE INDEX idx_chunks_bid ON chunks (bid)",
  IDX_CHUNKS_BID_CID = "CREATE UNIQUE INDEX idx_chunks_bid_cid ON chunks (bid,cid)",


  //insert statement

  I_METAHEAD = "INSERT INTO metaheads (mid,tags,bid) VALUES (?,?,?);",
  I_CHUNK = "INSERT INTO chunks (bid,cid,size,slot) VALUES (?,?,?,?)",

  //update

  //U_USER = "UPDATE users SET name = ?,mod_at=? WHERE uid=?",

  //queries (select)

  Q_MIDS_BY_TAG_PATTERN = "SELECT mid FROM metaheads WHERE tags LIKE '%' || ? || '%';",
  Q_ALL_METAHEADS = "SELECT mid,tags,bid FROM metaheads;",
  Q_METAHEAD = "SELECT tags,bid FROM metaheads WHERE mid=?;",
  Q_CHUNK = "SELECT size,slot FROM chunks WHERE bid=? AND cid=?;",

  EMPTY_STRING ="";

protected:
  void init_db();

  //  inline void add_session(const Id& uid, const string& auth_token){exec_s(I_SESSION,uid,auth_token);}


public:
  //write
  //void add_data(const string& value1, const uint64_t& value2) {exec_s(i_items,value1,value2);}
protected:

  void add_chunk(const Id& bid, const Id& cid, const size_t& size, const size_t& slot){exec_s(I_CHUNK,bid,cid,size,slot);}
  void add_metahead(const Metahead & metahead) {exec_s(I_METAHEAD,metahead.mid,metahead.tags,metahead.bid);}

  //read
  void get_all_metaheads(vector<Metahead>& metaheads);

  bool get_metahead(const Id &mid, Metahead& metahead);
  void get_mids_by_tag_pattern(const string& ref_pettern, vector<Id>& id);
  bool get_chunk(const Id& bid, const Id& cid, size_t& size, size_t& slot);

};

#endif // DATABASE_H
