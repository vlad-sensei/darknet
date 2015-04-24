#include "glob.h"
#include "ui.h"

bool process_args(int argc, char* argv[],uint16_t& ui_port){
  uint16_t port = DEFAULT_UI_LISTEN_PORT;
  if(argc == 2) {
    istringstream ss(argv[1]);
    if(!(ss >> port)){
      safe_printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
      return false;
    }
  }
  ui_port = port;
  return true;
}


int main(int argc, char* argv[]) {
  try{
    uint16_t ui_port = DEFAULT_UI_LISTEN_PORT;

    if(!process_args(argc, argv,ui_port)) return 0;

    ui = UI_ptr(new UI);
    ui->run(ui_port);
  } catch (exception& e){
    debug(" *** exception : %s", e.what());
  }

  debug("terminated");

  return 0;
}
