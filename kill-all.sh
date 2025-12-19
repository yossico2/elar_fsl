#!/bin/bash

PIDS=$(pgrep -f "./fsl")
if [[ -n "${PIDS}" ]]; then
	kill "${PIDS}"
fi
