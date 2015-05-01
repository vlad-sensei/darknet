from noxnet_systemtest import *

TEST_DATA = [
    # ---- Test 1 ----
    ("up testfile1.txt test1",0,1,"5842EE70ADE7FA0AF5B89457E341D2C18EBB41F4FFCEF1B38B0E072A58B537038C9BB7EFC739096B7B9BC6A3E96792068B18DDBE22F04E6736BF149A399F9075","*** Upload 1 failed",0),

    # ---- Test 2 ----
    ("do {}",1,1,"D7EB9416FA76660782BEDD96D88A1E53555A7515EE77DB0B28F3F692416F183F30821964EF18593900D7F324B21845D80C336DC8C844CB574641F0B343C11840","*** Download failed",0),

    # ---- Test 3 ----
    ("up testfile2.pdf test2",0,1,"E3AADFBAA0BE2D3A24FA8030A984C03070CB4B8ED534DA3DAB70CC5C207404C0611622A30D440FCAC1F82ABCA5430BA2852E61AEC9A9A37EAE1EEB644F8C45B7","*** Upload 2 failed",0),

    # ---- Test 4 ----
    ("as {} systemtest_filename_1",2,1,"systemtest_filename_1","*** Assemble 1 failed",0),

    # ---- Test 5 ----
    ("se test",0,1,"[mid 5842EE70ADE7FA0AF5B89457E341D2C18EBB41F4FFCEF1B38B0E072A58B537038C9BB7EFC739096B7B9BC6A3E96792068B18DDBE22F04E6736BF149A399F9075][tags test1][bid D7EB9416FA76660782BEDD96D88A1E53555A7515EE77DB0B28F3F692416F183F30821964EF18593900D7F324B21845D80C336DC8C844CB574641F0B343C11840]\n[mid E3AADFBAA0BE2D3A24FA8030A984C03070CB4B8ED534DA3DAB70CC5C207404C0611622A30D440FCAC1F82ABCA5430BA2852E61AEC9A9A37EAE1EEB644F8C45B7][tags test2][bid BDBD155F6030531DC8383240D1A533FF6B0380D282DC07F843996455F460CE604D767CD1FF742943A8E2AAB4CB93FF3DA4227C766E7894E6EBC18F8C9DEFAAEE]","*** Search 1 failed",0)
]

def main():
    systemtest = Systemtest("/home/cn/Documents/Qt/build-noxnet-GCC-Debug/src/",TEST_DATA)
    return systemtest.main()


if __name__ ==  "__main__":
    main()
