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

def r_generate(path, answer_path):
    num_files = r.randint(1, 10)
    for i in range(num_files):
        if r.random() > 0.9 and i > 1:
            run_command(['mkdir', path + "/recursive_test" + str(i)])
            run_command(['mkdir', answer_path + "/recursive_test_ans" + str(i)])
            r_generate(path+"/recursive_test" + str(i), answer_path+"/recursive_test_ans" + str(i))
        else:
            f = open(path + "/auto_test" + str(i) + ".txt", "w")
            f1 = open(answer_path + "/answers" + str(i) + ".txt", "w")
            # write a random file
            length = r.randint(1, 1000)   # Number of characters in file
            i = 0
            while i < length:
                s = r.choice(string.printable)
                f.write(s)
                f1.write(s)
                i += 1
            f.close()
            f1.close()

def grade(path, answer_path):
    print(os.listdir(path))
    print(os.listdir(answer_path))
    for file, ans in zip(sorted(os.listdir(path)), sorted(os.listdir(answer_path))):
        if os.path.isdir(path + "/" + file):
            grade(path+"/"+file, answer_path+"/"+ans)
            continue
        f = open(path + "/" + file, 'r')
        f1 = open(answer_path + "/" + ans, "r")
        for line, line_ans in zip(f, f1):
            if (line != line_ans):
                print("Comparing " + str(file) + " to " + str(ans))
                print("Error")
                error = True


def recursive():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    if (os.path.exists(dir_path + "/recursive_test")):
        run_command(['rm', '-r', dir_path + "/recursive_test"])
        run_command(['rm', '-r', dir_path + "/recursive_test_ans"])
    run_command(['mkdir', dir_path + "/recursive_test"])
    run_command(['mkdir', dir_path + "/recursive_test_ans"])
    r_generate(dir_path + "/recursive_test", dir_path + "/recursive_test_ans")
    name = dir_path + "/recursive_test"
    # make
    if os.path.isfile('fileCompressor'):
        run_command(['make', 'clean'])
    run_command(['make'])
    # build codebook
    run_command(['./fileCompressor', '-b', '-R', name])
    # compress
    run_command(['./fileCompressor', '-c', '-R', name, 'HuffmanCodebook'])
    # decompress
    run_command(['./fileCompressor', '-d', '-R', name + '.hcz', 'HuffmanCodebook'])
    # compare with origin file
    grade(dir_path + "/recursive_test", dir_path + "/recursive_test_ans")


def main():
    error = False
    name = "auto_test.txt"
    if len(sys.argv) == 2:
        if sys.argv[1] == "-R":
            return recursive()
        else:
            name = sys.argv[1]
            f = open(name, "r+")
            f1 = open("answers.txt", "w+")

    else:
        f = open(name, "w")
        f1 = open("answers.txt", "w+")
        # write a random file
        length = r.randint(1, 1000)   # Number of characters in file
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
