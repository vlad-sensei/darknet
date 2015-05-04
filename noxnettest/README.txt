To run any systemtest, make sure you are running on a UNIX system and have python installed.

IMPORTANT! You must also change the path in file 'build_src_path.txt'
to match your src folder in build path. 

To run a test, navigate to the test folder in a terminal and type
	'python <filename>'
where <filename> is replaced with the testfile you want to run.

The 'noxnet_systemtest_master.py' file will run all tests added to the file, all in a row, one after another.

The 'noxnet_systemtest.py' file is not a test itself, but is the module that executes the tests 
given in the test data.

If a test stops right after the line '====== BEGIN TESTING ======' for more than 5-10 seconds,
you should abort the test.


