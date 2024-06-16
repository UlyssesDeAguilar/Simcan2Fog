#!/bin/bash
RDIR=../results

# TargetDc -- Focus onto the single controller
#scavetool query -l -f 'attr:configname("targetDc") AND module("**.app") AND name("endToEndDelay*")' $RDIR/*.vec $RDIR/*.sca
scavetool export -f 'attr:configname("targetDc") AND module("**.app") AND name(roundTripTime:vector)' -F CSV-R -o targetDcDelay.csv $RDIR/*.vec
scavetool export -f 'attr:configname("targetFog") AND module("**.app") AND name(roundTripTime:vector)' -F CSV-R -o targetFogDelay.csv $RDIR/*.vec
