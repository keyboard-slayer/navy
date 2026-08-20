#!/usr/bin/env bash

if [ "$1" == "doctor" ]; then
    ./meta/scripts/doctor.sh
    exit 0
fi

if command -v buildkit > /dev/null; then
    buildkit $@
elif [ -f ./.venv/bin/buildkit ]; then
    ./.venv/bin/buildkit $@
else
    ./meta/scripts/runner $@
fi
