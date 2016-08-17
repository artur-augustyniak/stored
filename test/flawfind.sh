#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
${SCRIPT_DIR}/vendor/flawfinder -m 0 ${SCRIPT_DIR}/../src

