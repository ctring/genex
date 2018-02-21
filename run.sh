#!/bin/bash

show_help() {
	echo "usage: $0 build|run|test [debug]"
	echo "  build - build only"
	echo "  run   - build then run"
	echo "  test  - build then test"
	echo "  debug - compile in debug mode"
}

if [ $# -ne 1 ] && [ $# -ne 2 ]; then
	show_help
	exit 1
fi

if [ $# -eq 2 ] && [ $2 != "debug" ]; then
	show_help
	exit 1
fi

if [ $1 = "build" ] || [ $1 = "run" ] || [ $1 = "test" ]; then

	mkdir -p build 
	cd build

	if [ $# -eq 2 ]; then
		CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Debug"
	fi

	cmake .. $CMAKE_FLAGS
	CPU=$(lscpu | grep ^CPU\(s\): | tr -s " " | cut -f2 -d " ")
	BUILD_CPU=$(expr $CPU / 2)

	echo "Number of CPU(s): $CPU"
	echo "Building with $BUILD_CPU CPU(s)"
	make -j $BUILD_CPU

	if [ $1 = "run" ]; then
		./genexcli
	elif [ $1 = "test" ]; then
		ctest -V
	fi
	exit 0
fi

show_help
exit 1


