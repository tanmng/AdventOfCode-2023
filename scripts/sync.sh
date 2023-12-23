#!/bin/bash

# Sync the data - ignore the Python code which I used for basic debug
rsync -avip --progress \
	--exclude='**/*.py' \
	~/tann-monorepo/code/aoc-2023/ \
	.

# Fix all the `include` statements so that the code can work with new repo structure
find . \( -name '*.cpp' -o -name '*.h' \) \
	-exec sed -i 's;^\(#include "\)code/aoc-2023/\(.*\)$;\1\2;g' {} \;