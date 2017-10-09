#!/usr/bin/env sh

exec=`realpath $0`
root=`dirname $exec`

set -e
mkdir -p $root/_build

install_path=${CMAKE_INSTALL_PREFIX:-$root/_install}
cmake_config=${CMAKE_CONFIG:-RelWithDebInfo}
build_shared=${BUILD_SHARED_LIBS:-0}

if [ -z "$GENERATOR" ]; then
    gen_arg=
else
    gen_arg="-G'$GENERATOR'"
fi

### 3P deps ###

# zlib
mkdir -p $root/_build/3p/zlib
cmd="cmake -H$root/3p/zlib -B$root/_build/3p/zlib ${gen_arg} -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=$cmake_config -DBUILD_SHARED_LIBS=$build_shared -Wno-dev"
bash -c "$cmd"
env VERBOSE=1 cmake --build $root/_build/3p/zlib --target install --config $cmake_config

# bzip2
mkdir -p $root/_build/3p/bzip2
cmd="cmake -H$root/3p/bzip2 -B$root/_build/3p/bzip2 ${gen_arg} -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=$cmake_config -DBUILD_SHARED_LIBS=$build_shared -Wno-dev"
bash -c "$cmd"
env VERBOSE=1 cmake --build $root/_build/3p/bzip2 --target install --config $cmake_config

### MAIN PROJECT ###

cmd="cmake -H$root -B$root/_build ${gen_arg} -DCMAKE_MODULE_PATH=$install_path -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=$cmake_config -DBUILD_SHARED_LIBS=$build_shared -Wno-dev"
bash -c "$cmd"
env VERBOSE=1 cmake --build $root/_build --target tests --config $cmake_config
env VERBOSE=1 cmake --build $root/_build --target install --config $cmake_config
