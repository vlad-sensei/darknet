####################################################
#
#  noxnet_systemtest_multi_2.py
#
#---------------------------------------------------
#
# Tests Synch, Upload, Download and Assemble
#   functions mainly.
#   Uses three daemons and clients. Uploads 
#   downloaded files, i.e. file transfer with 
#   intermediate node.
#
####################################################

from noxnet_systemtest import *

TEST_DATA = [

    # ---- Test 1 ----
    ("co localhost 7998",0,1,"Connected to 'localhost:7998'.","*** Connecting 1 failed",0),

    # ---- Test 2 ----
    ("co localhost 7997",0,2,"Connected to 'localhost:7997'.","*** Connecting 2 failed",0),

    # ---- Test 3 ----
    ("up testfile1.txt test1",0,1,"5842EE70ADE7FA0AF5B89457E341D2C18EBB41F4FFCEF1B38B0E072A58B537038C9BB7EFC739096B7B9BC6A3E96792068B18DDBE22F04E6736BF149A399F9075","*** Upload 1 failed",0),

    # ---- Test 4 ----
    ("sy 1 1",0,2,"Synch started","*** Synch start 1 failed",3),

    # ---- Test 5 ----
    ("sy 0",0,2,"Synch stopped","*** Synch stop 1 failed",0),

    # ---- Test 6 ----
    ("do {}",3,2,"D7EB9416FA76660782BEDD96D88A1E53555A7515EE77DB0B28F3F692416F183F30821964EF18593900D7F324B21845D80C336DC8C844CB574641F0B343C11840","*** Download 1 failed",2),

    # ---- Test 7 ----
    ("as {} newtestfile1",6,2,"newtestfile1","*** Assemble 1 failed",0),

    # ---- Test 8 ---- # assemble ans should contain '.part' too, but doesn't now
    ("up {}.part test2",7,2,"7E6F2E1AA1B65F3009BC3BC7F908977486EF90DA68203CC3FE6EC155540CBE15830EB4ACDB77488046CE602D4CAD376A4A7863B6797FF596A813B63D645F4FB5","Upload 2 failed",0),

    # ---- Test 9 ----
    ("sy 1 1",0,3,"Synch started","*** Synch start 2 failed",3),

    # ---- Test 10 ----
    ("sy 0",0,3,"Synch stopped","*** Synch stop 2 failed",0),

    # ---- Test 11 ----
    ("do {}",8,3,"D7EB9416FA76660782BEDD96D88A1E53555A7515EE77DB0B28F3F692416F183F30821964EF18593900D7F324B21845D80C336DC8C844CB574641F0B343C11840","*** Download 2 failed",2),

    # ---- Test 12 ----
    ("as {} newtestfile2",11,3,"newtestfile2","*** Assemble 2 failed",0)

]

def main():
    systemtest = Systemtest(TEST_DATA,3,3,7,4)
    return systemtest.main()


if __name__ ==  "__main__":
    main()
