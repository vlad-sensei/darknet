#ifndef GLOB_H
#define GLOB_H

using namespace std;

//for now
#include <cstdio>
#include <iostream>



// Debug
// make better
template <typename ...Ts>
void debug(const char* fmt, Ts&&...args){
  printf(fmt,args...);
}

//end of Debug

#define SERVER_PORT 8453

#endif // GLOB_H
