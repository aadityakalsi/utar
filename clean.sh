#!/usr/bin/env sh
exec=`realpath $0`
root=`dirname $exec`

set -e

rm -fr $root/_build $root/_install $root/tests/install/_source $root/tests/install/testLog.txt
