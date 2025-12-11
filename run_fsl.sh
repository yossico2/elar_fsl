#!/bin/bash
# Run FSL in a Docker container with host networking
set -e

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
