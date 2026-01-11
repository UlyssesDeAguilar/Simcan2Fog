#NEDPATH=$SIMCAN_HOME/src:$INET_ROOT/src/:$SIMCAN_HOME/tests/modules/:$SIMCAN_HOME/tests/networks/:$SIMCAN_HOME:/tests/work/TDnsHeader/

INET_NEDFOLDERS=$(cat $INET_ROOT/.nedfolders | sed "s|^|$INET_ROOT/|" | tr '\n' ':')
S2F_NEDFOLDERS=$(cat $SIMCAN_HOME/.nedfolders | sed "s|^|$SIMCAN_HOME/|" | tr '\n' ':')

gdb --args ./work/testing_dbg \
    -r 0 -u Cmdenv \
    -n $S2F_NEDFOLDERS:$INET_NEDFOLDERS:$SIMCAN_HOME/tests/work/TDnsHeader \
    ./work/TDnsHeader/_defaults.ini