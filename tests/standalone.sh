NEDPATH=$SIMCAN_HOME/src:$INET_ROOT/src/:../modules:./switchtest
# valgrind --leak-check=yes
# cd work;  valgrind -q --leak-check=yes --vgdb-error=0 ./testing_dbg -r 0 -u Cmdenv -n $NEDPATH switch/omnetpp.ini
cd work;  valgrind -q --leak-check=yes --vgdb-error=0 ./testing_dbg -r 0 -u Cmdenv -n $NEDPATH switchtest/omnetpp.ini