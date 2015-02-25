#include "library.h"

#include <stdio.h>




int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}



bool Library::sql_exec(string command,int (*callback)(void *NotUsed, int argc, char **argv, char **azColName))
{
   char *zErrMsg = 0;
   int  rc;
   const char *sql=command.c_str();

   /* Execute SQL statement */
   rc = sqlite3_exec(metadata_db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      debug("faile with: \n %s \n",command.c_str());
      return false;
   }else{
      debug("execute command: \n %s \n",command.c_str());
      return true;
   }
}

bool Library::open_db(){
  int rc;
  string sql;
  rc=sqlite3_open("darknet.db",&metadata_db);

  if(rc){
      debug("no db opend\n");
      return false;
  }else{
      debug("db opend\n");

      sql = "CREATE TABLE METADATA("  \
              "ID INT PRIMARY KEY     NOT NULL," \
              "ID_TOFILE           TEXT    NOT NULL," \
              "TAGS            TEXT     NOT NULL)" ;

      creat_table(sql,"'METADATA'");
      sql = "CREATE TABLE LFILES("  \
              "ID INT PRIMARY KEY     NOT NULL," \
              "PWD_TOFILE           TEXT    NOT NULL)";
      creat_table(sql,"'LFILES'");
      return true;
  }


}

void Library::creat_table(string command,string name){
    string sql="select 1 from sqlite_master where type='table' and name="+name;
    char ** result;
    int row=0;
    int colum=0;
    char *err_msg = NULL;

    sqlite3_get_table(metadata_db,sql.c_str(),&result,&row,&colum,&err_msg);

    if(row==0){
        sql_exec(command,NULL);
    }
    sqlite3_free_table(result);
    sqlite3_free(err_msg);
}

Library::Library(){
    debug("creating library..\n");
    open_db();
}
Library::~Library(){
    close_db();
}


