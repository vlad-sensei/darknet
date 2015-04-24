#include <sstream>

#include <boost/filesystem.hpp>

#include "glob.h"
#include "core.h"

using namespace std;

#define HELP_MSG "TODO: ADD HELP_MSG HERE"

bool get_args(int argc, char* argv[], uint16_t& daemon_port, uint16_t& client_port, string& database_path, string& arena_path){
  for(int i = 0; i<argc; ++i){
    do{
      string arg = string(argv[i]);
      if(arg=="-h" || arg =="--help"){
        safe_printf("%s\n", HELP_MSG);
        break;
      }
      if(++i == argc) return false;
      string val = argv[i];
      if(arg=="-p" || arg == "--port") {
        try{ daemon_port = stoul(val);} catch(const exception&){}
        break;
      }

      if(arg=="-p_d" || arg == "--port_d"){
        try{ client_port = stoul(val);} catch(const exception&){}
        break;
      }

      if(arg=="-path" || arg == "--path_database"){
        database_path = move(val);
        break;
      }

      if(arg=="-path_a" || arg == "--path_arena"){
        arena_path = move(val);
        break;
      }

      return false;
    } while(false);
  }
  return true;
}

int main(int argc, char* argv[]){
  try{
    uint16_t daemon_port = DEFAULT_LISTEN_PORT;
    uint16_t client_port = DEFAULT_UI_LISTEN_PORT;
    string arena_path = DEFAULT_ARENA_PATH;
    string database_path = DEFAULT_DATABASE_PATH;
    if(!get_args(argc, argv,daemon_port, client_port, arena_path, database_path)) {
      safe_printf("%s\n", HELP_MSG);
      return 0;
    }

    core.reset(new Core);
    core->set_daemon_port(daemon_port);
    core->set_client_port(client_port);
    core->set_arena_path(arena_path);
    core->set_database_path(database_path);

    core->run();
  } catch(std::exception& e){
    debug(" *** exception : %s", e.what());
  }
  debug("terminated");

  return 0;
}
