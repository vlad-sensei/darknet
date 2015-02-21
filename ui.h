#ifndef UI_H
#define UI_H

/*
 * UI
 *
 * UI provides user input API which
 * communicate with Core.
 */

#include <memory>
#include <thread>
#include <vector>
#include <sstream>

#include "glob.h"

class UI;
typedef unique_ptr<UI> UI_ptr;

class UI{
public:
  UI();
private:
  void get_raw_input();
  void process_raw_input(string raw_input);
};

#endif // UI_H
