#ifndef GLOB_H
#define GLOB_H

// me no like 'using' from header files (especially not std) -R
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
  inline hash512_t(byte value[]){memcpy(data, value, sizeof(uint64_t)*8);}
  inline bool operator== (const hash512_t& other)const {return  !memcmp(data, other.data, sizeof(data));}
  inline size_t std_hash() const {return data[0]^data[1]^data[2]^data[3]^data[4]^data[5]^data[6]^data[7];}
  friend void operator << (ostream& os, const hash512_t& h);
  //TODO: remove this tmp_set_data
  void tmp_set_data(){
    data[0]=0xf628a784c8a4b793;
    data[1]=0x4be0e3e91d528b43;
    data[2]=0x237126807c4810a6;
    data[3]=0xc306f3ff061eba46;
    data[4]=0x7c77f583687c7637;
    data[5]=0x90f7e6119de2e6b6;
    data[6]=0xccea4b7e1e87931a;
    data[7]=0x0c09fee3b9001ee5;
  }

  inline bool set_data(uint64_t d, unsigned pos){
    if(pos <= 7){
      data[pos] = d;
      return true;
    }else{
      return false;
    }
  }

  inline string to_string() const{
    CryptoPP::HexEncoder encoder;
    string output;
    encoder.Attach( new CryptoPP::StringSink( output ) );
    encoder.Put( (unsigned char*) data, sizeof(data) );
    encoder.MessageEnd();

    return output;;
  }

private:
  uint64_t data[8];
};

inline void operator << (ostream& os, const hash512_t& h){
  os << h.to_string();
}

namespace std {
template<> struct hash<hash512_t>{
  inline size_t operator()(const hash512_t& value) const{ return value.std_hash();}
};
}

const hash512_t NULL_ID = hash512_t();


//typedef string hash_t;
typedef hash512_t Id;
typedef int64_t ts_t; //time_t, different on different platforms
typedef uint64_t peer_id_t;
typedef unsigned char byte;
typedef uint64_t file_size_t;

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

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif


//static uint16_t PORT=DEFAULT_LISTEN_PORT;
static string DATABASE_PATH=DEFAULT_DATABASE_PATH;
static string ARENA_PATH=DEFAULT_ARENA_PATH;

static uint16_t VARIABLE_IS_NOT_USED PORT=DEFAULT_LISTEN_PORT;
//static uint16_t VARIABLE_IS_NOT_USED UI_PORT=DEFAULT_UI_LISTEN_PORT;

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

inline uint16_t get_port(){
  return PORT;
}

inline string get_database_path(){
  return DATABASE_PATH;
}

inline string get_arena_path(){
  return ARENA_PATH;
}


#endif // GLOB_H
