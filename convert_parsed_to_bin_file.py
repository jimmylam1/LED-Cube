from sys import argv

def get_data(fname: str):
    # must use parsed file
    data = []
    with open(fname, 'r') as f:
        for line in f:
            line = line.strip()
            if line != '' and '//' not in line:
                # if line is not empty and is not a comment
                line = line.split(' ') # split between spaces
                for item in line:
                    # go through each item in the splitted line list
                    if ':' not in item: # to ignore frame count
                        for digit in item:
                            try:
                                digit = int(digit, 16)
                                data.append(digit)
                            except ValueError:
                                raise SyntaxError("value '{}' is not a hex digit".format(digit))
        return data

def convert(data, fname):
    byte_array = []
    for i in range(int(len(data) / 2)):
        # since each value in data is half a byte, combine 2 for 1 byte
        value1 = data[i*2]
        value2 = data[(i*2) + 1]
        byte_array.append((value1 * 16) + value2)

    with open('{}.bin'.format(fname[:-4]), 'wb') as f:
        #print(data)
        converted = bytearray(byte_array)
        f.write(converted)
    print('\nDone writing to {}.bin'.format(fname[:-4]))
