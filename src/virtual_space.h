#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include <Arduino.h>
#include <algorithm>
#include <iterator>

class VSpace {
public:
    bool double_buf = false;
    uint8_t** _lines;
    uint8_t** prev_lines;
    uint8_t* Screen_atari;

    int VMEM[16] = {0};

    int lines_size;
    int true_width;
    int true_height;
    int width;
    int height;
    //preset for my PVM in NTSC
    int w_offset_start = 8;
    int h_offset_start = 11;
    int w_offset_end = 9;
    int h_offset_end = 2;
    short frame_counter = 0;
    short _mouv = 0;
    float inv_height, inv_width;
    VSpace(uint8_t** __lines, int w, int h);
    VSpace(uint8_t** __lines, uint8_t** _prev_lines, int w, int h);
    void update();
    void set_xy(int x, int y, uint8_t value);
    void set_xy(float x, float y, uint8_t value);
    int color_scale(uint8_t value, uint8_t min, uint8_t max);
    int color_cut(uint8_t value, uint8_t cut, uint8_t width);

    //generators
    void SquareScan(bool fill, bool invert, bool wipe);
    void TriangleScan(bool fill, bool invert, bool wipe);
    void NLinesScan(bool, bool, bool);
    void LinesField(short, short);
    void XORField();
    void SquareInvasion(bool, bool);
    void Mirrors(bool, bool);
    void TestCard();
    void _state();
    void _reset(bool);
};
