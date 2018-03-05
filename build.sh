#! /bin/bash

pushd () {
	command pushd "$@" > /dev/null
}

popd () {
	command popd "$@" > /dev/null
}

if [ "$1" == "run" ]; then
	pushd build-linux
	#./datum
	./licPlateRec
	popd
	exit
fi

if [ "$1" == "test" ]; then
	pushd build-linux
	./datum
	popd
	exit
fi



mkdir -p build-linux
pushd build-linux &&\
cmake .. &&\
make
#cmake --build .

