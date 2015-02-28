#ifndef SQLITE3_BASE_H
#define SQLITE3_BASE_H

#include <memory>

#include <sqlite3.h>

#include "glob.h"

class Sqlite3_base {
protected:

  virtual const string& get_database_path() const =0;

  class Result {
  public:
    Result(sqlite3* db, sqlite3_stmt* stmt):
      db_(db),stmt_(stmt){}
    ~Result(){ sqlite3_finalize(stmt_); sqlite3_close_v2(db_);}

    inline bool next(){return sqlite3_step(stmt_)==SQLITE_ROW;}

    string get_string(const unsigned& pos);
    uint64_t get_uint64_t(const unsigned& pos);

  private:
    sqlite3* db_;
    sqlite3_stmt * stmt_;
  };

  typedef unique_ptr<Result> Result_ptr;

private:
  inline void set_prep_var(sqlite3_stmt* stmt, const int& pos, const string& var){sqlite3_bind_text(stmt,pos,var.c_str(),var.size(),SQLITE_STATIC);}
  inline void set_prep_var(sqlite3_stmt* stmt, const int& pos, const uint64_t& var){sqlite3_bind_int64(stmt,pos,var);}

  template <size_t ARG_NUM>
  inline void bind_args(sqlite3 *db, sqlite3_stmt *stmt){(void)db;(void)stmt;}

  template<size_t ARG_NUM, typename T, typename ...Ts>
  inline void bind_args(sqlite3*db, sqlite3_stmt* stmt, const T&var, const Ts&...args){
    set_prep_var(stmt,ARG_NUM-sizeof...(Ts),var);
    bind_args<ARG_NUM>(db,stmt,args...);
  }

  inline void return_sql(sqlite3*db, sqlite3_stmt*stmt, void *){
    if(SQLITE_DONE!=sqlite3_step(stmt))
      debug(" *** SQLITE: something is wrong executing statement");
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
  }

  inline Result_ptr return_sql(sqlite3 *db, sqlite3_stmt *stmt, Result_ptr *){
    return Result_ptr(new Result(db,stmt));
  }

  protected:

  template<typename Tr, typename ...Ts>
  inline Tr exec_sql(const string& sql, const Ts&...args){
    sqlite3* db;
    sqlite3_open_v2(get_database_path().c_str(),&db,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(db,sql.c_str(),sql.size(),&stmt,NULL);
    bind_args<sizeof...(args)>(db,stmt,args...);
    return return_sql(db,stmt, (Tr*)nullptr);
  }
#define exec_s exec_sql<void>
#define exec_q exec_sql<Result_ptr>
};

#endif // SQLITE3_BASE_H
