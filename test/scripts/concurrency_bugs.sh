#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
valgrind -v --tool=helgrind ${SCRIPT_DIR}/../../build/stored -f ${SCRIPT_DIR}/../../etc/stored.cfg