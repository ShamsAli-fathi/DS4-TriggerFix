#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
sh tests/run_tests.sh
cc -std=gnu11 -Wall -Wno-unused-parameter -Wno-unused-function \
    -fsyntax-only -Itests/stubs -Iinclude source/main.c source/config.c
printf '%s\n' 'PASS: GamePad Helper source syntax with local SDK stubs'
cc -std=gnu11 -O2 -ffunction-sections -fdata-sections \
    -Itests/stubs -Iinclude tests/test_hooks_config.c source/config.c \
    -Wl,--gc-sections -o tests/test_hooks_config
./tests/test_hooks_config
