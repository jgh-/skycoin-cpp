#!/bin/bash
sudo apt-get -y install cmake libtool autoconf automake libssl-dev

WORKING_DIR=`pwd`
DEPS=$WORKING_DIR/.deps
THIRDPARTY=$WORKING_DIR/THIRDPARTY
SEPARATOR="--------------------------------------------------------------------------------"

mkdir -p $DEPS
rm -rf cpp_modules
mkdir -p cpp_modules/lib
mkdir -p cpp_modules/include

echo "Working from $WORKING_DIR"
echo "3rd Party Software" > $THIRDPARTY
echo "-=-=-=-=-=-=-=-=-=" >> $THIRDPARTY
GIT_REV=""

# unpause tools
git clone https://github.com/unpause-live/cpptools.git $DEPS/unpause-tools
pushd $DEPS/unpause-tools
    cp -R include/* $WORKING_DIR/cpp_modules/include
    GIT_REV=$(git rev-parse --short HEAD)
    cp -R include/unpause/* $WORKING_DIR/cpp_modules/include/unpause
    printf "\n\nC++Tools by Unpause ($GIT_REV)\n" >> $THIRDPARTY
    cat LICENSE >> $THIRDPARTY
    echo $SEPARATOR >> $THIRDPARTY
popd

# spdlog
git clone https://github.com/gabime/spdlog.git $DEPS/spdlog
mkdir -p $WORKING_DIR/cpp_modules/include/spdlog
pushd $DEPS/spdlog
    git fetch --tags
    GIT_REV=$(git describe --tags `git rev-list --tags --max-count=1`)
    git checkout $GIT_REV
    cp -R include/spdlog/* $WORKING_DIR/cpp_modules/include/spdlog

    printf "\n\nspdlog Super fast C++ logging library by Gabi Melman ($GIT_REV)\n" >> $THIRDPARTY
    cat LICENSE >> $THIRDPARTY
    echo $SEPARATOR >> $THIRDPARTY
popd

# json.hpp
git clone https://github.com/nlohmann/json.git $DEPS/json
mkdir -p $WORKING_DIR/cpp_modules/include/json
pushd $DEPS/json
    git fetch --tags
    GIT_REV=$(git describe --tags `git rev-list --tags --max-count=1`)
    git checkout $GIT_REV
    cp src/json.hpp $WORKING_DIR/cpp_modules/include/json

    printf "\n\nJSON for Modern C++ by Niels Lohmann ($GIT_REV)\n" >> $THIRDPARTY
    cat LICENSE.MIT >> $THIRDPARTY
    echo $SEPARATOR >> $THIRDPARTY
popd
rm -rf $DEPS