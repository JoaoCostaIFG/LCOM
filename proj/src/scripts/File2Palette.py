#!/usr/bin/env python3

import sys


def file_read_content(file_name):
    # open and read file
    fp = open(file_name, "r")
    text = fp.read()

    print(text)  # print file contents for debug purposes

    fp.close()    # close file
    return text


def out_binary_content(output_file, text):
    # open file for output
    fp = open(output_file, "wb")

    for word in text.split():
        fp.write(bytes([int(word)]))

    fp.close()
    return 0


if len(sys.argv) != 3:
    print("Usage: <input_file> <output_file>")

# get input file content
text = file_read_content(sys.argv[1])

# output binary content to file
out_binary_content(sys.argv[2], text)

# f = open(sys.argv[2], "rb")
# try:
    # byte = f.read(1)
    # while byte != b'':
        # # Do stuff with byte.
        # print(int.from_bytes(byte, "little"))
        # byte = f.read(1)
# finally:
    # f.close()
