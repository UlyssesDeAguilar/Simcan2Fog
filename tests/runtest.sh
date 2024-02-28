# Point to the definitions
NEDPATH=$SIMCAN_HOME/src:$INET_HOME/src/:$SIMCAN_HOME/tests/modules/:.
# DNS/*.test simschema/*.test

if [ $# -eq 0 ]
then
    echo "You must specify test folder/s (or \"all\" for running all tests) as arguments"
    exit 1
fi

if [ $1 = "all" ]
then
    TARGETS=("DNS" "simschema" "unit" "switch")
else
    TARGETS="$@"
fi

for folder in ${TARGETS[@]}
do
    ALL="$ALL $folder/*.test"
done

# Then run the tests
opp_test run -v $ALL -p testing_dbg -a "-n $NEDPATH" || exit 1
