#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_HOME/src/:
gdb --args ../../Simcan2Cloud_dbg -r 0 -u Cmdenv -n $NEDPATH $*