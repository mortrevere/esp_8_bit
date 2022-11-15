#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"

#ifndef vspace_hpp
#define vspace_hpp

#define N_INPUTS 14
#define N_PRESETS 6

class VSpace
{
public:
    bool double_buf = false;
    uint8_t **_lines;
    uint8_t **prev_lines;
    uint8_t *Screen_atari;

    int _current_preset = 0;
    int _last_preset_switch = 0;

    int PIN_INPUTS[N_INPUTS] = {36, 39, 34, 35, 32, 33, 26, 27, 14, 12, 13, 4, 2, 15};
    int VMEM[16] = {0};
    int VINPUTS[32] = {0};

    int _time_loop = 0;
    int _time_loop_multiplier = 0;
    int _time_loop_period = 0;

    int _preset_pot = 0;
    int _preset_switch1 = 0;
    int _preset_switch2 = 0;

    int _color_cut_period = 0;
    int _color_cut_width = 0;
    

    int _color_select_start = 0;
    int _color_select_range = 0;
    int _color_select_end = 0; //not a button, virtual value

    int _slit_index = 0;
    int _slit_mode = 0; // 0 off, 1 hor, 2 vert

    int _sym_vertical = 0; // 0 off, 1 first half, 2 last half
    int _sym_horizontal = 0; // 0 off, 1 first half, 2 last half

    int _buttons = 0;

    void get_inputs();

    int lines_size;
    int true_width;
    int true_height;
    int width;
    int height;
    // preset for my PVM in NTSC
    int w_offset_start = 8;
    int h_offset_start = 11;
    int w_offset_end = 9;
    int h_offset_end = 2;
    int frame_counter = 0;
    short _mouv = 0;
    float inv_height, inv_width;
    VSpace(uint8_t **__lines, int w, int h);
    VSpace(uint8_t **__lines, uint8_t **_prev_lines, int w, int h);
    void update();
    void set_xy(int x, int y, uint8_t value);
    void set_xy(float x, float y, uint8_t value);
    int color_scale(uint8_t value, uint8_t min, uint8_t max);
    int color_cut(uint8_t value, uint8_t cut, uint8_t width);

    // generators
    void SquareScan(bool fill, bool invert, bool wipe);
    void TriangleScan(bool fill, bool invert, bool wipe);
    void NLinesScan(bool, bool, bool);
    void LinesField(short, short);
    void XORField();
    void SquareInvasion(bool, bool);
    void Mirrors(bool, bool);
    void Linez();
    void Linez2();
    void TestCard();
    void _state();
    void _reset(bool);
};

#endif /* emu_hpp */