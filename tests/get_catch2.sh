#!/bin/bash
# Download Catch2 single-header (v2.x recommended)
# This script will fetch Catch2 and place it in the tests/ directory.

cd "$(dirname "$0")" || exit 1

if [ ! -f "catch.hpp" ]; then
	echo "Downloading Catch2 v2.13.10..."
	curl -L -o catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp
else
	echo "Catch2 v2 header already exists."
fi
