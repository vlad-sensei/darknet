#include "glob.h"
#include "ui.h"


bool process_args(int argc, char* argv[],uint16_t& ui_port){
  if(argc>2){
    printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
    return false;
  }

  uint16_t port = DEFAULT_UI_LISTEN_PORT;
  if(argc==2) {
    istringstream ss(argv[1]);
    if(!(ss >> port)){
      printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
      return false;
    }

    ui_port = port;

    debug("has set UI_PORT: [%d]",ui_port);
  }else{
    debug("wrong arg count");
    printf("clientconsole [PORT=%d]\n",DEFAULT_UI_LISTEN_PORT);
  }
  return true;
}


int main(int argc, char* argv[]) {
  try{
    uint16_t tmp_port = DEFAULT_UI_LISTEN_PORT;
    uint16_t& ui_port = tmp_port;

    if(!process_args(argc, argv,ui_port)) return 0;


    ui = UI_ptr(new UI);
    ui->run(ui_port);
  } catch (exception& e){
    debug(" *** exception : %s", e.what());
  }

  debug("terminated");

  return 0;
}
