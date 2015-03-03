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

#include "database.h"
#include<memory>


int main(int argc, char* argv[]){
    unique_ptr<Database> db(new Database);
    for(uint64_t i = 0; i<10UL; i++)
        db->add_data(i,"it works",i);
    vector<uint64_t> id,value2;
    vector<string> value1;
    db->get_data(6,id,value1,value2);
    db.reset();
    for(unsigned i=0; i<value2.size(); i++)
        printf("[id:%lu][value1:%s][value2:%lu]\n",id[i],value1[i].c_str(),value2[i]);

}
