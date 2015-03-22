#include "library.h"
Library::Library()
{
 debug("initializing Library..");

//TODO remove this it's for test
//upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej1","hej1:tja1");
//upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej2","hej2:tja2");
//upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej3","hej3:tja3");
//upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej4","hej4:tja4");
//upload_file("/home/andno037/code/build-darknet-Desktop_Qt_5_3_0_GCC_64bit-Debug/hej4","hej5tja");
// vector<Id>mids;
// search("hej",mids);
// for(unsigned i=0; i<mids.size(); i++)
//    debug("[Id:%s]\n",mids[i] );

}



void Library::upload_file(const string& filename, const string& tags)
{
    //get the size of the file
    //TODO movie this to tags
    ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    int size=in.tellg();


    Metahead mid_;
    //TODO call upload file from invetory
    mid_.file_size=size;
    mid_.bid=hash512_t(filename);
    mid_.tags=tags;
    //
    mid_.mid=hash512_t(filename+to_string(mid_.file_size)+mid_.tags);

    add_mid(mid_.mid,mid_.file_size,mid_.tags,mid_.bid);
}

void Library::search(const string& pattern, vector<Id>& mids){
    get_mids(pattern,mids);
}
