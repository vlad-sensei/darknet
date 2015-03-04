#include "library.h"

#include <stdio.h>
#include <fstream>


Library::Library(){
    debug("creating library..\n");
    db.reset(new Database);

}
Library::~Library(){
}


void Library::upload_file(const string& filepath,const string& Tags){

    //ifstream in(filepath, std::ifstream::ate | std::ifstream::binary);
    //int Size=in.tellg();

    //TODO get verifikationlist from inventory
    //hash_t Hash_vl=hash512(filepath);
    //

    //string to_be_hash=to_string(Size);

    //hash_t Id=hash512(Hash_vl+to_be_hash+Tags);

}

