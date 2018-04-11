#!/bin/bash
set -e

show_help() {
	echo "usage: $0 [run|test] [Options...]"
	echo "Build GENEX if no argument is provided. Otherwise:"
	echo "	run     build then run"
	echo "	test    build then test"
	echo "Options:"
	echo "  -d					compile in debug mode"
	echo "  -r					rebuild everything"
	echo "  -h					show this help"
	echo "  --cmake	flags...	flags to be passed to cmake"
}

OPTS=$(getopt -o rdh --long cmake: -n "$0" -- "$@")

if [ $? != 0 ] ; then show_help; exit 1; fi

REBUILD=false
DEBUG=false
SHOW_HELP=false

while true; do
	case "$1" in
		-r ) 
			REBUILD=true; shift; ;;
		-d )
			DEBUG=true; shift; ;;
		-h )
			SHOW_HELP=true; shift; ;;
		--cmake )
			CMAKE_FLAGS="$2"; shift; shift; ;;
		-- ) 
			shift; break; ;;
		* ) 
			break ;;
	esac
done

if [ $SHOW_HELP = true ]; then
	show_help
	exit 0
fi

if [ $# -gt 1 ]; then
	echo "$0: too many arguments"
	show_help
	exit 1
fi

if [ $# -eq 1 ] && [ "$1" != "run" ] && [ "$1" != "test" ]; then
	echo "$0: unknown argument - $1"
	show_help
	exit 1
fi

echo "Create and enter the 'build' directory"
mkdir -p build 
cd build

if [ $DEBUG = true ]; then
	echo "Build in Debug mode"
	CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=Debug"
fi

echo "CMake flags: ${CMAKE_FLAGS}"
cmake .. $CMAKE_FLAGS

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

if [ $unameOut = Linux ]; then
	CPU=$(lscpu | grep ^CPU\(s\): | tr -s " " | cut -f2 -d " ")
	BUILD_CPU=$(expr $CPU / 2)
	MULTI_THREAD_BUILD="-j $BUILD_CPU"
	echo "Number of CPU(s): $CPU"
	echo "Building with $BUILD_CPU CPU(s)"
fi

if [ $REBUILD = true ]; then
	make clean
fi

make $MULTI_THREAD_BUILD

cp -v pygenex/pygenex.so ../scripts/ 

if [ "$1" = "run" ]; then
	./genexcli
elif [ "$1" = "test" ]; then
	ctest -V
fi

exit 0
