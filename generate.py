import parse_text_file
import convert_parsed_to_bin_file
from sys import argv

def main() :
    """runs all the steps necessary to get binary file"""
    #fname = 'first_15.txt'
    fname = 'animations.txt'

    if len(argv) == 2:
        fname = argv[1]

    if len(argv) == 3:
        if argv[1] == '-v':
            print_frames = True
        fname = argv[2]

    parsed_data = parse_text_file.parse_data(fname)
    parse_text_file.write_to_file(parsed_data, fname)

    fname = '{}.par'.format(fname[:-4])
    data = convert_parsed_to_bin_file.get_data(fname)
    convert_parsed_to_bin_file.convert(data, fname)

if __name__ == '__main__':
    main()