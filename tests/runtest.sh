# Point to the definitions
NEDPATH=$SIMCAN_HOME/src:$INET_ROOT/src/:$SIMCAN_HOME/tests/modules/:.
# DNS/*.test simschema/*.test

if [ $# -eq 0 ]
then
    echo "You must specify test folder/s (or \"all\" for running all tests) as arguments"
    exit 1
fi

if [ $1 = "all" ]
then
    TARGETS=("unit" "simschema" "datamanager" "switch" "DNS" "dc")
else
    TARGETS="$@"
fi

for folder in ${TARGETS[@]}
do
    ALL="$ALL $folder/*.test"
done

# Then run the tests
opp_test run -v $ALL -p testing_dbg -a "-n $NEDPATH" || exit 1
