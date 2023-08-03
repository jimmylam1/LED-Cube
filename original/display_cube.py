from graphics import *
from sys import argv
from time import sleep

def get_data(fname):
    """Reads a binary file and converts each bit to a 0 or 1.
       Each bit is an individual element in frame_as_bits list,
       and each element in data is a frame of 64 bits.
    """
    with open(fname, 'rb') as f:
        data = [] # each stores

        count = 0
        byte_data = f.readline()
        for line in f:
            byte_data += line

        for i in range(int(len(byte_data) / 8)):
            frame = []
            frame_as_bits = []
            for j in range(8):
                frame.append(byte_data[count])
                count += 1

            for byte in frame:
                as_bits = bin(byte)
                as_bits = as_bits[2:]  # to remove 0b from beginning
                num_bits = len(str(as_bits))

                for k in range(8 - num_bits):
                    frame_as_bits.append(0)
                for bit in as_bits:
                    frame_as_bits.append(int(bit))
            data.append(frame_as_bits)

    return data

def init_circles():
    init_circles_list = []
    # c1 = Circle(Point(100, 400), 10)

    for layer in range(4):
        for i in range(4):
            i += 1
            for j in range(4):
                j += 1
                if layer == 0:                             # x-right      y-down
                    init_circles_list.append(Circle(Point(j * 130 - 50, i * 130 + 35), 15))
                elif layer == 1:
                    init_circles_list.append(Circle(Point(j * 130 + 25 - 50, i * 130 - 0), 13))
                elif layer == 2:
                    init_circles_list.append(Circle(Point(j * 130 + 50 - 50, i * 130 - 35), 12))
                else:
                    init_circles_list.append(Circle(Point(j * 130 + 75 - 50, i * 130 - 70), 12))

    for circle in init_circles_list:
        # grey = off, blue = on
        circle.setOutline('grey')
        circle.setFill('grey')

    circles_list = []
    for i in range(4):
        for j in range(3, -1, -1):
            for k in range(4):
                circles_list.append(init_circles_list[(i * 4) + (j * 16) + k])

    return circles_list

def get_lines(circles_list):
    lines_list = []
    # get vertical lines
    for i in range(4):
        for j in range(4):
            point1 = circles_list[(i * 16) + (j * 4)].getCenter()
            point2 = circles_list[(i * 16) + (j * 4) + 3].getCenter()
            lines_list.append(Line(point1, point2))

    # get horizontal lines
    for i in range(4):
        for j in range(4):
            point1 = circles_list[i + (j * 4)].getCenter()
            point2 = circles_list[i + 48 + (j * 4)].getCenter()
            lines_list.append(Line(point1, point2))

    # get vertical lines
    for i in range(4):
        for j in range(4):
            point1 = circles_list[(i * 16) + j].getCenter()
            point2 = circles_list[(i * 16) + 12 + j].getCenter()
            lines_list.append(Line(point1, point2))

    return lines_list

def visualize(data, animate=False, period=0, repeats = 1):
    win = GraphWin("Window", 600, 600)
    win.setBackground('wheat')

    circles_list = init_circles()
    line_list = get_lines(circles_list)

    for line in line_list:
        line.draw(win)

    for circle in circles_list:
        circle.draw(win)

    #win.getKey()
    message = Text(Point(500, 10), 'Frame count: 0')
    message.draw(win)

    for i in range(repeats):
        frame_count = 0

        for frame in data:
            message.setText('Frame count: {}'.format(frame_count))
            frame_count += 1
            for i in range(len(frame)):
                if frame_count <= 2:
                    if frame[i] == 0:
                        circles_list[i].setFill('grey')
                    else:
                        circles_list[i].setFill('blue')
                else:
                    if frame[i] == 0:
                        # if bit == 0, LED should be off
                        if i == 0:
                            circles_list[i].setFill('grey')
                        elif data[frame_count - 2][i] != data[frame_count - 1][i]:
                            # -2 because frame_count is incremented immediately after next frame
                            circles_list[i].setFill('gray')
                        #circles_list[i].setOutline('grey')
                    else:
                        # bit == 1, so LED should be on
                        if i == 0:
                            circles_list[i].setFill('blue')
                        elif data[frame_count - 2][i] != data[frame_count - 1][i]:
                            circles_list[i].setFill('blue')
                        #circles_list[i].setOutline('blue')
                
            if animate:
                sleep(period)
            else:
                win.getKey() # wait for keypress per frame

    end_message = Text(Point(300, 300), 'All Done. Press any key to exit.')
    end_message.draw(win)
    win.getKey()
    win.close()

def main():
    fname = 'animations.bin'

    # ---------------------------
    #animate = False
    animate = True
    frequency = 20 #Hz
    repeat = 1 # number of repeats ( 1 = run only once )
    # ---------------------------

    period = 1 / frequency

    data = get_data(fname)
    visualize(data, animate, period, repeat)

if __name__ == '__main__':
    main()
