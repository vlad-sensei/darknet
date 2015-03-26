#ifndef GLOB_H
#define GLOB_H

// me no like 'using' from header files (especially not std) -R
using namespace std;

#include <string>
#include <cstring>
#include <cinttypes>
#include <iostream>
#include "cryptopp/sha.h"

/* Example usage:
  Id id1(""), id2("The quick brown fox jumps over the lazy dog"), id3("The quick brown fox jumps over the lazy dog."), id4("");
  debug("Ids: \n%s\n%s\n%s\n%s\n",id1,id2,id3,id4);
  debug("id2==id3 : %s; id1==id4 : %s", id2==id3 ? "true" : "false", id1==id4 ? "true" : "false");
  */

//TODO: actually make glob.cpp to not include cryptopp?
struct hash512_t{
  inline hash512_t(){}
  explicit inline hash512_t(const string& value){CryptoPP::SHA512().CalculateDigest((byte*)data, (byte*)value.data(), value.size());}
  inline bool operator== (const hash512_t& other)const {return  !memcmp(data, other.data, sizeof(data));}
  inline size_t std_hash() const {return data[0]^data[1]^data[2]^data[3]^data[4]^data[5]^data[6]^data[7];}
  friend void operator << (ostream& os, const hash512_t& h);
  //string get_string()const {return string((char*) data,sizeof(uint64_t)*8);}
  /*hash512_t& set(const string& blob){
      memcpy(data,blob.data(),sizeof(uint64_t)*8);
      return *this;
  }*/
private:
  uint64_t data[8];
};

inline void operator << (ostream& os, const hash512_t& h){ os << std::hex << h.data[0] << h.data[1] << h.data[2] << h.data[3] << h.data[4] << h.data[5] << h.data[6] << h.data[7] << std::dec;}

namespace std {
template<> struct hash<hash512_t>{
  inline size_t operator()(const hash512_t& value) const{ return value.std_hash();}
};
}

//typedef string hash_t;
typedef hash512_t Id;
typedef int64_t ts_t; //time_t, different on different platforms
typedef uint64_t peer_id_t;

#define DEFAULT_LISTEN_PORT 8453
#define DEFAULT_UI_LISTEN_PORT 8888
#define ID_SIZE 64

#define DEFAULT_DATABASE_PATH "test.db"

// c++14
#include <shared_mutex>
typedef std::shared_timed_mutex rw_mutex;
typedef std::shared_lock<std::shared_timed_mutex> r_lock;
typedef std::unique_lock<std::shared_timed_mutex> w_lock;

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

#endif // GLOB_H
