#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
valgrind --leak-check=yes --track-origins=yes -v ${SCRIPT_DIR}/../../src/stored