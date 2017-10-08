#!/usr/bin/env sh

exec=`realpath $0`
root=`dirname $exec`

set -e
mkdir -p $root/_build
cd $root/_build && conan install .. && cd -
cmake -H$root -B$root/_build -DCMAKE_INSTALL_PREFIX=$root/_install -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=1 -Wno-dev
env VERBOSE=1 cmake --build $root/_build --target tests
env VERBOSE=1 cmake --build $root/_build --target install
