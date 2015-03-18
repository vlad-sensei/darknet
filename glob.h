#ifndef GLOB_H
#define GLOB_H

// me no like 'using' from header files (especially not std) -R
using namespace std;

#include <string>
#include <cinttypes>

typedef string hash_t;
typedef hash_t Id;
typedef int64_t ts_t; //time_t, different on different platforms
typedef uint64_t peer_id_t;

#define LISTEN_PORT 8453

#define DEFAULT_DATABASE_PATH "test.db"

#if __cplusplus < 201300L
// pre c++14
#include <boost/thread.hpp>
typedef boost::shared_mutex rw_mutex;
typedef boost::shared_lock<boost::shared_mutex> r_lock;
typedef boost::unique_lock<boost::shared_mutex> w_lock;
#else
// c++14
#include <shared_mutex>
typedef std::shared_timed_mutex rw_mutex;
typedef std::shared_lock<std::shared_timed_mutex> r_lock;
typedef std::unique_lock<std::shared_timed_mutex> w_lock;
#endif // end of c++14 check

// ~~~~~~~~~~~~~~~~~ functions ~~~~~~~~~~~~~~~~~

// ~~ Debug
#define DEBUG_ON
#ifdef DEBUG_ON
#include <string>
#include <ctime>
#include <memory>
#include <iostream>
#endif

// from wikipedia
inline void safe_printf(const char *s)
{
  while (*s) {
    if (*s == '%') {
      if (*(s + 1) == '%') {
        ++s;
      }
      else {
        // throw std::runtime_error("invalid format string: missing arguments");
        cout << "invalid format string: missing arguments\n";
      }
    }
    std::cout << *s++;
  }
}

template<typename T, typename... Args>
inline void safe_printf(const char *s, T value, Args... args)
{
  while (*s) {
    if (*s == '%') {
      if (*(s + 1) == '%') {
        ++s;
      }
      else {
        std::cout << value;
        s += 2;
        safe_printf(s, args...); // call even when *s == 0 to detect extra arguments
        return;
      }
    }
    std::cout << *s++;
  }
}

template <typename ...Ts>
void debug(const std::string& fmt, const Ts&...args){
#ifdef DEBUG_ON
  time_t now_ = time(nullptr);
  unique_ptr<struct tm> now = unique_ptr<struct tm>(new struct tm);
  localtime_r(&now_, now.get());
  string datestr = asctime(now.get());
  datestr.pop_back();
  cout << "["<<datestr << "] ";
  safe_printf(fmt.c_str(),args...);
  cout << "\n";
#endif //DEBUG_ON
}


// ~~ Keccak
/* Hash function that uses SHA-3, implemented using libcrypto++.
 * The function allocates a 64 byte on the heap using unique_ptr
 * the result is moved into the return value.
*/
hash_t hash512(const string& value);

/* Prints a hash_t using the debug function.*/
void debug_hash512(const hash_t&);

#endif // GLOB_H
