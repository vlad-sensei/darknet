#ifndef GLOB_H
#define GLOB_H

// me no like 'using' from header files (especially not std) -R
using namespace std;

//for now
#include <cstdio>
#include <iostream>

#include <boost/multiprecision/integer.hpp>
typedef boost::multiprecision::uint512_t uint512_t;
typedef uint512_t uuid_t;
typedef uint64_t peer_id_t;


#define LISTEN_PORT 8453


#define DEBUG_ON
#ifdef DEBUG_ON
#include <string>
#include <ctime>
#endif

/* debug - the unix way. logging to file is done by piping output to (e.g)
 * tee, which logs to file. our program logs to stdout.
 * TODO: the windows way?
 * suggestion: rename this to "log" instead of debug

usage:
debug("%s\n", "hello, debug()");
[...]
$ g++ -std=c++11 -DDEBUG_ON test.cpp
$ ./a.out
Fri Feb 20 10:45:53 2015: hello, debug()
*/
template <typename ...Ts>
void debug(const std::string& fmt, const Ts&...args){
#ifdef DEBUG_ON
  //for this function for now
#pragma GCC diagnostic ignored "-Wformat-security"
  using namespace std;

  time_t time_now = time(nullptr);
  std::string datestr = asctime(localtime(&time_now)); // TODO: maybe fix a "better" (shorter) format?
  datestr.pop_back(); // remove the last "\n"

  //TODO: surpress warnings?
  size_t size = 1 + snprintf(nullptr, 0, fmt.c_str(), args ...); // count necessary size
  string formatted(size,'\0');

  snprintf(&formatted[0], size, fmt.c_str(), args ...); // actually do the formatting
  printf("[%s] %s\n", datestr.c_str(),formatted.c_str());
#else
  (void)fmt;
#endif // end of DEBUG_ON
}
//end of Debug

#endif // GLOB_H
