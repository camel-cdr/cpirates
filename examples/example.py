#!/usr/bin/env python3
import sys

def write(msg):
    print(msg, flush=True)

def read():
    return sys.stdin.read(2)[0]

def run():
    write("lf")
    if read() == 't':
        write("mT")
        read()
        run()
        write("pbm");
        return

    write("ll")
    if read() == 't':
        write("tl")
        run()
        write("tr")
        return

    write("lr")
    if read() == 't':
        write("tr")
        run()
        write("tl")
        return

    write("tbpbm")
    return

run();

