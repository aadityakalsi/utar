#!/usr/bin/env sh

exec=`realpath $0`
root=`dirname $exec`

set -e
mkdir -p $root/_build

#cd $root/_build && conan install .. && cd -
install_path=${CMAKE_INSTALL_PREFIX:-$root/_install}

### 3P deps ###

# zlib
mkdir -p $root/_build/3p/zlib
cmake -H$root/3p/zlib -B$root/_build/3p/zlib -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 -Wno-dev
env VERBOSE=1 cmake --build $root/_build/3p/zlib --target install

# bzip2
mkdir -p $root/_build/3p/bzip2
cmake -H$root/3p/bzip2 -B$root/_build/3p/bzip2 -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 -Wno-dev
env VERBOSE=1 cmake --build $root/_build/3p/bzip2 --target install

### MAIN PROJECT ###

cmake -H$root -B$root/_build -DCMAKE_MODULE_PATH=$install_path -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=1 -Wno-dev
env VERBOSE=1 cmake --build $root/_build --target tests
env VERBOSE=1 cmake --build $root/_build --target install
