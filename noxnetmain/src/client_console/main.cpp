#include "glob.h"
#include "ui.h"

int main() {
  try{
    UI_ptr ui = UI_ptr(new UI);
    ui->run();
  } catch (exception& e){
    debug(" *** exception : %s", e.what());
  }

  debug("terminated");

  return 0;
}
