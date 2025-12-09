#!/bin/bash

set -e

function main() {
	print_usage() {
		echo "Usage: $0 [-b|--build debug|release] [-t|--target linux|petalinux] [-h|--help]"
		echo "  -b, --build   Build type: debug (default) or release"
		echo "  -t, --target  Target platform: linux (default) or petalinux"
		echo "  -h, --help    Show this help message"
	}
	BUILD_TYPE="Debug"
	BUILD_DIR="build-debug"
	TARGET="linux"

	# Parse arguments
	while [[ $# -gt 0 ]]; do
		key="$1"
		case $key in
		-b | --build)
			if [[ "$2" == "release" || "$2" == "Release" ]]; then
				BUILD_TYPE="Release"
				BUILD_DIR="build-release"
			elif [[ "$2" == "debug" || "$2" == "Debug" ]]; then
				BUILD_TYPE="Debug"
				BUILD_DIR="build-debug"
			else
				echo "Unknown build type: $2. Use 'debug' or 'release'."
				exit 1
			fi
			shift # past argument
			shift # past value
			;;
		-t | --target)
			if [[ "$2" == "linux" || "$2" == "Linux" ]]; then
				TARGET="linux"
			elif [[ "$2" == "petalinux" || "$2" == "PetaLinux" || "$2" == "petalinux" ]]; then
				TARGET="petalinux"
			else
				echo "Unknown target: $2. Use 'linux' or 'petalinux'."
				exit 1
			fi
			shift # past argument
			shift # past value
			;;
		-h | --help)
			print_usage
			exit 0
			;;
		*) # unknown option
			echo "Unknown option: $1"
			print_usage
			exit 1
			;;
		esac
	done

	# Print in white on blue background
	echo -e "\e[97;44mStarting build process ($BUILD_TYPE, $TARGET)...\e[0m"

	mkdir -p "$BUILD_DIR"
	cd "$BUILD_DIR"

	if [[ "$TARGET" == "petalinux" ]]; then
		cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DPETALINUX=ON ..
	else
		cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
	fi

	make
	# Also build tests target if present
	if grep -q 'add_executable *(tests' ../CMakeLists.txt; then
		make tests || true
	fi
	cd ..

	# Print in green
	echo -e "\e[32mBuild process completed.\e[0m"
}

main "$@"
