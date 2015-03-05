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

    lib->upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej1");
    lib->upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej2");
    lib->upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej3");
    lib->upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej4");
}
