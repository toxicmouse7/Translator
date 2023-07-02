import sys


def generate_cpp_include(test_count: int):
    with open('cpp/tests.hpp', 'w') as f:
        f.write('#pragma once\n')
        for i in range(test_count):
            f.write(f'#include "test{i + 1}.hpp"\n')


def main():
    test_count = int(sys.argv[1])
    generate_cpp_include(test_count)


if __name__ == '__main__':
    main()
