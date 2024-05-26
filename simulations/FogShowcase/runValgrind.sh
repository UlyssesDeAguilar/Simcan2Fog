#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_HOME/src/:
valgrind -q --leak-check=yes --vgdb-error=0 ../../Simcan2Cloud_dbg -c FogShowcase_2 -r 0 -u Cmdenv -n $NEDPATH $*