#!/usr/bin/env python3

import sys


def file_read_content(file_name):
    # open and read file
    fp = open(file_name, "r")
    text = fp.read()

    print(text)  # print file contents for debug purposes

    fp.close()    # close file
    return text


def process_text(text):
    # skip till '#' char is found
    raw_words = text.split()
    for i in range(len(raw_words)):
        if (raw_words[i] == '#'):
            raw_words = raw_words[i + 1:]
            break

    final_text = [len(raw_words) // 4, 0]
    for i in range(0, len(raw_words), 4):
        final_text.append(raw_words[i])
        final_text.append(raw_words[i+1])
        final_text.append(raw_words[i+2])

    print(final_text)
    return final_text


def write2file(file_name, text):
    # open file for writing
    fp = open(file_name, "w")
    for i in text:
        fp.write(str(i))
        fp.write(' ')

    fp.close()


if len(sys.argv) != 3:
    print("Usage: <input_file> <output_file>")

# get input file content
text = file_read_content(sys.argv[1])

# process the read file content
final_text = process_text(text)

# output final content to file
write2file(sys.argv[2], final_text)
