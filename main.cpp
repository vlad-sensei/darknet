#include <sstream>

#include "glob.h"
//#include "core.h"

using namespace std;

//int main(int argc, char* argv[]){
//  try{
//    if(argc>2){
//      printf("./darknet [PORT]\n");
//      return 0;
//    }
//    uint16_t port = LISTEN_PORT;
//    if(argc==2) {
//      istringstream ss(argv[1]);
//      if(!(ss >> port)){
//        printf("./darknet [PORT]\n");
//        return 0;
//      }
//    }
//    ba::io_service io_s;
//    debug("listening at port %u", port);
//    tcp::endpoint endpoint(tcp::v4(),port);
//    core.reset(new Core(io_s,endpoint));
//    io_s.run();
//  } catch(std::exception& e){
//    debug(" *** exception : %s", e.what());
//  }
//  debug("terminated");

//  return 0;
//}
#include "library.h"


int main(int argc, char* argv[]){
    Library_ptr lib;
    lib.reset(new Library());


}
