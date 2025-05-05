#!/bin/bash
RDIR=../results

# TargetDc
# Focus only onto the dc allocated resources
## opp_scavetool query -f 'module=~"**.dc.resourceManager" AND name=~"allocated*:vector" AND runattr:configname=~targetDc' $RDIR/*.vec

opp_scavetool export -f 'module=~"**.dc.resourceManager" AND name=~"allocated*:vector" AND runattr:configname=~targetDc' -F CSV-R -o targetDcRes.csv $RDIR/*.vec

# TargetFog
# Export all fog resources - might be interesing to isolate by region
## opp_scavetool query -f 'module=~"**.fg.resourceManager" AND name=~"allocated*:vector" AND runattr:configname=~targetFog' $RDIR/*.vec

opp_scavetool export -f 'module=~"**.fg.resourceManager" AND name=~"allocated*:vector" AND runattr:configname=~targetFog' -F CSV-R -o targetFogRes.csv $RDIR/*.vec
