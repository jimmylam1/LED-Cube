//#include "../arduino.h" // TODO: comment out this line in the Arduino IDE

#define DATA_PIN 0
#define SRCK 1
#define RCK 2

#define L1 3
#define L2 4
#define L3 5
#define L4 6

byte cube[64]; // the array that holds the data for the cube

byte spiralColumns[12] = {0, 1, 2, 3, 7, 11, 15, 14, 13, 12, 8, 4}; // outside perimeter
byte randomColumns[16] = {5, 11, 13, 3, 9, 0, 14, 8, 2, 12, 7, 4, 15, 1, 10, 6}; // "random" columns

void displayCube(int delay_ms) {
    // the main driver, sends the data from the cube array to the actual LED cube
    unsigned long startTime = millis();
    while (millis() - startTime < delay_ms) {
        // multiplex each layer until elapsed time has reached delay_ms
        for (byte i = 0; i < 4; i++) {
            digitalWrite(SRCK, LOW);
            digitalWrite(RCK, LOW);
            // shift out 16 bits for each layer
            for (byte j = 0; j < 16; j++) {
                digitalWrite(DATA_PIN, cube[i*16 + j]);
                digitalWrite(SRCK, HIGH);
                delayMicroseconds(1);
                digitalWrite(SRCK, LOW);
            }
            // latch the data to the internal registers after shifting out 16 bits
            digitalWrite(RCK, HIGH);
            delayMicroseconds(1);
            digitalWrite(RCK, LOW);
            // select the layer to light up
            if (i != 0) {
                digitalWrite(L1 + i - 1, 0);
            }
            digitalWrite(L1 + i, 1);
        }
        //delayMicroseconds(3000);  // 1 MHz clock
        delayMicroseconds(375);     // 8 MHz clock
        digitalWrite(L4, 0);
    }
}

void allOff() {
    /* set all LEDs in the cube to be off */
    for (byte i = 0; i < 64; i++) {
        cube[i] = 0;
    }
}

void allOn() {
    /* set all LEDs in the cube to be on */
    for (byte i = 0; i < 64; i++) {
        cube[i] = 1;
    }
}

// --------------------- Pattern Functions ---------------------
void spiralingColumns(int delay_ms) {
    /* starting at the top left column, the columns around the
     * perimeter, going clockwise, will sequentially all light
     * up before they all go off.
     */
    allOff(); // clear the cube
    // turn columns on
    for (byte i = 0; i < 12; i++) {
        cube[spiralColumns[i]] = 1;
        cube[spiralColumns[i] + 16] = 1;
        cube[spiralColumns[i] + 32] = 1;
        cube[spiralColumns[i] + 48] = 1;
        displayCube(delay_ms);
    }
    // turn columns off
    for (byte i = 0; i < 12; i++) {
        cube[spiralColumns[i]] = 0;
        cube[spiralColumns[i] + 16] = 0;
        cube[spiralColumns[i] + 32] = 0;
        cube[spiralColumns[i] + 48] = 0;
        displayCube(delay_ms);
    }
}

void lightUpSpiral(int delay_ms) {
    /* light up the cube, starting with a center 2x2x4 column,
     * then the outside light up, 1 LED at a time
     */
    byte inside[4] = {53, 54, 57, 58};
    // light up the inside 2x2 column
    for (byte i = 0; i < 64; i += 16) {
        for (byte j = 0; j < 4; j++) {
            cube[inside[j] - i] = 1;
            displayCube(delay_ms);
        }
    }
    // light up the exterior
    for (byte i = 0; i < 64; i += 16) {
        for (byte j = 0; j < 12; j++) {
            cube[spiralColumns[j] + i] = 1;
            displayCube(delay_ms / 2);
        }
    }
    //displayCube(1000);
    allOff();
}

void rainDrop(int delay_ms) {
    /* LEDs "randomly" begin to fall from the top to the bottom.
     * Each drop consists of 2 stacked LEDs and there should be 4
     * total LEDs on at a time.
     */
    int8_t leds[3][2]; // keep track of up to 3 drops at a time
    byte available[3] = {1, 1, 1}; // keeps track of whether or not leds[i] is free mor not
    byte next_idx = 0, done = 0, count = 0;
    byte i, j;

    // init leds array
    for (i = 0; i < 3; i++) {
        leds[i][0] = -1;
        leds[i][1] = -1;
    }
    allOff(); // clear the cube
    while (!done) {
        // shift the available leds left 1 index
        for (i = 0; i < 3; i++) {
            leds[i][0] = leds[i][1];
            if (leds[i][1] < 48 && leds[i][1] != -1) {
                leds[i][1] += 16;
            } else {
                leds[i][1] = -1;
            }
            if (leds[i][0] == -1) {
                available[i] = 1;
            }
        }
        if (count == 0) {
            // add a new column to led cube; find an available array
            for (i = 0; i < 3; i++) {
                if (available[i]) {
                    if (next_idx < 16) {
                        available[i] = 0;
                    }
                    break;
                }
            }
            if (next_idx < 16) {
                leds[i][1] = randomColumns[next_idx];
            } else if (next_idx == 16) {
                leds[i][1] = randomColumns[0];
            }
            next_idx++;
        }
        count = (count + 1) % 2;
        // display the rain
        allOff();
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 2; j++) {
                if (leds[i][j] != -1) {
                    cube[leds[i][j]] = 1;
                }
            }
        }
        displayCube(delay_ms);

        if (available[0] && available[1] && available[2]) {
            done = 1;
        }
    }
}

void LRSweep(int delay_ms) {
    /* A wall of LEDs, starting from the left side, move to the
     * right before bouncing back to where they came from.
     */
    // make LEDs go right
    int8_t i, j;

    for (i = 0; i < 4; i++) {
        allOff();
        // turn on the wall of LEDs
        for (j = 0; j < 64; j += 4) {
            cube[i + j] = 1;
        }
        displayCube(delay_ms);
    }
    // make LEDs go left
    for (i = 2; i >= 0; i--) {
        for (j = 0; j < 64; j += 4) {
            cube[i + j + 1] = 0;
            cube[i + j] = 1;
        }
        // if repeating LRSweep, continued > 0, so don't display the first
        // wall again because when it loops back, it displays 2x the time duration
        displayCube(delay_ms);
    }
}

void FBSweep(int delay_ms) {
    /* A wall of LEDs, starting from the front, goes to the back
     * before bouncing back to the front.
     */
    // go forward
    int8_t i, j, k;

    for (i = 0; i < 16; i += 4) {
        allOff();
        for (j = 0; j < 64; j += 16) {
            for (k = 0; k < 4; k++) {
                cube[i + j + k] = 1;
            }
        }
        displayCube(delay_ms);
    }
    // go backwards
    for (i = 12; i >= 0; i -= 4) {
        for (j = 0; j < 64; j += 16) {
            for (k = 0; k < 4; k++) {
                cube[i + j + k + 4] = 0;
                cube[i + j + k] = 1;
            }
        }
        displayCube(delay_ms);
    }
}

void UDSweep(int delay_ms) {
    /* A wall of LEDs, starting at the top, goes down and up again
     */
    // go down
    int8_t i, j;

    for ( i = 0; i < 4; i++) {
        allOff();
        for ( j = 0; j < 16; j++) {
            cube[j + i * 16] = 1;
        }
        displayCube(delay_ms);
        //displayCube(1000);
    }
    // go up
    for ( i = 2; i >= 0; i--) {
        for ( j = 0; j < 16; j++) {
            cube[j + (i + 1) * 16] = 0;
            cube[j + i * 16] = 1;
        }
        displayCube(delay_ms);
    }
}

void startAnimation() {
    int8_t i, j;
    int8_t speed1 = 15, speed2 = 50, speed3 = 35;

    allOff();
    // starting from cube[63] (bottom corner) to cube[0], light up all LEDs
    for (i = 63; i >= 0; i--) {
        cube[i] = 1;
        displayCube(speed1);
    }
    // flash cube, slow then fast
    /*for (i = 80; i >= 20; i -= 30) {
        for (j = 0; j < 5; j++) {
            allOff();
            displayCube(i);
            allOn();
            displayCube(i);
        }
    }*/
    displayCube(500);
    // sequentially turn off LEDs, starting from cube[0] - cube[63],
    // but leave the center 2x2 cube lit up
    for (i = 0; i < 64; i++) {
        if (i != 21 && i != 22 && i != 25 && i != 26 &&
            i != 37 && i != 38 && i != 41 && i != 42) {
            cube[i] = 0;
            displayCube(speed1);
        } else {
            displayCube(speed1);
        }
    }
    displayCube(500);
    // move the 2x2 cube up, one corner column at a time
    for (i = 21; i < 27; i++) {
        if (cube[i] == 1) {
            cube[i - 16] = 1;
            cube[i + 16] = 0;
            displayCube(speed2);
            cube[i] = 0;
            displayCube(speed2);
            cube[i - 16] = 0;
            displayCube(speed2);
        }
    }
    // have 1x4 corner columns move bottom up, 1 at a time
    byte corners[4] = {48, 51, 60, 63};
    for (i = 0; i < 4; i++) {
        // turn on 1 column, bottom up
        for (j = 0; j < 64; j += 16) {
            cube[corners[i] - j] = 1;
            displayCube(speed3);
        }
        // turn off the column, bottom up
        for (j = 0; j < 64; j += 16) {
            cube[corners[i] - j] = 0;
            displayCube(speed3);
        }
    }
}

void growShrinkBox(byte start_idx, byte end_idx, int delay_ms) {
    /* starts with 1 led at a corner, then it grows to a full 4x4 box
     * where all the corners and edges of the cube are lit up. Start
     * and end can be any value between 0 and 7; adaptive pattern
     */
    byte corners0[8] = {0, 3, 12, 15, 48, 51, 60, 63}; // 1 LED
    byte corners1[8] = {0, 2,  8, 10, 32, 34, 40, 42}; // top back left corner
    byte corners2[8] = {0, 1, 4, 5, 16, 17, 20, 21};   // top back left corner
    byte centers2[6] = {5, 17, 20, 22, 25, 37};        // remove 3x3 centers
    byte columns3[4] = {16, 19, 28, 31};               // light up columns, full box

    allOff();
    // >>> grow
    // 1. light up just 1 LED
    cube[corners0[start_idx]] = 1;
    displayCube(delay_ms);

    // 2. light up a 2x2 block
    for (byte i = 0; i < 32; i += 16) {
        for (byte j = 0; j <8; j += 4) {
            for (byte k = 0; k < 2; k++) {
                cube[corners1[start_idx] + i + j + k] = 1;
            }
        }
    }
    displayCube(delay_ms);

    // 3. light up 3x3 block without center
    for (byte i = 0; i < 48; i += 16) {
        for (byte j = 0; j <12; j += 4) {
            for (byte k = 0; k < 3; k++) {
                cube[corners2[start_idx] + i + j + k] = 1;
            }
        }
    }
    cube[corners2[start_idx] + 21] = 0; // turn off the center of the 3x3 cube
    // turn off the center of the faces
    for (byte i = 0; i < 6; i++) {
        cube[corners2[start_idx] + centers2[i]] = 0;
    }
    displayCube(delay_ms);

    // 4. light up 4x4 box
    allOff();
    for (byte i = 0; i < 12; i++) {
        cube[spiralColumns[i]] = 1;
        cube[spiralColumns[i] + 48] = 1;
    }
    for (byte i = 0; i < 4; i++) {
        cube[columns3[i]] = 1;
        cube[columns3[i] + 16] = 1;
    }
    displayCube(delay_ms);

    // >>> shrink
    // 1. light up 3x3 block without center
    allOff();
    for (byte i = 0; i < 48; i += 16) {
        for (byte j = 0; j <12; j += 4) {
            for (byte k = 0; k < 3; k++) {
                cube[corners2[end_idx] + i + j + k] = 1;
            }
        }
    }
    cube[corners2[end_idx] + 21] = 0; // turn off the center of the 3x3 cube
    // turn off the center of the faces
    for (byte i = 0; i < 6; i++) {
        cube[corners2[end_idx] + centers2[i]] = 0;
    }
    displayCube(delay_ms);

    // 2. light up a 2x2 block
    allOff();
    for (byte i = 0; i < 32; i += 16) {
        for (byte j = 0; j <8; j += 4) {
            for (byte k = 0; k < 2; k++) {
                cube[corners1[end_idx] + i + j + k] = 1;
            }
        }
    }
    displayCube(delay_ms);

    // 3. light up just 1 LED
    allOff();
    cube[corners0[end_idx]] = 1;
    displayCube(delay_ms);
}

void singleDrop() {
    int8_t i, j;

    // light up top layer
    for (i = 0; i < 16; i++) {
        cube[i] = 1;
    }
    // let 1 random LED drop from top to the bottom layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 48; j+= 16) {
            cube[randomColumns[i] + j] = 0;
            cube[randomColumns[i] + j + 16] = 1;
            displayCube(60);
        }
    }
    displayCube(1000);
    // go up now, but much faster
    for (i = 0; i < 16; i++) {
        for (j = 48; j >= 0; j-= 16) {
            cube[randomColumns[i] + j] = 1;
            cube[randomColumns[i] + j + 16] = 0;
            displayCube(5);
        }
        cube[randomColumns[i]] = 0;
        displayCube(20);
    }
    displayCube(1000);
}

void randomWalk(int8_t distance, int delay_ms) {
    int8_t line[4] = {-1, -1, -1, 48};
    int8_t i, j, tmp, valid = 0;

    for (i = 0; i < distance; i++) {
        // move elements down 1 index
        for (j = 0; j < 2; j++) {
            line[j] = line[j + 1];
        }
        do {
            tmp = (int8_t) random(0, 6);
            if (tmp == 0) {        // up
                line[3] = tmp - 16;
            } else if (tmp == 1) { // down

            } else if (tmp == 2) { // left

            } else if (tmp == 3) { // right

            } else if (tmp == 4) { // front

            } else {               // back

            }
        } while (!valid);
    }

}

// ----------------------------------------------------------------
// ------------------------- Program Main -------------------------
void setup() {
    byte corners[8] = {0, 3, 12, 15, 48, 51, 60, 63};
    byte i;

    allOff();
    for (i = 0; i < 7; i++) {
        pinMode(i, OUTPUT);
        cube[corners[i]] = 1;
    }
    displayCube(2000);
}

void loop() {
    byte i;
    byte sequence[8] = {6, 7, 0, 4, 3, 1, 2, 5};

    startAnimation();
    lightUpSpiral(80);
    
    for (i = 0; i < 3; i++) {
        spiralingColumns(60 - (i*11));
    }
    for (i = 0; i < 3; i++) {
        rainDrop(80);
    }
    for (i = 0; i < 3; i++) {
        LRSweep(80);
    }
    for (i = 0; i < 3; i++) {
        FBSweep(80);
    }
    for (i = 0; i < 3; i++) {
        UDSweep(80);
    }
    for (i = 0; i < 7; i++) {
        growShrinkBox(sequence[i], sequence[i + 1], 80);
    }
    singleDrop();
}
