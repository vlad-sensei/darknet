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



bool Library::sql_exec_no_return(char *command)
{
   char *zErrMsg = 0;
   int  rc;


   /* Execute SQL statement */
   rc = sqlite3_exec(metadata_db, command, NULL, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      sqlite3_free(zErrMsg);
      debug("faile with: %s",command);
      return false;
   }else{
      debug("execute command: %s",command);
      return true;
   }
}

bool Library::sql_exec(string command)
{
   char *zErrMsg = 0;
   int  rc;
   const char *sql=command.c_str();

   /* Execute SQL statement */

   if( rc != SQLITE_OK ){
      sqlite3_free(zErrMsg);
      debug("faile with: %s",command.c_str());
      return false;
   }else{
      debug("execute command: %s",command.c_str());
      return true;
   }
}


void Library::search_and_get_row(string table,string sword,string cstring,string& ID,string& file_ID,string& tags){
    string sql = "select * from "+table+" where "+sword + " = "+cstring;
    const char* sqlc=sql.c_str();

    int rc = sqlite3_prepare(metadata_db,sqlc,strlen(sqlc),&prepared,NULL);

    sqlite3_step(prepared);
    get_data_row(ID, file_ID, tags);


    if(rc){
        debug("fail with preperd");
    }

}

void Library::get_data_row(string& ID,string& file_ID,string& tags){

    ID=(const char *) sqlite3_column_text(prepared,0);
    file_ID=(const char*)sqlite3_column_text(prepared,1);
    tags=(const char*)sqlite3_column_text(prepared,2);

}


void Library::search_tags_and_get_rows(string table,string cstring,string& ID,string& file_ID,string& tags){
    string sql = "select * from "+table+"where TAGS like " + "'%"+cstring+"%'";
    const char* sqlc=sql.c_str();

    int rc = sqlite3_prepare(metadata_db,sqlc,strlen(sqlc),&prepared,NULL);
    if(rc){
        debug("fail with preperd");
        sqlite3_free(prepared);
        return;
    }
    while(sqlite3_step(prepared)){
        get_data_row(ID, file_ID, tags);
    }


}



bool Library::open_db(){
  int rc;
  char * sql;
  const char* hej;
  rc=sqlite3_open("darknet.db",&metadata_db);
  if(rc){
      debug("no db opend\n");
      return false;
  }else{
      debug("db opend\n");

      sql = "CREATE TABLE IF NOT EXISTS METADATA("  \
              "ID   TEXT    NOT NULL," \
              "FILE_ID           TEXT    NOT NULL," \
              "TAGS            TEXT     NOT NULL)" ;

      sql_exec_no_return(sql);

      sql = "CREATE TABLE IF NOT EXISTS LFILES("  \
              "ID   TEXT     NOT NULL," \
              "PWD_TOFILE           TEXT    NOT NULL)";

      sql_exec_no_return(sql);

    string ID,file_id,tags;
    search_and_get_row("METADATA","ID","12345",ID,file_id,tags);

    debug("test: %s , %s ,%s ",ID.c_str(),file_id.c_str(),tags.c_str());



      return true;
  }


}


Library::Library(){
    debug("creating library..\n");





}
Library::~Library(){
    close_db();
}


