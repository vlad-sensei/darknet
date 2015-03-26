#ifndef SQLITE3_BASE_H
#define SQLITE3_BASE_H

#define SQLITE3_MAX_SLEEP_MS 1000

#include <memory>
#include <cstdio>
#include <fstream>
#include <mutex>

#include <sqlite3.h>

#include "glob.h"

class Sqlite3_base {
  Sqlite3_base();
  const string DATABASE_PATH;
protected:
  Sqlite3_base(const string& db_path);
  ~Sqlite3_base();

  //Result wrapper
  class Result {
  public:
    Result(sqlite3* db, sqlite3_stmt* stmt):
    db_(db),stmt_(stmt){}
    ~Result();

    bool next();

    string get_string(const unsigned& pos);
    Id get_id(const unsigned& pos);
    ts_t get_ts_t(const unsigned& pos);

  private:
    sqlite3* db_;
    sqlite3_stmt * stmt_;
  };

  typedef unique_ptr<Result> Result_ptr;

private:

  //TODO add set_prep_var for Id
  inline void set_prep_var(sqlite3_stmt* stmt, const int& pos,const Id& var){sqlite3_bind_blob(stmt,pos,(char*)&var,sizeof(var),SQLITE_STATIC);}


  inline void set_prep_var(sqlite3_stmt* stmt, const int& pos, const string& var){sqlite3_bind_blob(stmt,pos,var.data(),var.size(),SQLITE_STATIC);}
  inline void set_prep_var(sqlite3_stmt* stmt, const int& pos, const uint64_t& var){sqlite3_bind_int64(stmt,pos,var);}

  template <size_t ARG_NUM>
  inline void bind_args(sqlite3 *db, sqlite3_stmt *stmt){(void)db;(void)stmt;}

  template<size_t ARG_NUM, typename T, typename ...Ts>
  inline void bind_args(sqlite3*db, sqlite3_stmt* stmt, const T&var, const Ts&...args){
    set_prep_var(stmt,ARG_NUM-sizeof...(Ts),var);
    bind_args<ARG_NUM>(db,stmt,args...);
  }

  inline void return_sql(sqlite3*db, sqlite3_stmt*stmt, void *){

    int ret;
    unique_lock<mutex> l(db_mtx);
    ret=sqlite3_step(stmt);
    l.unlock();
    if(SQLITE_DONE!=ret){
      debug(" *** SQLITE: %s",sqlite3_errmsg(db));
      debug(" *** errc : %d",ret);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
  }

  inline Result_ptr return_sql(sqlite3 *db, sqlite3_stmt *stmt, Result_ptr *){
    return Result_ptr(new Result(db,stmt));
  }

  template<typename Tr, typename ...Ts>
  inline Tr exec_sql(const string& sql, const Ts&...args){
    sqlite3* db;
    sqlite3_open_v2(DATABASE_PATH.c_str(),&db,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_NOMUTEX, NULL);
    sqlite3_stmt * stmt;
    sqlite3_busy_timeout(db,SQLITE3_MAX_SLEEP_MS);
    sqlite3_prepare_v2(db,sql.c_str(),(unsigned)sql.size(),&stmt,NULL);
    bind_args<sizeof...(args)>(db,stmt,args...);
    return return_sql(db,stmt, (Tr*)nullptr);
  }

  mutex db_mtx;

protected:

  template <typename ...Ts>
  inline void exec_s(const string& sql, const Ts&...args){exec_sql<void>(sql, args...);}

  template <typename ...Ts>
  inline Result_ptr exec_q(const string& sql, const Ts&...args){return exec_sql<Result_ptr>(sql, args...);}

  inline int remove_db(){return remove(DATABASE_PATH.c_str());}
  inline bool exists(){ifstream f(DATABASE_PATH.c_str()); return f.good();}

};

#endif // SQLITE3_BASE_H
