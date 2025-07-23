# Testing
The objective of this section is to introduce the reader (and maybe future contributor) to the tools and techniques for testing the simulator. 

## Fundamentals
The current way of testing the simulator is using the tools already provided by OMNeT++. It essentially boils down into designing networks, modules and experiments that generate an output (std::out) that will be compared with an expected output declared in the test file. This methodology allows for both unit and module testing under the same techniques.

## Where to start
Navigate to the folder ```Simcan2Fog/tests```. There you will find the following directories on a clean install:

* ```modules/```: Contains the specificifically developed modules for testing
* ```networks/```: Folder with networks that can be reused in different tests
* ```protocol/```: Includes the tests for protocol related modules like the DNS servers, the SimcanMessaqueQueue and the simplified Switch model.
* ```data/```: The tests for the DataEngines
* ```unit/```: The unit tests are kept here

Also you'll have at your disposal the scripts:
* ```gentest.sh```: Renders the test/s needed files based on the specification on the test file and compiles the executable in order to run the test. At this time the folder ```work/``` will contain the aforementioned artifacts.
* ```runtest.sh```: Runs the specified test. It **must** be run after ```gentest.sh```.
* ```recompile.sh```: Helps with recompiling both the Simcan2Fog library and the test executable.
* ```standalone.sh```: Launches a valgrind instance of the specified test. It includes the option ```--vgdb-error=0``` in order to connect it with gdb, so you'll have the absolute best combination of debugging tools.

## An example
### Generating all test cases
```
(.s2f) ulysses@tmach:~/omnetpp-6.1/Simcan2Fog/tests$ ./gentest.sh unit/* protocol/* data/*
Targets:  unit/TDnsTree.test protocol/DnsDb.test protocol/DnsResolution.test protocol/MessageQueue.test protocol/switchtest.test data/simschemajson.test data/simschemamysql.test
Namespace(mode='gen', verbose=2, workdir='./work', shell='sh', testprogram='', filenames=['unit/TDnsTree.test', 'protocol/DnsDb.test', 'protocol/DnsResolution.test', 'protocol/MessageQueue.test', 'protocol/switchtest.test', 'data/simschemajson.test', 'data/simschemamysql.test'], extraargs=None)
opp_test.py: extracting files from *.test files into ./work...
  parsing `unit/TDnsTree.test' file

  testname for `unit/TDnsTree.test' is TDnsTree

  generating files for `unit/TDnsTree.test':
  writing `./work/TDnsTree/package.ned'
  writing `./work/TDnsTree/test.ned'
  writing `./work/TDnsTree/test.cc'
  writing `./work/TDnsTree/_defaults.ini'
  parsing `protocol/DnsDb.test' file

  testname for `protocol/DnsDb.test' is DnsDb

  generating files for `protocol/DnsDb.test':
  writing `./work/DnsDb/package.ned'
  writing `./work/DnsDb/omnetpp.ini'
  writing `./work/DnsDb/_defaults.ini'
  writing `./work/DnsDb/TestDnsDb.ned'
  writing `./work/DnsDb/zones.json'
  parsing `protocol/DnsResolution.test' file

  testname for `protocol/DnsResolution.test' is DnsResolution

  generating files for `protocol/DnsResolution.test':
  writing `./work/DnsResolution/package.ned'
  writing `./work/DnsResolution/omnetpp.ini'
  writing `./work/DnsResolution/_defaults.ini'
  writing `./work/DnsResolution/zones.json'
  parsing `protocol/MessageQueue.test' file

  testname for `protocol/MessageQueue.test' is MessageQueue

  generating files for `protocol/MessageQueue.test':
  writing `./work/MessageQueue/package.ned'
  writing `./work/MessageQueue/omnetpp.ini'
  writing `./work/MessageQueue/_defaults.ini'
  parsing `protocol/switchtest.test' file

  testname for `protocol/switchtest.test' is switchtest

  generating files for `protocol/switchtest.test':
  writing `./work/switchtest/package.ned'
  writing `./work/switchtest/omnetpp.ini'
  writing `./work/switchtest/_defaults.ini'
  writing `./work/switchtest/EchoNet1.ned'
  parsing `data/simschemajson.test' file

  testname for `data/simschemajson.test' is simschemajson

  generating files for `data/simschemajson.test':
  writing `./work/simschemajson/package.ned'
  writing `./work/simschemajson/omnetpp.ini'
  writing `./work/simschemajson/_defaults.ini'
  writing `./work/simschemajson/apps.json'
  writing `./work/simschemajson/vms.json'
  writing `./work/simschemajson/slas.json'
  writing `./work/simschemajson/users.json'
  writing `./work/simschemajson/experiments.json'
  parsing `data/simschemamysql.test' file

  testname for `data/simschemamysql.test' is simschemamysql

  generating files for `data/simschemamysql.test':
  writing `./work/simschemamysql/package.ned'
  writing `./work/simschemamysql/omnetpp.ini'
  writing `./work/simschemamysql/_defaults.ini'
Creating Makefile in /home/ulysses/omnetpp-6.1/Simcan2Fog/tests/work...

modules/DcDriver/DcDriver.cc
modules/dns/DnsTestClient.cc
modules/echo/Echo.cc
modules/mq/MQDriver.cc
modules/querier/DataManagerQuerier.cc
TDnsTree/test.cc
Creating executable: cmp/clang-debug/tests/work/testing_dbg
```
## Running all test cases at the same time
```
(.s2f) ulysses@tmach:~/omnetpp-6.1/Simcan2Fog/tests$ ./runtest.sh unit/* protocol/* data/*
opp_test: running tests using testing_dbg...
*** unit/TDnsTree.test: PASS
*** protocol/DnsDb.test: PASS
*** protocol/DnsResolution.test: PASS
*** protocol/MessageQueue.test: PASS
*** protocol/switchtest.test: PASS
*** data/simschemajson.test: PASS
*** data/simschemamysql.test: EXPECTEDFAIL (test program returned exit code 1)
stdout:
>>>>OMNeT++ Discrete Event Simulation  (C) 1992-2024 Andras Varga, OpenSim Ltd.
Version: 6.1, build: 241008-f7568267cd, edition: Academic Public License -- NOT FOR COMMERCIAL USE
See the license for distribution terms and warranty disclaimer

Setting up Cmdenv...

Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/src:  104
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/src/:  1187
Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/tests/modules/:  7
Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/tests/networks/:  4
Loading NED files from .:  1

Preparing for running configuration General, run #0...
Assigned runID=General-0-20250722-21:15:22-14904

End.
<<<<
stderr:
>>>>
<!> Error: Network 'Unconnected' not found, check .ini and .ned files
<<<<
CMD: (cd ./work/simschemamysql && ../testing_dbg -u Cmdenv -n /home/ulysses/omnetpp-6.1/Simcan2Fog/src:/home/ulysses/omnetpp-6.1/inet4.5/src/:/home/ulysses/omnetpp-6.1/Simcan2Fog/tests/modules/:/home/ulysses/omnetpp-6.1/Simcan2Fog/tests/networks/:.  omnetpp.ini _defaults.ini "$@")
========================================
EXPECTEDFAIL: data/simschemamysql.test
========================================
PASS: 6   FAIL: 0   ERROR: 0   EXPECTEDFAIL: 1   SKIPPED: 0

Aggregate result: PASS
```