#!/bin/bash
# Run FSL in a Docker container with host networking
set -e

function main() {
	instance="${1:-1}"
	docker run --rm -it --network=host \
		--name "fsl-${instance}" \
		-e SENSOR_INSTANCE="${instance}" \
		"fsl"
}

main "$@"
