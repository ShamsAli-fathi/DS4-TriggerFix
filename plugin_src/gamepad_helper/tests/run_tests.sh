#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
cc -std=c11 -O2 -Wall -Wextra -Werror -Iinclude \
   tests/test_trigger_scaling.c -o tests/test_trigger_scaling
./tests/test_trigger_scaling
