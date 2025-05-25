#!/bin/sh

if [ "$#" -lt 1 ]; then
    echo "Illegal number of parameters. Add the configuration name"
    exit 1
fi

if [ "$#" -gt 1 ]; then
	if [ "$2" = "--dbg" ]; then
		echo "Debugging ..."
		SUFFIX="_dbg"
	else 
		echo "Illegal second argument"
		exit 1
	fi
fi


NEDPATH=$SIMCAN_HOME/src/:$SIMCAN_HOME/simulations/:$INET_ROOT/src/:
Simcan2Fog$SUFFIX -c $1 -u Cmdenv -n $NEDPATH 
