#!/usr/bin/env bash

if [ "$1" == "doctor" ]; then
    ./meta/scripts/doctor.sh
    exit 0
fi

if command -v buildkit > /dev/null; then
    buildkit $@
else
    ./meta/scripts/runner $@
fi
