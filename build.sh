#! /bin/bash

pushd () {
	command pushd "$@" > /dev/null
}

popd () {
	command popd "$@" > /dev/null
}

if [ "$1" == "run" ]; then
	pushd build/bin
	./datum
	popd
	exit
fi

if [ "$1" == "test" ]; then
	pushd build/bin
	./datum
	popd
	exit
fi



mkdir -p build
pushd build &&\
cmake .. &&\
make
#cmake --build .

