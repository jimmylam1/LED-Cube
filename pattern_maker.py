
# ----------------------------------------------------------------------
def write_to_file(fname, cube):
    """Saves pattern to given file"""
    with open(fname, 'a') as f:
        # add to .txt file
        for layer in cube:
            for i in range(4):
                val1 = 8*layer[i*4]
                val2 = 4*layer[i*4 + 1]
                val3 = 2*layer[i*4 + 2]
                val4 = layer[i*4 + 3]
                val = val1 + val2 + val3 + val4
                val = str(hex(val))[-1].upper()
                f.write(val)
            f.write(' ')
        f.write('\n')
    return

def init_cube():
    cube = []
    for i in range(4):
        layer = []
        for j in range(16):
            layer.append(0)
        cube.append(layer)
    return cube
# ----------------------------------------------------------------------
#                     All patterns are below

def led_drop(fname):
    """ Randomly pick LEDs from top row, and they fall to bottom row"""
    led_pick = [8, 6, 14, 3, 4, 1, 9, 15, 12, 11, 10, 2, 5, 13, 0, 7]
    cube = init_cube()

    for i in range(16):
        cube[0][i] = 1 # [layer][row_idx]

    for pick in led_pick:
        for i in range(3):
            cube[i][pick] = 0
            cube[i + 1][pick] = 1
            write_to_file(fname, cube)

def spin_column(fname):
    """ light up vertical edge by going around"""
    cube = init_cube()
    row_idx = [0, 1, 2, 3, 7, 11, 15, 14, 13, 12, 8, 4]

    for i in range(4):
        cube[i][0] = 1 # [layer][row_idx]

    for i in range(len(row_idx)):
        for j in range(4):
            cube[j][row_idx[i]] = 1
        write_to_file(fname, cube)

    for i in range(len(row_idx)):
        for j in range(4):
            cube[j][row_idx[i]] = 0
        write_to_file(fname, cube)

def main():
    fname = 'custom-pattern.txt'

    #led_drop()
    spin_column(fname)

    print(f'Done saving pattern data to {fname}')

if __name__ == '__main__':
    main()
