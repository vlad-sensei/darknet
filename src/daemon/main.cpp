#include <sstream>

#include <boost/filesystem.hpp>

#include "glob.h"
#include "core.h"

using namespace std;

typedef boost::filesystem::path path;


// Should have 'printf' argument as variable probably
bool process_args(int argc, char* argv[],uint16_t& ui_port){

  if(argc > 5){
    printf("darknet [PORT=%d,UI_PORT=%d,DATABASE_PATH=\"%s\",ARENA_PATH=\"%s\"]\n",DEFAULT_LISTEN_PORT,DEFAULT_UI_LISTEN_PORT,DEFAULT_DATABASE_PATH,DEFAULT_ARENA_PATH);
    return false;
  }

  if(argc == 5) {
    uint16_t port = DEFAULT_LISTEN_PORT;
    uint16_t tmp_ui_port = DEFAULT_UI_LISTEN_PORT;
    istringstream ss(argv[1]);
    if(!(ss >> port)){
      safe_printf("darknet [PORT=%d,UI_PORT=%d,DATABASE_PATH=\"%s\",ARENA_PATH=\"%s\"]\n",DEFAULT_LISTEN_PORT,DEFAULT_UI_LISTEN_PORT,DEFAULT_DATABASE_PATH,DEFAULT_ARENA_PATH);
      return false;
    }

    istringstream ss2(argv[2]);
    if(!(ss2 >> tmp_ui_port)){
      safe_printf("darknet [PORT=%d,UI_PORT=%d,DATABASE_PATH=\"%s\",ARENA_PATH=\"%s\"]\n",DEFAULT_LISTEN_PORT,DEFAULT_UI_LISTEN_PORT,DEFAULT_DATABASE_PATH,DEFAULT_ARENA_PATH);
      return false;
    }


    path database_path(argv[3]);
    path arena_path(argv[4]);

    // file name checking: http://www.boost.org/doc/libs/1_57_0/libs/filesystem/doc/portability_guide.htm

    // stem() ignores extension of files

    string database_name = database_path.stem().string();
    string arena_name = arena_path.stem().string();

    if(database_name.length() < 3 || !boost::filesystem::portable_name(database_name)){
      // Call it invalid. This will probably prevent some errors at least
      // Require 3 characters to prevent weird short names
      safe_printf("Database name [%s] is invalid or too short. Name must be at least 3 characters without extension",database_path.string());
      return false;
    }

    if(arena_name.length() < 2 || !boost::filesystem::portable_name(arena_name)){
      // Call it invalid. This will probably prevent some errors at least.
      // Require 3 characters to prevent weird short names
      safe_printf("Arena name [%s] is invalid or too short. Name must be at least 3 characters without extension",arena_path.string());
      return false;
    }

    /*
     * Could check extension of path (of database) to make sure you are
     * not overwriting some other type of file (than .db)
     */

    /*
     * Could use boost::filesystem::exists() or something to check
     * if file exists, but it could be leftovers from earlier runs,
     * so I leave it without checking for now.
     */


    DAEMON_PORT = port;
    ui_port = tmp_ui_port;
    DATABASE_PATH = database_path.string();
    ARENA_PATH = arena_path.string();
  }else{
    printf("darknet [PORT=%d,UI_PORT=%d,DATABASE_PATH=\"%s\",ARENA_PATH=\"%s\"]\n",DEFAULT_LISTEN_PORT,DEFAULT_UI_LISTEN_PORT,DEFAULT_DATABASE_PATH,DEFAULT_ARENA_PATH);
  }
  return true;
}

int main(int argc, char* argv[]){
  try{
    uint16_t tmp_port = DEFAULT_UI_LISTEN_PORT;
    uint16_t& ui_port = tmp_port;

    if(!process_args(argc, argv,ui_port)) return 0;
    //debug("listening at port %u", port);

    core.reset(new Core);
    core->set_port(DAEMON_PORT);

    // These debug messages make it work, but I don't know why...
    debug("Main database path : [%s]",get_database_path());
    debug("Main arena path : [%s]",get_arena_path());
    //    debug("Main daemon ui_port : [%s]",ui_port);

    core->run(ui_port);
  } catch(std::exception& e){
    debug(" *** exception : %s", e.what());
  }
  debug("terminated");

  return 0;
}
