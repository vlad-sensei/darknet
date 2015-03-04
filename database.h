#ifndef DATABASE_H
#define DATABASE_H

/*
 * Database
 *
 * NOTE: not all types are emplemented,
 * implement them in sqlite3_base.h on demand
 *
 * example usage:
 * #include "glob.h"
 * #include "database.h"
 *
 * #include<memory>
 *
 * int main() {
 *   unique_ptr<Database> db(new Database);
 *   for(uint64_t i = 0; i<10UL; i++)
 *     db->add_data("it works",i);
 *   vector<uint64_t> id,value2;
 *  vector<string> value1;
 *   db->get_data(6,id,value1,value2);
 *   db.reset();
 *   for(unsigned i=0; i<id.size(); i++)
 *     printf("[id:%lu][value1:%s][value2:%lu]\n",id[i],value1[i].c_str(),value2[i]);
 *   return 0;
 * }
 *
 */

#define DEFAULT_DATABASE_PATH "test.db"

#include "glob.h"
#include "sqlite3_base.h"

class Database: Sqlite3_base {
public:
  Database();
  ~Database();

private:
  const string DATABASE_PATH = DEFAULT_DATABASE_PATH;
  virtual const string& get_database_path() const {return DATABASE_PATH;}
  /*
   * all sql squeries go here
   * format:
   * s_<statement>
   * i_<insert statement>
   * u_<update statement>
   * q_<(select) query>
   */
  const string

  //statements (misc)
  //s_create_table = "CREATE TABLE IF NOT EXISTS items (id UNSEGNED BIGINT PRIMARY KEY, value1 TEXT, value2 UNSIGNED BIGINT);",
  s_create_table = "CREATE TABLE IF NOT EXISTS items (id TEXT, Size UNSIGNED BIGINT, Tags TEXT,Hash_vl TEXT);",
  //insert statement
  //i_items_2 = "INSERT INTO items (value1, value2) VALUES (?,?);",
  i_items = "INSERT INTO items (id,Size, Tags,Hash_vl) VALUES (?,?,?,?);",


  //queries (select)
  //q_items = "SELECT id,value1,value2 FROM items WHERE id<?;",
  q_items = "SELECT * FROM items WHERE Tags like %?%;",



  //dummy, for the last ";" basicall;
  EMPTY_STRING ="";

protected:
  void init_db();
  void shutdown_db();
public:
  //write
  void add_data(const hash_t& Id,const uint64_t& Size,const string& Tags, const uint64_t& Hash_vl) {exec_s(i_items,Id,Size,Tags,Hash_vl);}

  //read
  void get_data(const string& Tag_ref, vector<const string>& Id, vector<uint64_t>& Size, vector<string>& Tags, vector<hash_t> &Hash_vl);

};

#endif // DATABASE_H
