#!/usr/bin/env bash
set -euo pipefail
test_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
firmware_dir="$(cd -- "$test_dir/.." && pwd)"
build_dir="$(mktemp -d)"
trap 'rm -f -- "$build_dir/firmware-tests"; rmdir -- "$build_dir"' EXIT

mapfile -t sources < <(find "$firmware_dir/src" -name '*.cpp' -print | sort)
for diagnostics in 0 1; do
    "${CXX:-g++}" -std=c++14 -O1 -g -Wall -Wextra -Werror \
        -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer -fno-pie -no-pie \
        -DMULTIFX_DIAGNOSTICS="$diagnostics" \
        -I"$test_dir/host/stubs" -I"$firmware_dir/include" \
        "${sources[@]}" "$test_dir/host/test_firmware.cpp" \
        -o "$build_dir/firmware-tests"
    "$build_dir/firmware-tests"
done
