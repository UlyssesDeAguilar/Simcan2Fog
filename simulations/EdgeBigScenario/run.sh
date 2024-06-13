#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_HOME/src/:
../../Simcan2Cloud -c targetFog -u Cmdenv -n $NEDPATH $*