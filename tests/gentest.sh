#! /bin/bash

# Check if there are arguments
if [ $# -eq 0 ]
then
    echo "You must specify test folder/s (or \"all\" for generating all tests) as arguments"
    exit 1
fi

if [ $1 = "all" ]
then
    TARGETS=("DNS" "simschema" "unit" "switch")
else
    TARGETS="$@"
fi

MAKE="make -j4 MODE=debug"

# Create work directory, needed by opp_test
if [ ! -d work ];  then mkdir work; fi
if [ ! -d work/modules ]; then cd work; ln -s ../modules modules; cd .. ; fi 

# Paths to the project files
INET_PROJ=$INET_HOME/src
SM_PROJ=$SIMCAN_HOME/src

# Constants for the makefile
M_CONST=\-KINET4_PROJ\=$INET_PROJ\ \-KSM_PROJ\=$SM_PROJ

# This is for older valgrind debugging -> Ubuntu 22.04 ships with a buggy version with clang v5 DWARF
#C_OPTS=\-KCXXFLAGS=-gdwarf-4 

# Libs for the proyect
LIBS=\-lINET\$\(D\)\ \-lSimcan2Cloud\$\(D\)\ \-lmysqlcppconn

# Generate the test case files
for folder in ${TARGETS[@]}
do
    ALL="$ALL $folder/*.test"
done

echo "Targets: $ALL"
opp_test gen -v $ALL


# At first it seems like magic. It really is just importing the Simcan2Cloud and INET framework
(cd work; opp_makemake -f -O ./tests/work/cmp -o testing --deep $M_CONST $C_OPTS -DINET_IMPORT  -I$\(SM_PROJ\)/ -I$\(INET4_PROJ\)/ -L$\(SM_PROJ\)/ -L$\(INET4_PROJ\)/ $LIBS; $MAKE) || exit 1
echo
