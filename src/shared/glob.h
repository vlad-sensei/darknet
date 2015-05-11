#ifndef GLOB_H
#define GLOB_H

using namespace std;

#include <string>
#include <cstring>
#include <cinttypes>
#include <iostream>
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"
#include <iomanip>

/* Example usage:
  Id id1(""), id2("The quick brown fox jumps over the lazy dog"), id3("The quick brown fox jumps over the lazy dog."), id4("");
  debug("Ids: \n%s\n%s\n%s\n%s\n",id1,id2,id3,id4);
  debug("id2==id3 : %s; id1==id4 : %s", id2==id3 ? "true" : "false", id1==id4 ? "true" : "false");
  */

//TODO: actually make glob.cpp to not include cryptopp?
struct hash512_t{
  inline hash512_t() : data {0,0,0,0,0,0,0,0} {}
  explicit inline hash512_t(const string& value){CryptoPP::SHA512().CalculateDigest((byte*)data, (byte*)value.data(), value.size());}
  inline bool operator== (const hash512_t& other)const {return  !memcmp(data, other.data, sizeof(data));}
  inline size_t std_hash() const {return data[0]^data[1]^data[2]^data[3]^data[4]^data[5]^data[6]^data[7];}
  friend ostream& operator << (ostream& os, const hash512_t& h);

  inline string to_string() const;
  bool from_string(const string& id_str);

private:
  uint64_t data[8];
};

inline ostream& operator << (ostream& os, const hash512_t& h){
  os << h.to_string();
  return os;
}

inline string hash512_t::to_string() const{
  CryptoPP::HexEncoder encoder;
  string output;
  encoder.Attach( new CryptoPP::StringSink( output ) );
  encoder.Put( (unsigned char*) data, sizeof(data) );
  encoder.MessageEnd();
  return output;
}

inline bool hash512_t::from_string(const string &id_str){
  if(id_str.size() != sizeof(hash512_t)*2) return false;
  try{
    string decoded;

    CryptoPP::HexDecoder decoder;

    decoder.Attach( new CryptoPP::StringSink( decoded ) );
    decoder.Put( (byte*)id_str.data(), id_str.size() );
    decoder.MessageEnd();

    memcpy(&data,decoded.data(),sizeof(hash512_t));
  } catch (const exception&) {
    return false;
  }
  return true;
}


namespace std {
template<> struct hash<hash512_t>{
  inline size_t operator()(const hash512_t& value) const{ return value.std_hash();}
};
}

//const hash512_t NULL_ID = hash512_t();


//typedef string hash_t;
typedef hash512_t Id;
typedef int64_t ts_t; //time_t, different on different platforms
typedef uint64_t peer_id_t;
typedef unsigned char byte;
typedef uint64_t file_size_t;
typedef uint32_t ip_t;


#define TEST

#ifdef __linux__
#define NCURSES
#endif //__linux__

#ifdef TEST
#ifdef NCURSES
#undef NCURSES
#endif //NCURSES
#endif //TEST

#define DEFAULT_LISTEN_PORT 8453
#define DEFAULT_UI_LISTEN_PORT 8888
#define ID_SIZE 64
#define METAHEAD_SIZE 1024
#define SYNC false
#define SYNC_PERIOD 30
#define N_SHARED_METAHEADS 100
#define DEFAULT_DATABASE_PATH "database.db"
#define DEFAULT_ARENA_PATH "/tmp/arena"
#define DEFAULT_ARENA_SLOT_NUM 200
#define DEAFULT_PEER_REQ_COUNT 10



#if __cplusplus <= 201103L
#include <boost/thread.hpp>
typedef boost::defer_lock_t defer_lock_type;
typedef boost::shared_mutex rw_mutex;
typedef boost::shared_lock<boost::shared_mutex> r_lock;
typedef boost::unique_lock<boost::shared_mutex> w_lock;
#else
// c++14
#include <shared_mutex>
typedef std::defer_lock_t defer_lock_type;
typedef std::shared_timed_mutex rw_mutex;
typedef std::shared_lock<std::shared_timed_mutex> r_lock;
typedef std::unique_lock<std::shared_timed_mutex> w_lock;

#endif

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
  //TODO: getting time causes race condition. synchronize
  /*
  time_t now_ = time(nullptr);
  unique_ptr<struct tm> now = unique_ptr<struct tm>(new struct tm);
  localtime_r(&now_, now.get());
  string datestr = asctime(now.get());
  datestr.pop_back();
  cout << "["<<datestr << "] ";
  */
  safe_printf(fmt.c_str(),args...);
  cout << "\n";
#endif //DEBUG_ON

}

//Temporary debug function used for inspecting long strings
template<size_t size>
array<char, size> debug_str(const string& str){
  array<char, size> arr;
  memcpy(arr.data(), str.data(), size);
  return arr;
}

#endif // GLOB_H
