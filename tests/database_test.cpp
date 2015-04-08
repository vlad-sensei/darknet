#include <unittest++/UnitTest++.h>
#include "database.h"
#include <vector>

//string thats 1024 chars long
static const string lorem = string(
    "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor"
    "invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et"
    "accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus"
    "est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr,  sed"
    "diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua."
    "At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata"
    "sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr,"
    "sued diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua."
    "At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata"
    "sanctus est Lorem ipsum dolor sit amet. Duis autem vel eum iriure dolor in hendrerit in vulputate velit"
    "esse molestie consequat, vel illum dolore eu feugiat nulla facilisis aaaaaaaaaaa"
    );
class Database_test: public Database{
public:
  void get_random_metaheads(const int n, vector<Metahead>& metaheads){
    Database::get_random_metaheads(n, metaheads);
  }
  void add_metahead(const Metahead & metahead){
    Database::add_metahead(metahead);
  }
};

Database_test db;

TEST(db_insertion){
  vector<Metahead> metaheads = {
    Metahead(Id("wj01lEv8X4PwlP2Rciyn"), "batman"),
    Metahead(Id("InFDcKbYZD7NDISCPkGP"), "catwoman"),
    Metahead(Id("p891dIuOJuSL7em7IVJ1"), "superman"),
    Metahead(Id("DHHI1eocea3Mv4lPIC0O"), "donald duck"),
    Metahead(Id("Bo4NL9XjPZtVgsC56UYy"), "mickey mouse"),
    Metahead(Id("BxV6KzJHk1ai70rujntM"), "åäö"),
    Metahead(Id("BspmPmUttYXxXXybUxxf"), "aoeu"),
    Metahead(Id("BLASQmCBq1KZfO8qF107"), "asdf"),
    Metahead(Id("ok0EkyjGTxr4XB2j9sGZ"), "!@#$"),
    Metahead(Id("S2n9ikqkhw1cHFf0ykXh"), "^%&*"),
    Metahead(Id("uOvrF6rIMIU4UCeKQfu9"), "ABCD"),
    Metahead(Id("W6a8wRiDaNCKwuf5o3vW"), "EFGH"),
    Metahead(Id("hglckWwZaCo0Sdh9HXCC"), "SERIOUS SAM"),
    Metahead(Id("CyiHUdofnOAAuc7T0Yql"), lorem),
    Metahead(Id("RniP5GbPsCVqa4kaONa2"), lorem + "t")
  };

  cout << "*** inserting some metaheads ***" << endl;
  for (const auto& e: metaheads){
      cout << "inserting metahead" << endl;
      db.add_metahead(e);
    }
}

TEST(db_random_metaheads){
  const int LIMIT = 10;
  vector<Metahead> v;

  cout << "Getting " << LIMIT << "random metaheads" << endl;

  db.get_random_metaheads(LIMIT, v);

  for (const auto& e: v){
      cout << "Metahead: " << e.tags << endl;
    }
}
