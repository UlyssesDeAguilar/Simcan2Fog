#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_ROOT/src/:
echo $1
../../Simcan2Fog -f omnetpp.ini -c targetDc -u Cmdenv -n $NEDPATH $*

