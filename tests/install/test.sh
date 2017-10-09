#!/usr/bin/env sh
exec=`realpath $0`
currdir=`dirname $exec`
parent=`dirname $currdir`
root=`dirname $parent`

set -e

if [ -z "$GENERATOR" ]; then
    export GENERATOR_ARG=
else
    export GENERATOR_ARG="-G'$GENERATOR'"
fi

cmd="cmake -H. -B$currdir/_build ${GENERATOR_ARG} -DCMAKE_INSTALL_PREFIX=$root/_install -DTESTS_DIR=$root/tests"
bash -c "$cmd"
env VERBOSE=1 cmake --build $currdir/_build --target tests_run > $currdir/testLog.txt
rm -fr $currdir/_build

echo '----------------------------------------------------------'
echo '# TEST LOG                                               #'
echo '----------------------------------------------------------'
cat $currdir/testLog.txt
echo '----------------------------------------------------------'
