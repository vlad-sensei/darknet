####################################################
#
#  noxnet_systemtest.py
#
#---------------------------------------------------
#
# Module providing methods for making systemtest
#   on Noxnet. This program does NOT perform any
#   validation of provided data to methods. It
#   is up to the user to provide correct data or
#   this program will likely CRASH and leave open
#   (sub)processes RUNNING on computer.
#
#   The following UNIX command could be used to 
#   detect and kill these processes, if
#   neccessary. It will ONLY kill processes
#   matching 'noxnet' in name.
#
#    kill $(ps aux | grep '[n]oxnet' | awk '{print $2}')
#
#   Reference: http://stackoverflow.com/questions/3510673/find-and-kill-a-process-in-one-line-using-bash-and-regex
#
####################################################

from subprocess import PIPE, Popen
from time import sleep
import fcntl  # UNIX only!!
import os
import sys
import re
import string

SUCCESS_COMMAND_PREFIX = "(<>)"
FAILED = False
SUCCESS = True

# Used to simulate enum, since Enum requires Python 3.4
# Holds constants for test_data tuples
class Test_data_index:
    CMD       = 0
    REQ_ANS   = 1
    CLIENT_NO = 2
    EXP_OUT   = 3
    ERR_MSG   = 4
    DELAY     = 5


################################################
#
# Systemtest class - the testing main class
#
################################################

'''
 Variable <test_data> should contain a list of tuples,
   one tuple per test
 Each tuple consists of (
    command to execute,
    Index <one-indexed> of answer of previous test.
      Use 0 if previous answer is not required.
      <command should have {} where answer should be>,
    client number <one-indexed> to run cmd,
    expected answer of executed command (use '\n' if
      output contains newlines),
    error message if command failed,
    delay before continuing to next test, in seconds, 
      if test succeeded. Could be a real number. Use
      0 for no delay
 )
'''
class Systemtest(object):
    'Systemtest class used to test Noxnet'

    def __init__(self,
                 test_data,
                 daemon_count = 1,
                 client_count = 1,
                 daemon_init_wait = 5,
                 client_init_wait = 2):
        self.test_data = test_data
        self.daemon_count = daemon_count
        self.client_count = client_count
        self.daemon_init_wait = daemon_init_wait
        self.client_init_wait = client_init_wait

        self.test_count = 0

        self.find_build_path()

    def main(self):
        daemon_processes, client_processes = self.init()
        result = self.run(daemon_processes, client_processes)

        if result == SUCCESS:
            print "-----------------------------------------------"
            print "Test result: SUCCESS"
            print "-----------------------------------------------"
        else:
            print ""
            print "TEST ABORTED"
            print ""
            print "-----------------------------------------------"
            print "Test result: FAILURE"
            print "-----------------------------------------------"
        

        print ""
        print "<Test complete>"
        print "Finishing test..."
            
        # terminating processes also closes pipes
        for process in (client_processes+daemon_processes):
            process.terminate()

        return result


    '''
    Assume:
      daemon 1 connected to client 1
      daemon 2 connected to client 2
      ...
    '''
    def run(self, daemons, clients):

        print ""
        print "====== BEGIN TESTING ======"

        ans_list = []
        for test_tuple in self.test_data:
            # CLIENT_NO is one-indexed
            client = clients[test_tuple[Test_data_index.CLIENT_NO]-1]
            req_ans = test_tuple[Test_data_index.REQ_ANS]
            if req_ans == 0:
                result, ans = self.do_and_check_command(test_tuple, client, "")
            else:
                result, ans = self.do_and_check_command(test_tuple, client, ans_list[req_ans-1])

            ans_list.append(ans)

            if result == FAILED:
                return FAILED

            delay = test_tuple[Test_data_index.DELAY]
            if delay > 0:
                sleep(delay)
            
        print "==== TESTING FINISHED ===="

        return SUCCESS


    def do_and_check_command(self, test, client, prev_ans):
        
        self.inc_test_count()

        # -1 due to array is zero-indexed
        cmd = test[Test_data_index.CMD]
        exp_out = test[Test_data_index.EXP_OUT]
        cmd_err_msg = test[Test_data_index.ERR_MSG]
        if test[Test_data_index.REQ_ANS]:
            cmd = cmd.format(prev_ans)

        result, ans = client.send_command(cmd)

        if result == FAILED:
            # Command failed
            print "During test {}".format(self.get_test_count())
            print ans
            print cmd_err_msg.format(cmd)
            return FAILED, ""

        # One line if, like ( ? : ) in C/C++/Java. "<expr1> if <cond> else <expr2>"
        # Returns expr1 if cond is True, otherwise expr2
        test_result = SUCCESS if (ans == exp_out) else FAILED

        self.print_testresult(test_result, ans, exp_out)

        return test_result, ans


    def print_testresult(self, result, ans, exp_out):

        if result == FAILED:
            print "Test {} failed".format(self.get_test_count())
            print "  => Command result mismatch."
            print "     Expected [{}]".format(exp_out)
            print "     Got      [{}]".format(ans)            
        else:
            print "Test {} succeded".format(self.get_test_count())

    def inc_test_count(self):
        self.test_count += 1

    def get_test_count(self):
        return self.test_count

    """ If 'build_src_path.txt' exists, interprets its contents as the build dir.
    Otherwise, tries to find the build dir """
    def find_build_path(self):
        if os.path.exists("build_src_path.txt"):
            print "found build_src_path.txt"
            f = open("build_src_path.txt","r")
            build_src_path = f.read()        
            f.close()
            # Remove newlines, which might be hard to see manually
            self.path = build_src_path.strip('\n')        
        else:
            build_root = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
            dirs = os.listdir(build_root)
            print "d " + string.join(dirs)
            
            build_dir = re.findall(r"build-noxnet.*?Debug", string.join(dirs))[0]
            
            if build_dir:
                self.path = os.path.join(build_root, build_dir, 'src/')
                print "found build dir: " + self.path
            else:
                print "Couldn't find build dir, exiting ..."
                sys.exit(1)

    def init(self):

        print "========= INITIALIZTION ========="

        daemon_processes = []
        client_processes = []

        print "Start daemons..."
        for i in range(1,self.daemon_count+1):        
            print "Daemon {} created".format(i)
            daemon_processes.append(Daemon(self.path, i))

        print "Sleep {} s to wait for daemon to init...".format(self.daemon_init_wait)
        sleep(self.daemon_init_wait)

        print "Done sleeping."
        print "Start clients..."
        for i in range(1,self.client_count+1):
            print "Client {} created".format(i)
            client_processes.append(Client(self.path, i))

        print "Sleep {} s to wait for client to init...".format(self.client_init_wait)
        sleep(self.client_init_wait)
        print "Done sleeping."

        print "Clearing pipes..."
        # Get rid of potential output from initialization of processes
        for pr in (daemon_processes+client_processes):
            # Read but ignore content => clears buffer
            pr.read_all()

        print "Done clearing."

        print "===== INITIALIZATION COMPLETE ====="

        return daemon_processes, client_processes


################################################
#
# Testprocess class - helper class for 
#   subprocess handling
#
################################################

class Testprocess(object):
    'Wrapper for more convenient use of subprocesses'

    def __init__(self, process):
        self.process = process

    # Blocking if nothing to read
    def readline(self):
        return self.process.stdout.readline()

    # Ugly way to do this, but didn't find any better
    def read_all(self):
        # Read default flag of stdout
        old_flag = fcntl.fcntl(self.process.stdout.fileno(), fcntl.F_GETFL)

        # Makes reading of stdout non-blocking, raises IOError instead if nothing to read
        fcntl.fcntl(self.process.stdout.fileno(), fcntl.F_SETFL, os.O_NONBLOCK)

        content = ""

        # Read all content, until error => clears the buffer
        while True:
            try:
                '''
                 read() reads all content unil EOF (which we don't have)
                
                Are you here because of backtrace after Crtl+C an execution of a test?
                -> Read below

                 Sometimes the execution freezes at following statement. This is
                 NOT because read() blocks. It's because a PIPE contains an 
                 infinite amount of empty characters, '' . This becomes an 
                 infinite loop and the program seemingly freezes at read(). I
                 don't know the cause of this, but one solution could be to 
                 restart computer or run another test in between. Trying again
                 might work, but the problem is likely to occur again.
                '''
                content += self.process.stdout.read()

            except IOError:
                # No more to read => finished
                break

        # Restore default flags, so that reading blocks again
        fcntl.fcntl(self.process.stdout.fileno(), fcntl.F_SETFL, old_flag & ~os.O_NONBLOCK)

        return content

    def terminate(self):
        self.process.terminate()



################################################
#
# Daemon class - subclass of Testprocess
#   used for creating daemons
#
################################################

class Daemon(Testprocess):

    def __init__(self, path, number):
        # This is UNIX only test, so /tmp should exist. Also '/' should work
        proc = Popen([path+"daemon/daemon", "-c",str(9000-number),"-d","database"+str(number)+".db","-p",str(8000-number),"-a","/tmp/arena"+str(number)],stdout=PIPE)
        print "   with client port {} and out port {}".format(9000-number,8000-number)

        # Call super constructor
        super(Daemon, self).__init__(proc)



################################################
#
# Client class - subclass of Testprocess
#   used for creating clients and some help
#   methods for writing and reading commands
#
################################################

class Client(Testprocess):

    def __init__(self, path, number):
        # This is UNIX only test, so '/' should work
        proc = Popen([path+"client_console/client_console", str(9000-number)], stdin=PIPE, stdout=PIPE, bufsize=1)
        print "   with port {}".format(9000-number)
        # Call super constructor
        super(Client, self).__init__(proc)

    def write(self, msg):
        print >>self.process.stdin, msg

    def send_command(self, cmd):
        self.write(cmd)

        # Call parent (Testprocess) readline() method
        ret_str = super(Client,self).readline()

        # Read the rest of the output, if any, now that we
        #  know that we got answer
        ret_str += super(Client,self).read_all()

        # Removes unprintable characters (though some might be printable
        #   in Unicode) in case we read some junk.
        ans = ''.join(char for char in ret_str if char in string.printable)

        if ans.startswith(SUCCESS_COMMAND_PREFIX):
            # +1 for whitespace after prefix, -1 to remove newline at end
            ans = ans[len(SUCCESS_COMMAND_PREFIX)+1:-1]
            return SUCCESS, ans
        elif len(ans) > 0:
            err_msg = "*** Sent command [{}] failed, got [{}]".format(cmd,ans)
            return FAILED, err_msg
        else:
            err_msg = "*** No return value from command [{}]".format(cmd)
            return FAILED, err_msg


###########################################################################


# Put an empty test sequence to test itself
# Will not run if imported as module from other program
if __name__ ==  "__main__":

     if len(sys.argv) != 2:
         print "Must supply path to build src folder as argument"
     else:
         systemtest = Systemtest(sys.argv[1],"")
         systemtest.main()
