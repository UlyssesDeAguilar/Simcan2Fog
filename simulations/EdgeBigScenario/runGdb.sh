#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_HOME/src/:
gdb --args ../../Simcan2Cloud_dbg -c targetDc -u Cmdenv -n $NEDPATH $* --debug-on-errors=true