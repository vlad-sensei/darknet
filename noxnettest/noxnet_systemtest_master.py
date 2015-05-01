import noxnet_systemtest_single as nst_single
import noxnet_systemtest_multi as nst_multi
from noxnet_systemtest import SUCCESS

def main():

    test_results = []

    print "+---------------------------------------+"
    print "|             Starting test             |"
    print "|                                       |"
    print "|       Single daemon and client        |"
    print "|                                       |"
    print "+---------------------------------------+"
    
    test_results.append(nst_single.main())

    print ""

    print "+---------------------------------------+"
    print "|             Starting test             |"
    print "|                                       |"
    print "|     Multiple daemons and clients      |"
    print "|                                       |"
    print "+---------------------------------------+"
    
    test_results.append(nst_multi.main())

    print ""
    print "===== Test suite results ====="
    for i in range(len(test_results)):
        result = test_results[i]
        if result == SUCCESS:
            print "Test suite {} result: SUCCESS".format(i+1)
        else:
            print "Test suite {} result: FAILED".format(i+1)

    print "=============================="

if __name__ ==  "__main__":
    main()
