MAKE="make -j4 MODE=debug"
if [ ! -d work ];  then mkdir work; fi
if [ ! -d work/linet ]; then ln -s ../../../inet4/src work/linet; fi
if [ ! -d work/src ]; then ln -s ../../src/ work/src; fi
if [ ! -d work/modules ]; then ln -s ../modules work/modules; fi
NEDPATH=../../../src:.:../../../../inet4/src/:../modules
EXTRA_INCLUDES="-I../../../src"
INET_PROJ=./linet/

# Generate the test case files
opp_test gen -v DNS/*.test
opp_test gen -v simschema/*.test

# Copy the network into simschema
cp simschema/Unconnected.ned work/simschemamysql

# At first it seems like magic. It really is just importing the Simcan2Cloud and INET framework
(cd work; opp_makemake -f -O ./tests/work/cmp -o testing --deep -KINET4_PROJ=$INET_PROJ -DINET_IMPORT  -I../../src/ -I$\(INET4_PROJ\)/ -L../../lib/ -L$\(INET4_PROJ\)/  -X $INET_PROJ -lINET$\(D\) -lmysqlcppconn; $MAKE) || exit 1
echo

# Then run the tests
opp_test run -v DNS/*.test simschema/*.test -p testing_dbg -a "-n $NEDPATH" || exit 1