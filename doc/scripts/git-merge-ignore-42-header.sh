#!/bin/sh

set -eu

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <ancestor> <current> <other>" >&2
    exit 2
fi

BASE="$1"
CURRENT="$2"
OTHER="$3"

TMP_DIR="$(mktemp -d)"
cleanup() {
    rm -rf "$TMP_DIR"
}
trap cleanup EXIT INT TERM

extract_header() {
    file="$1"
    if [ ! -s "$file" ]; then
        return
    fi

    first_line="$(head -n 1 "$file" 2>/dev/null || true)"
    case "$first_line" in
        '# **************************************************************************** #'|'/* ************************************************************************** */')
            awk '
                NR == 1 { marker = $0 }
                {
                    print
                    if ($0 == marker) {
                        count++
                        if (count == 2) {
                            exit
                        }
                    }
                }
            ' "$file"
            ;;
        *)
            ;;
    esac
}

strip_header() {
    file="$1"
    if [ ! -s "$file" ]; then
        return
    fi

    first_line="$(head -n 1 "$file" 2>/dev/null || true)"
    case "$first_line" in
        '# **************************************************************************** #'|'/* ************************************************************************** */')
            awk '
                NR == 1 { marker = $0 }
                {
                    if ($0 == marker) {
                        count++
                        if (count == 2) {
                            skipping = 2
                            next
                        }
                    }
                    if (skipping == 2) {
                        if ($0 == "") {
                            next
                        }
                        skipping = 0
                    }
                    if (count >= 2 && skipping == 0) {
                        print
                    }
                }
            ' "$file"
            ;;
        *)
            cat "$file"
            ;;
    esac
}

extract_header "$CURRENT" > "$TMP_DIR/header"
strip_header "$BASE" > "$TMP_DIR/base"
strip_header "$CURRENT" > "$TMP_DIR/current"
strip_header "$OTHER" > "$TMP_DIR/other"

set +e
git merge-file -p "$TMP_DIR/current" "$TMP_DIR/base" "$TMP_DIR/other" > "$TMP_DIR/merged"
MERGED_STATUS=$?
set -e

if [ -s "$TMP_DIR/header" ]; then
    cat "$TMP_DIR/header" > "$CURRENT"
    printf '\n' >> "$CURRENT"
    cat "$TMP_DIR/merged" >> "$CURRENT"
else
    cat "$TMP_DIR/merged" > "$CURRENT"
fi

exit "$MERGED_STATUS"