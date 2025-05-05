#!/bin/bash
RDIR=../results

# TargetDc -- Focus onto the single controller
# opp_scavetool query -f 'module=~"**.app" AND roundTripTime:vector AND runattr:configname=~targetDc' $RDIR/*.vec $RDIR/*.sca
# opp_scavetool query -f 'module=~"**.app" AND roundTripTime:vector AND runattr:configname=~targetFog' $RDIR/*.vec $RDIR/*.sca

opp_scavetool export -f 'module=~"**.app" AND roundTripTime:vector AND runattr:configname=~targetDc' -F CSV-R -o targetDcDelay.csv $RDIR/*.vec

opp_scavetool export -f 'module=~"**.app" AND roundTripTime:vector AND runattr:configname=~targetFog' -F CSV-R -o targetFogDelay.csv $RDIR/*.vec
