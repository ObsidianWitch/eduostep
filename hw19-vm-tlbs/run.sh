#!/bin/env -S taskset -c 0 bash

set -o errexit -o nounset

make
for i in {0..13}; do
    out/tlb 0 "$((2**i))"
done
