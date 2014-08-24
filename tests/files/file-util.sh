#!/bin/sh

tnef=$srcdir/../../src/tnef
test=`echo $0 | sed -e 's/\(.*\)\..*/\1/' -e 's/^\.\///'`

check_test_files () {
    for f in `cat datafiles/$test.list`
    do
      check_test_full "$srcdir/$test.dir/$f"              \
                      "$srcdir/baselines/$f.baseline"     \
                      "$srcdir/$f.diff"
    done
}

mktestdir () { 
    mkdir $srcdir/$test.dir
}

cleanup () {
    rm -rf $srcdir/$test.dir
}

run_test () {
    check_exists $srcdir/datafiles/$test.tnef
    check_exists $srcdir/datafiles/$test.list
    $tnef --debug --directory $srcdir/$test.dir \
	--save-body=$test-body $TEST_EXTRA_ARGS \
        $srcdir/datafiles/$test.tnef > $srcdir/$test.output \
	2> $srcdir/$test.error
    if [ -f $srcdir/$test.error ]; then
	cat $srcdir/$test.error >> $srcdir/$test.output
	rm -f $srcdir/$test.error
    fi
    check_test_full $srcdir/$test.output                \
                    $srcdir/baselines/$test.baseline    \
                    $srcdir/$test.diff
    check_test_files
}
