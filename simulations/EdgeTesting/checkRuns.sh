#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_HOME/src/:
../../Simcan2Cloud -x experiment -c omnetpp.ini -u Cmdenv -n $NEDPATH $*
