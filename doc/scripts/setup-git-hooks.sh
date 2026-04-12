#!/bin/sh

set -eu

if [ ! -f ".gitattributes" ]; then
    echo "❌ Run this script from the repository root." >&2
    exit 1
fi

sh doc/scripts/setup-git-ignore-42-header.sh
git config core.hooksPath .githooks

echo "✅ clang-format will be applied before each push"
