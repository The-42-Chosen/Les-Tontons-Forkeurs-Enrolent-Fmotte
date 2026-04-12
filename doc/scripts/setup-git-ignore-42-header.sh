#!/bin/sh

set -eu

if [ ! -f ".gitattributes" ]; then
    echo "Run this script from the repository root (where .gitattributes is located)." >&2
    exit 1
fi

git config merge.ignore42header.name "Ignore 42 header differences"
git config merge.ignore42header.driver "sh doc/scripts/git-merge-ignore-42-header.sh %O %A %B"

echo "Configured merge driver: ignore42header"
echo "It is now active for *.c, *.h, *.cpp, *.hpp and Makefile via .gitattributes"