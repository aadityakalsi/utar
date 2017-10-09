#!/usr/bin/env sh

exec=`realpath $0`
root=`dirname $exec`

set -e
mkdir -p $root/_build

#cd $root/_build && conan install .. && cd -
install_path=${CMAKE_INSTALL_PREFIX:-$root/_install}
cmake_config=${CMAKE_CONFIG:-RelWithDebInfo}

if [ -z "$GENERATOR" ]; then
    export GENERATOR_ARG=
else
    export GENERATOR_ARG="-G'$GENERATOR'"
fi

if [ -z "$CMAKE_CONFIG" ]; then
    export CMAKE_CONFIG_ARG=
else
    export CMAKE_CONFIG_ARG="-G'$CMAKE_CONFIG'"
fi

### 3P deps ###

# zlib
mkdir -p $root/_build/3p/zlib
cmd="cmake -H$root/3p/zlib -B$root/_build/3p/zlib ${GENERATOR_ARG} -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=$cmake_config -DBUILD_SHARED_LIBS=1 -Wno-dev"
bash -c "$cmd"
env VERBOSE=1 cmake --build $root/_build/3p/zlib --target install --config $cmake_config

# bzip2
mkdir -p $root/_build/3p/bzip2
cmd="cmake -H$root/3p/bzip2 -B$root/_build/3p/bzip2 ${GENERATOR_ARG} -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=$cmake_config -DBUILD_SHARED_LIBS=1 -Wno-dev"
bash -c "$cmd"
env VERBOSE=1 cmake --build $root/_build/3p/bzip2 --target install --config $cmake_config

### MAIN PROJECT ###

cmd="cmake -H$root -B$root/_build ${GENERATOR_ARG} -DCMAKE_MODULE_PATH=$install_path -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=$cmake_config -DBUILD_SHARED_LIBS=1 -Wno-dev"
bash -c "$cmd"
env VERBOSE=1 cmake --build $root/_build --target tests --config $cmake_config
env VERBOSE=1 cmake --build $root/_build --target install --config $cmake_config
