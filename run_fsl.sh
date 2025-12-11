#!/bin/bash
# Run FSL in a Docker container with host networking
set -e

print_usage() {
	echo "Usage: $0 [instance] [-d]"
	echo "  instance    Instance number (default: 1)"
	echo "  -d          Run container in detached mode (can be before or after instance)"
}

if [[ "$1" == "-h" || "$1" == "--help" ]]; then
	print_usage
	exit 0
fi

function main() {
	detach=""
	instance=1

	# parse args in any order
	for arg in "$@"; do
		if [[ "$arg" == "-d" ]]; then
			detach="-d"
		elif [[ "$arg" =~ ^[0-9]+$ ]]; then
			instance="$arg"
		fi
	done

	docker run --rm ${detach} -it --network=host \
		--name "fsl-${instance}" \
		-e SENSOR_INSTANCE="${instance}" \
		fsl
}

main "$@"
