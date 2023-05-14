import sys


def main():
    test_name = sys.argv[1]
    test_filename = f'{test_name}.bin'
    with open(test_filename, 'rb') as f:
        binary = f.read()
    print(' '.join(f'{x:02x}' for x in binary))


if __name__ == '__main__':
    main()
