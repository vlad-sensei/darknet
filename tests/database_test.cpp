#include <unittest++/UnitTest++.h>
#include "database.h"
#include <vector>

Database d;

class Database_test: public Database{
public:
  void get_random_metaheads(const int n, vector<Metahead>& metaheads){
    get_random_metaheads(n, metaheads);
  }
};

//add
TEST(get_random_metaheads){
  //Don't know how to test this function
  const int LIMIT = 10;
  Database_test db;
  vector<Metahead> v;
  db.get_random_metaheads(LIMIT, v);

  cout << "Getting " << LIMIT << "random metaheads" << endl;

  for (const auto& e: v){
      cout << "Metahead: " << e.tags << endl;
    }
}
