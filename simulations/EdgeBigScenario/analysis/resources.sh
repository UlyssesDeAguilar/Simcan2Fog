#!/bin/bash
RDIR=../results

# TargetDc
# Focus only onto the dc allocated resources
## scavetool query -l -f 'attr:configname("targetDc") AND module("**.dc.resourceManager") AND name("allocated*:vector")' $RDIR/*.vec $RDIR/*.sca
#scavetool export -f 'attr:configname("targetDc") AND module("**.dc.resourceManager") AND name("allocated*:vector")' -F CSV-R -o targetDc.csv $RDIR/*.vec

# TargetFog
# Export all fog resources - might be interesing to isolate by region
## scavetool query -l -f 'attr:configname("targetFog") AND module("**.fg*.resourceManager") AND name("allocated*:vector")' $RDIR/*.vec $RDIR/*.sca
scavetool export -f 'attr:configname("targetFog") AND module("**.fg*.resourceManager") AND name("allocated*:vector")' -F CSV-R -o targetFog.csv $RDIR/*.vec