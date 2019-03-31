#!/usr/bin/python3
import random as r
import sys, os, subprocess
import string

class Error(Exception):
    pass

class CommmandError(Error):
    def __init__(self, cmd, code, out=None):
        self.cmd = cmd
        self.code = code
        self.out = out

def run_command(cmd):
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (out, err) = p.communicate()

    if p.returncode != 0:
        raise CommmandError(cmd, p.returncode, out)

def main():
    error = False
    name = "auto_test.txt"
    if len(sys.argv) == 2:
        name = sys.argv[1]
        f = open(name, "r+")
        f1 = open("answers.txt", "w+")

    else:
        f = open(name, "w")
        f1 = open("answers.txt", "w+")
        # write a random file
        length = r.randint(0, 100000)   # Number of characters in file
        i = 0
        while i < length:
            s = r.choice(string.printable)
            f.write(s)
            f1.write(s)
            i += 1
        f.close()
        f1.close()
        # make
        if os.path.isfile('fileCompressor'):
            run_command(['make', 'clean'])
        run_command(['make'])
        # build codebook
        run_command(['./fileCompressor', '-b', name])
        # compress
        run_command(['./fileCompressor', '-c', name, 'HuffmanCodebook'])
        # decompress
        run_command(['./fileCompressor', '-d', name + '.hcz', 'HuffmanCodebook'])
        # compare with origin file
        f = open(name, 'r')
        f1 = open("answers.txt", "r")
        for line, ans in zip(f, f1):
            if (line != ans):
                print("Error")
                error = True
    if not error:
        print("Test passed!")
    return

if __name__ == "__main__":
    main()
