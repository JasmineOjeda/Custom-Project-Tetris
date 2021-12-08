#ifndef __BLOCKS_H__
#define __BLOCKS_H__

#include "ledmatrix7219d88.h"
#include "nokia5110.h"

uint8_t rows[8] = {
                        0b10000000,
                        0b01000000,
                        0b00100000,
                        0b00010000,
                        0b00001000,
                        0b00000100,
                        0b00000010,
                        0b00000001
};

struct Block1 {
    uint8_t cur_col1;
    uint8_t cur_row1;
    uint8_t cur_col2;
    uint8_t cur_row2;
} block1;

void moveBlock1(unsigned char down, unsigned char left, unsigned char right) {
    if (right || left) {
        ledmatrix7219d88_setrow(0, block1.cur_col1 + 1, 0b00000000);
        ledmatrix7219d88_setrow(0, block1.cur_col1 - 1, 0b00000000);
	ledmatrix7219d88_setrow(0, block1.cur_col2 + 1, 0b00000000);
        ledmatrix7219d88_setrow(0, block1.cur_col2 - 1, 0b00000000);
    }

    if (down) {
	ledmatrix7219d88_setrow(0, block1.cur_col1, 0b00000000);
        block1.cur_col1--;
	ledmatrix7219d88_setrow(0, block1.cur_col2, 0b00000000);
	block1.cur_col2--;
    }
    else if (right) {
        block1.cur_row1++;
        block1.cur_row2++;
    }
    else if (left) {
        block1.cur_row1--;
	block1.cur_row2--;
    }

    ledmatrix7219d88_setrow(0, block1.cur_col1, rows[block1.cur_row1]);
    ledmatrix7219d88_setrow(0, block1.cur_col2, rows[block1.cur_row2]);
}

#endif
