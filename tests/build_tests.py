import os
import subprocess
from subprocess import Popen

from generate_cpp_test import generate_test
from generate_cpp_test_include import generate_cpp_include


def main():
    assembly_tests = os.listdir('asm')
    os.chdir('asm')
    filtered_tests = [test for test in assembly_tests if test.endswith('.asm')]
    for test in filtered_tests:
        jwasm_proc = Popen(args=['jwasm', '-nologo', '-bin', test], stdout=subprocess.PIPE)
        jwasm_proc.wait()
    os.chdir('..')
    for test in filtered_tests:
        generate_test(test.replace('.asm', ''))
    generate_cpp_include(len(filtered_tests))
    for test in filtered_tests:
        os.remove(f'asm/{test.replace(".asm", ".BIN")}')


if __name__ == '__main__':
    main()
