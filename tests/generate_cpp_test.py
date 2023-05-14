import sys


def main():
    test_name = sys.argv[1]
    test_filename = f'asm/{test_name}.bin'
    with open(test_filename, 'rb') as f:
        binary = f.read()
    cpp_test_filename = f'cpp/{test_name}.hpp'
    with open(cpp_test_filename, 'w') as f:
        f.write(f'''#pragma once

class {test_name.capitalize()}
{{
public:
    static constexpr unsigned char data[{len(binary)}] = {{{', '.join(f'0x{x:02x}' for x in binary)}}};
}};
''')


if __name__ == '__main__':
    main()
