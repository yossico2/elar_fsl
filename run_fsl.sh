#!/bin/bash

print_usage() {
	echo "Usage: $0 [instance] [-d] [-nd] [-h|--help]"
	echo "  instance    Instance number to run (default: 1)"
	echo "  -d          Run container in detached mode (can be before or after instance)"
	echo "  -nd         Run FSL directly (no Docker)"
	echo "  -h, --help  Show this help message"
	echo ""
	echo "Environment variables:"
	echo "  NO_DOCKER=1   Run FSL directly (no Docker), equivalent to -nd"
}

if [[ "$1" == "-h" || "$1" == "--help" ]]; then
	print_usage
	exit 0
fi

function main() {
	local detach=""
	local instance=0
	local run_no_docker=0

	# parse args in any order
	for arg in "$@"; do
		if [[ "$arg" == "-d" ]]; then
			detach="-d"
		elif [[ "$arg" == "-nd" ]]; then
			run_no_docker=1
		elif [[ "$arg" =~ ^[0-9]+$ ]]; then
			instance="$arg"
		fi
	done

	# Check NO_DOCKER env var
	if [[ "${NO_DOCKER}" == "1" ]]; then
		run_no_docker=1
	fi

	if [[ ${run_no_docker} -eq 1 ]]; then
		logdir="${HOME}/dev/logs"
		mkdir -p "${logdir}"

		local fsldir=
		if [[ "${ABSOLUTE_FSL_RUN}" == "true" ]]; then
			fsldir="${HOME}/dev/elar/elar_fsl/build/linux/debug"
		else
			# source env.sh to get BUILD_TARGET_DIR
			source "$(dirname "$0")/env.sh"
			fsldir="${BUILD_TARGET_DIR}"
		fi

		cd "${fsldir}" || exit 1

		# kill all previous fsl for this instance
		# use pgrep to find and kill all fsl processes for this instance
		local prev_pids=
		prev_pids=$(pgrep -f "./fsl ${instance}")
		echo "Previous fsl instances for instance ${instance}: ${prev_pids}"
		if [[ -n "${prev_pids}" ]]; then
			for pid in ${prev_pids}; do
				kill "${pid}"
			done
		fi

		nohup ./fsl "${instance}" >"${logdir}/fsl-${instance}.log" 2>&1 &
		if [[ $? -ne 0 ]]; then
			echo "Error: Failed to start fsl in background" >&2
			exit 1
		fi

		cd - &>/dev/null || exit 1
		echo "Started fsl instance ${instance} in background. Output: ${logdir}/fsl-${instance}.log" >&2
	else
		docker run --rm ${detach} -it --network=host \
			--name "fsl-${instance}" \
			-e STATEFULSET_INDEX="${instance}" \
			fsl
	fi
}

main "$@"
