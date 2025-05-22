NEDPATH=$SIMCAN_HOME/src:$INET_ROOT/src/:$SIMCAN_HOME/tests/modules/:$SIMCAN_HOME/tests/networks/
# valgrind --leak-check=yes
# cd work;  valgrind -q --leak-check=yes --vgdb-error=0 ./testing_dbg -r 0 -u Cmdenv -n $NEDPATH switch/omnetpp.ini
valgrind -q --leak-check=yes --vgdb-error=0 $SIMCAN_HOME/tests/work/testing_dbg --debug-on-errors=true -r 0 -u Cmdenv -n $NEDPATH $SIMCAN_HOME/tests/work/$1/omnetpp.ini