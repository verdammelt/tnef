#!/bin/sh

# parameters:
#   output
#   baseline
#   diff
check_test_full () {
    diff $1 $2 > $3
    if [ $? -ne 0 ]; then
        echo "\'diff $1 $2 > $3\' -- Test Failed!"
        exit 1
    fi
}

check_test () {
    check_test_full $1.output $1.baseline $1.diff
}

check_exists () {
    if [ ! -f $1 ]; then
        echo "$1 does not exist.  Test Failed!"
        exit 1
    fi
}

check_file () {
    check_exists $1
    check_test_full $1 $1.baseline $1.diff
}

    
        