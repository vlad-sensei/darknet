#include <boost/algorithm/string.hpp>
#include "glob.h"
#include "ui.h"


bool process_args(int argc, char* argv[],uint16_t& ui_port, string& cmd){
  if(argc>3){
    safe_printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
    return false;
  }

  uint16_t port = DEFAULT_UI_LISTEN_PORT;
  if(argc>=2) {
    istringstream ss(argv[1]);
    if(!(ss >> port)){
      printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
      return false;
    }

    ui_port = port;

    if (argc==3){
      string argv_cmd = argv[2];
      boost::replace_all(argv_cmd, "\"","");
      cmd = argv_cmd;
      }
  }else{
    printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
  }
  return true;
}


int main(int argc, char* argv[]) {
  try{
    uint16_t ui_port = DEFAULT_UI_LISTEN_PORT;
    string cmd = "";

    if(!process_args(argc, argv,ui_port, cmd)) return 0;


    ui = UI_ptr(new UI);
    ui->run(ui_port, cmd);
  } catch (exception& e){
    debug(" *** exception : %s", e.what());
  }

  debug("terminated");

  return 0;
}
