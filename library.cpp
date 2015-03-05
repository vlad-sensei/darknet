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

    ifstream in(filepath, std::ifstream::ate | std::ifstream::binary);
    int Size=in.tellg();

    //TODO get verifikationlist from inventory
    hash_t Hash_vl=hash512(filepath);
    //

    string to_be_hash=to_string(Size);

    hash_t Id=hash512(Hash_vl+to_be_hash+Tags);

    db->add_data(Id,Size,Tags,Hash_vl);

    vector<string> Id_v,Hash_vl_v,Tags_v;
    vector<uint64_t> Size_v;

    db->get_data_on_metadata(Id,Id_v,Size_v,Tags_v,Hash_vl_v);

    //debug("test");
    for(unsigned i=0; i<Id_v.size(); i++)
        debug("[Id:%s][Size:%lu][Tags:%s][Hash:%s]\n",Id_v[i].c_str() ,Size_v[i],Tags_v[i].c_str(),Hash_vl_v[i].c_str());




}

