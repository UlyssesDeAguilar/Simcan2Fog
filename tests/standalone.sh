NEDPATH=$SIMCAN_HOME/src:$INET_HOME/src/:../modules
# valgrind --leak-check=yes
# cd work;  valgrind -q --leak-check=yes --vgdb-error=0 ./testing_dbg -r 0 -u Cmdenv -n $NEDPATH switch/omnetpp.ini
cd work;  valgrind -q --leak-check=yes --vgdb-error=0 ./testing_dbg -r 0 -u Cmdenv -n $NEDPATH NetworkStack/omnetpp.ini