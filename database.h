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
  s_create_table = "CREATE TABLE IF NOT EXISTS items (id UNSEGNED BIGINT PRIMARY KEY, value1 TEXT, value2 UNSIGNED BIGINT);",

  //insert statement
  i_items_2 = "INSERT INTO items (value1, value2) VALUES (?,?);",
  i_items_3 = "INSERT INTO items (id,value1, value2) VALUES (?,?,?);",


  //queries (select)
  q_items = "SELECT id,value1,value2 FROM items WHERE id<?;",

  //dummy, for the last ";" basicall;
  EMPTY_STRING ="";

protected:
  void init_db();
  void shutdown_db();
public:
  //write
  void add_data(const uint64_t& id,const string& value1, const uint64_t& value2) {exec_s(i_items_3,id,value1,value2);}

  //read
  void get_data(const uint64_t& ref_id, vector<uint64_t>& id, vector<string>& value1, vector<uint64_t>& value2);

};

#endif // DATABASE_H
