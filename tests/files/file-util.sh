#!/bin/sh

tnef=$srcdir/../../src/tnef
test=`echo $0 | sed -e 's/\(.*\)\..*/\1/'`

check_test_files () {
    for f in `cat datafiles/$test.list`
    do
      check_test_full $test.dir/$f baselines/$f.baseline $f.diff
    done
}

mktestdir () { 
    mkdir $test.dir
}

cleanup () {
    rm -rf $test.dir
}

run_test () {
    check_exists datafiles/$test.tnef
    check_exists datafiles/$test.list
    $tnef --debug --directory $test.dir \
        datafiles/$test.tnef > $test.output 2>&1
    check_test_full $test.output baselines/$test.baseline $test.diff
    check_test_files
}
