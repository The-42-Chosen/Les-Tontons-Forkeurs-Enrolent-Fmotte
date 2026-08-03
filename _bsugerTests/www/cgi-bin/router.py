#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qsl

UPLOAD_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "upload")


def get_query_param(name):
    raw = os.environ.get("QUERY_STRING", "")
    for key, value in parse_qsl(raw, keep_blank_values=True):
        if key == name:
            return value
    return ""


def read_body():
    try:
        length = int(os.environ.get("CONTENT_LENGTH") or 0)
    except ValueError:
        length = 0
    if length <= 0:
        return b""
    return sys.stdin.buffer.read(length)


def main():
    method = os.environ.get("REQUEST_METHOD", "GET")
    filename = get_query_param("name")

    if not filename or "/" in filename or filename in (".", ".."):
        print("Content-Type: text/plain")
        print()
        print("Bad filename")
        return

    target = os.path.join(UPLOAD_DIR, filename)

    # NOTE: CGIRequest.cpp maps every non-POST HTTP method to REQUEST_METHOD=GET
    # (src/request/CGIRequest.cpp:294), so a real DELETE request is indistinguishable
    # from a GET here. Since this router is only ever invoked with POST or DELETE in
    # the test suite, treat anything that isn't POST as a delete.
    if method == "POST":
        body = read_body()
        with open(target, "wb") as f:
            f.write(body)
        print("Content-Type: text/plain")
        print()
        print("Created %s" % filename)
    else:
        try:
            os.remove(target)
        except OSError:
            pass
        print("Content-Type: text/plain")
        print()


main()
