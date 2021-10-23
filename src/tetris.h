/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 HereweTech Co.LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file tetris.h
 * @author Dr.NP <conan.np@gmail.com>
 * @since 10/17/2021
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>

// Definations
#define APP_NAME                        "Tetris"
#define APP_VERSION                     "0.0.1-1024@spec"

#define SCENE_MIN_WIDTH                 80
#define SCENE_MIN_HEIGHT                25
#define SPLASH_TITLE_WIDTH              81
#define SPLASH_TITLE_HEIGHT             18
#define PLAYGROUND_WIDTH                16
#define PLAYGROUND_HEIGHT               30
#define TOPIC_BOX_WIDTH                 40
#define TOPIC_BOX_HEIGHT                11
#define MSG_BOX_WIDTH                   16
#define MSG_BOX_HEIGHT                  7
#define NEXT_BOX_WIDTH                  16
#define NEXT_BOX_HEIGHT                 9
#define TRACE_BOX_WIDTH                 16
#define TRACE_BOX_HEIGHT                7

#define DEFAULT_TETRIS_LEVEL            3
#define MIN_TETRIS_LEVEL                1
#define MAX_TETRIS_LEVEL                9

#define EGG_SCORE                       1024

/* {{{ Structures */

// Scene
enum scene_status_e
{
    STATUS_PREPARE,
    STATUS_PLAYING,
    STATUS_OVER,
    STATUS_EGG,
};

struct tetris_scene_t
{
    int                 win_width;
    int                 win_height;
    enum scene_status_e status;
    int                 score;
    int                 level;
    int                 blocks;
    int                 speed;
    char                playground[PLAYGROUND_HEIGHT][PLAYGROUND_WIDTH];
};

static char *title_t[] = {
    "111111111111",
    "111111111111",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
};

static char *title_e[] = {
    "111111111111",
    "111111111111",
    "111100000000",
    "111100000000",
    "111111111111",
    "111111111111",
    "111100000000",
    "111100000000",
    "111111111111",
    "111111111111",
};

static char *title_r[] = {
    "111111111100",
    "111111111110",
    "111100001111",
    "111100000111",
    "111100001111",
    "111111111110",
    "111111111111",
    "111100001111",
    "111100000111",
    "111100000111",
};

static char *title_i[] = {
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
    "000011110000",
};

static char *title_s[] = {
    "000111111000",
    "011111111110",
    "111100000111",
    "011110000000",
    "000111111000",
    "000000111110",
    "000000001111",
    "111000001111",
    "011111111110",
    "000111111000",
};

static char *title_1[] = {
    "00011000",
    "00111000",
    "00011000",
    "00011000",
    "00011000",
    "00011000",
    "00111100",
};

static char *title_0[] = {
    "00111100",
    "01100110",
    "11000011",
    "11000011",
    "11000011",
    "01100110",
    "00111100",
};

static char *title_2[] = {
    "00111100",
    "01100110",
    "00000011",
    "00000110",
    "00011000",
    "01100000",
    "11111111",
};

static char *title_4[] = {
    "00001100",
    "00111100",
    "11001100",
    "11001100",
    "11111111",
    "00001100",
    "00011110",
};

static char *title_heart[] = {
    "0000111001110000",
    "0011111111111100",
    "0111111111111110",
    "1111111111111111",
    "0111111111111110",
    "0001111111111000",
    "0000011111100000",
    "0000000110000000",
};

static char *smile_failure[] = {
    "0000011111111111100000",
    "0011111111111111111100",
    "1110011001111001100111",
    "1111100111111110011111",
    "1110011001111001100111",
    "1111111111111111111111",
    "1111110000000000111111",
    "0011100111111110011100",
    "0001111111111111111000",
    "0000001111111111000000",
};

// Blocks
enum block_type_e {
    BLOCK_UNKNOWN,
    BLOCK_L,
    BLOCK_S,
    BLOCK_J,
    BLOCK_I,
    BLOCK_Z,
    BLOCK_O,
    BLOCK_T,
};

enum block_direction_e {
    BLOCK_DIR_0,
    BLOCK_DIR_90,
    BLOCK_DIR_180,
    BLOCK_DIR_270,
};

struct pos_t {
    int x;
    int y;
};

typedef struct tetris_block_t {
    enum block_type_e
                        type;
    enum block_direction_e
                        direction;
    struct pos_t        pos;
    int                 tile;
    int                 color;
    bool                dropped;
} BLOCK;

// Block maps
static int block_L[4] = {
    /*
        . B . .    . . . .    B B . .    . . B .
        . X . .    B X B .    . X . .    B X B .
        . B B .    B . . .    . B . .    . . . .
        . . . .    . . . .    . . . .    . . . .
    */
    0b0100010001100000, 0b0000111010000000, 0b1100010001000000, 0b0010111000000000,
    // Reverse
    // 0b0000011001000100, 0b0000100011100000, 0b0000010001001100, 0b0000000011100010
};

static int block_S[4] = {
    /*
        . B B .    . B . .    . . . .    B . . .
        B X . .    . X B .    . X B .    B X . .
        . . . .    . . B .    B B . .    . B . .
        . . . .    . . . .    . . . .    . . . .
    */
    0b0110110000000000, 0b0100011000100000, 0b0000011011000000, 0b1000110001000000,
    // Reverse
    // 0b0000000011000110, 0b0000001001100100, 0b0000110001100000, 0b0000010011001000
};

static int block_J[4] = {
    /*
        . B . .    B . . .    . B B .    . . . .
        . X . .    B X B .    . X . .    B X B .
        B B . .    . . . .    . B . .    . . B .
        . . . .    . . . .    . . . .    . . . .
    */
    0b0100010011000000, 0b1000111000000000, 0b0110010001000000, 0b0000111000100000,
    // Reverse
    // 0b0000110001000100, 0b0000000011101000, 0b0000010001000110, 0b0000001011100000
};

static int block_I[4] = {
    /*
        . B . .    . . . .    . . B .    . . . .
        . B . .    B B B B    . . B .    . . . .
        . B . .    . . . .    . . B .    B B B B
        . B . .    . . . .    . . B .    . . . .
    */
    0b0100010001000100, 0b0000111100000000, 0b0010001000100010, 0b0000000011110000,
    // Reverse
    // 0b0100010001000100, 0b0000000011110000, 0b0010001000100010, 0b0000111100000000
};

static int block_Z[4] = {
    /*
        B B . .    . . B .    . . . .    . B . .
        . X B .    . X B .    B X . .    B X . .
        . . . .    . B . .    . B B .    B . . .
        . . . .    . . . .    . . . .    . . . .
    */
    0b1100011000000000, 0b0010011001000000, 0b0000110001100000, 0b0100110010000000,
    // Reverse
    // 0b0000000001101100, 0b0000010001100010, 0b0000011011000000, 0b0000100011000100
};

static int block_O[4] = {
    /*
        . B B .    . B B .    . B B .    . B B .
        . B B .    . B B .    . B B .    . B B .
        . . . .    . . . .    . . . .    . . . .
        . . . .    . . . .    . . . .    . . . .
    */
    0b0110011000000000, 0b0110011000000000, 0b0110011000000000, 0b0110011000000000,
    // Reverse
    // 0b0000000001100110, 0b0000000001100110, 0b0000000001100110, 0b0000000001100110
};

static int block_T[4] = {
    /*
        B B B .    . . B .    . . . .    B . . .
        . X . .    . X B .    . X . .    B X . .
        . . . .    . . B .    B B B .    B . . .
        . . . .    . . . .    . . . .    . . . .
    */
    0b1110010000000000, 0b0010011000100000, 0b0000010011100000, 0b1000110010000000,
    // Reverse
    // 0b0000000001001110, 0b0000001001100010, 0b0000111001000000, 0b0000100011001000
};

/* }}} */

// FUnctions

// Create tetris block by given type
BLOCK * new_block();

// Delete (free) block
void del_block(BLOCK *);

// Rotate block
int try_rotate_block(BLOCK *, bool, enum block_direction_e *);

// Check solid
bool check_block_solid(BLOCK *, int, int, int, int, int);

// Generate random integer from urandom device
unsigned int get_random();

// Level => Speed rate
int calculate_speed(int);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
