#!/bin/sh
NEDPATH=$SIMCAN_HOME/src:$SIMCAN_HOME/simulations/:$INET_HOME/src/:
../../Simcan2Cloud -c FogShowcase_100 -r 0 -u Cmdenv -n $NEDPATH $*