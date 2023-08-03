from sys import argv

def parse_data(fname: str):
    """resolves repeats, leaves comments in"""
    data = []

    with open(fname, 'r') as f:
        # remove header lines
        for i in range(12):
            f.readline().strip()

        for line in f:
            line = line.strip()
            if 'repeat' in line and 'keyword' not in line and 'e.g.' not in line and '//' not in line:
                line = line.split(' ')
                iterations = int(line[1])
                sequence = []
                while '}' not in line:
                    line = f.readline()
                    line = line.strip()
                    if '}' not in line:
                        sequence.append(line)
                for i in range(iterations):
                    for frames in sequence:
                        data.append(frames.upper())
            elif '//' in line:
                # don't capitalize comments
                data.append(line)
            else:
                # just the data
                data.append(line.upper())
    return data

def write_to_file(data_list, fname: str):
    with open("{}.par".format(fname[:-4]), 'w') as f:
        count = 0
        for line in data_list:
            if line == '' or '//' in line:
                f.write('{}\n'.format(line))
            else:
                if len(data_list) < 10:
                    f.write('{}: {}\n'.format(count, line))
                elif len(data_list) < 100:
                    f.write('{:2}: {}\n'.format(count, line))
                else:
                    f.write('{:3}: {}\n'.format(count, line))
                count += 1
    print('\nDone parsing {} to {}.par'.format(fname, fname[:-4]))

