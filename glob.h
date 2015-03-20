#ifndef GLOB_H
#define GLOB_H

// me no like 'using' from header files (especially not std) -R
using namespace std;

#include <string>
#include <cinttypes>
#include <cstdint>

typedef string hash_t;
typedef hash_t Id;
typedef int64_t ts_t; //time_t, different on different platforms
typedef uint64_t peer_id_t;

#define LISTEN_PORT 8453
#define ID_SIZE 64

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

struct Id_{
private:
        uint64_t data[8];
public:
        inline bool operator== (const Id_& other){
            for (int i = 0; i < 8; i++){
                if(data[i] != other.data[i]){
                    return false;
                }
            }
            return true;
        }
        inline uint64_t std_hash() const {return data[0]^data[1]^data[2]^data[3]^data[4]^data[5]^data[6]^data[7];}
};

namespace std {
    template<>
    struct hash<Id_>{
        std::size_t operator()(const Id_& value) const{
            return value.std_hash(); //What should this return??
        }
    };
}

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
