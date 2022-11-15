#include "virtual_space.h"
#include <Arduino.h>
#include <algorithm>
#include <iterator>

extern "C" void *MALLOC32(int size, const char *name);

VSpace::VSpace(uint8_t **__lines, int w, int h) : _lines(__lines), true_width(w), true_height(h)
{
    printf("VSpace init with %d x %d\n", w, h);
    // printf("Sizeof _lines %d", sizeof(__lines));
    _state();

    // prev_lines = (uint8_t**)MALLOC32(true_width*true_height*sizeof(uint8_t*),"prev_lines");
    // lines_size = true_height*sizeof(uint8_t*);
}

VSpace::VSpace(uint8_t **__lines, uint8_t **_prev_lines, int w, int h) : _lines(__lines), prev_lines(_prev_lines), true_width(w), true_height(h)
{
    printf("VSpace init with %d x %d\n", w, h);
    // printf("Sizeof _lines %d", sizeof(__lines));
    _state();

    // prev_lines = (uint8_t**)MALLOC32(true_width*true_height*sizeof(uint8_t*),"prev_lines");
    // lines_size = true_height*sizeof(uint8_t*);
}

void VSpace::_state()
{
    width = true_width - w_offset_start - w_offset_end;
    height = true_height - h_offset_start - h_offset_end;
    inv_width = 1.0f / width;
    inv_height = 1.0f / height;
    // printf("VSpace state update with %d x %d\n", width, height);
}

void VSpace::set_xy(int x, int y, uint8_t value)
{

    int final_x, final_y;
    final_x = h_offset_start + x;
    final_y = w_offset_start + y;
    if (
        final_y < width &&
        final_x < height)
    {
        _lines[final_x][final_y] = value;
    }
}

void VSpace::set_xy(float x, float y, uint8_t value)
{
}

int VSpace::color_scale(uint8_t value, uint8_t min, uint8_t max)
{
    return min + ((max - min) * value / 255);
    // return map(value, 0, 255, min, max);
}

int VSpace::color_cut(uint8_t value, uint8_t cut, uint8_t width = 1)
{
    if (cut == 0) {
        return value;
    }
    if (value % cut < width)
    {
        return 255-value;
    }
    else
    {
        return value;
    }
    // return min + range*value;
}

float InvSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;             // store floating-point bits in integer
    i = 0x5f3759df - (i >> 1);      // initial guess for Newton's method
    x = *(float *)&i;               // convert new bits into float
    x = x * (1.5f - xhalf * x * x); // One round of Newton's method
    return x;
}

void VSpace::Mirrors(bool vertical = true, bool horizontal = true)
{
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            if (i < height / 2)
            {
                //_lines[i][j] = _lines[i - height/2][j];
                _lines[i][j] = _lines[height - i][j];
            }

            if (j < width / 2)
            {
                //_lines[i][j] = _lines[i][j - width/2];
                _lines[i][j] = _lines[i][width - j];
            }
        }
    }
    /*
    if (i > height/2) {
        //_lines[i][j] = _lines[i - height/2][j];
        _lines[i][j] = _lines[height - i][j];
    }

    if (j > width/2) {
        //_lines[i][j] = _lines[i][j - width/2];
        _lines[i][j] = _lines[i][width - j];
    }*/
}

void VSpace::SquareScan(bool fill = true, bool invert = true, bool wipe = false)
{
    _time_loop = map(_time_loop, 0, _time_loop_period, 0, 50);
    int w1, w2, h1, h2;
    w1 = width / 2 - _time_loop;
    w2 = width / 2 + _time_loop;
    h1 = height / 2 - _time_loop;
    h2 = height / 2 + _time_loop;
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            if (j == w1 || j == w2 || i == h1 || i == h2)
            {
                _lines[i][j] = 15; //+(16*(frame_counter%16));
            }
            else if (
                fill && ((!invert && (j < w2 && j > w1 && i < h2 && i > h1)) ||
                         (invert && !(j < w2 && j > w1 && i < h2 && i > h1))))
            {
                //_lines[i][j] = color_scale(_time_loop % 256, 134, 206);
                _lines[i][j] = color_cut(color_scale(_time_loop % 256, _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
                //_lines[i][j] = 15+(16*(frame_counter%16));
            }
            else if (wipe || _time_loop == 0)
            {
                _lines[i][j] = 0;
            }
        }
    }
}

void VSpace::TriangleScan(bool fill = false, bool invert = false, bool wipe = false)
{
    _time_loop = map(_time_loop, 0, _time_loop_period, 0, 50);
    fill = _preset_switch1;
    wipe = _preset_switch2;
    invert = _slit_mode;
    int a, b, c;
    a = height / 2 + _time_loop;
    b = (width + height) / 2 - _time_loop;
    c = (width - height) / 2 + _time_loop;
    // h2 = height/2 + _mouv;
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            if (i == a || i + j == b || j - i == c)
            {
                _lines[i][j] = 15; //+(16*(frame_counter%16));
            }
            else if (
                fill && ((!invert && (i < a && i + j > b && j - i < c)) ||
                         (invert && !(i < a && i + j > b && j - i < c))))
            {
                //_lines[i][j] = color_scale(frame_counter % 256, 134, 206);
                _lines[i][j] = color_cut(color_scale(frame_counter % 256, _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
                //_lines[i][j] = 15+(16*(frame_counter%16));
            }
            else if (wipe || _time_loop == 0)
            {
                _lines[i][j] = 0;
                //_lines[i][j] &= prev_lines[abs(i-j)%height][abs(j-j)%width];
            }
        }
    }
}

void VSpace::LinesField(short v_dir = 1, short h_dir = 1)
{
    uint8_t value = 0;
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            value = ((v_dir * i) + (h_dir * j) + (_time_loop)) / 2;
            _lines[i][j] = color_cut(color_scale(value, _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
        }
    }
}

void VSpace::_reset(bool erase)
{
    //frame_counter = 0;
    for (int i = 0; i < 16; i++)
    {
        VMEM[i] = 0;
    }
    if (erase)
    {
        std::fill(&_lines[0][0], &_lines[0][0] + true_height * true_width, 0);
        // memset(_lines, 0, height*sizeof(uint8_t*));
        // memset(prev_lines, 0, height*sizeof(uint8_t*));
    }
}

void VSpace::NLinesScan(bool vertical = true, bool horizontal = false, bool wipe = false)
{
    vertical = _preset_switch1;
    horizontal = _preset_switch2;
    //wipe = _preset_switch1 == 2 || _preset_switch2 == 2;
    uint8_t N = map(_preset_pot, 0, 4096, 1, 20);

    int w_s = width / N;
    int h_s = height / N;
    if (VMEM[0] == 0)
        VMEM[0] = random(0,256);
        VMEM[0] = color_cut(color_scale(VMEM[0], _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
    if (VMEM[1] == 0)
        VMEM[1] = random(0,256);
        VMEM[1] = color_cut(color_scale(VMEM[1], _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            if (vertical)
            {

                if ((j + (frame_counter % w_s)) % w_s == 0)
                {
                    _lines[i][j] = VMEM[0]; // color_scale(j,100,128);
                }
                if (_time_loop % w_s == w_s - 1)
                {
                    _lines[i][j] = 0;
                    VMEM[0] = random(0,256); //16 * random(0, 16) - random(0, 6);
                    VMEM[0] = color_cut(color_scale(VMEM[0], _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
                    _time_loop = 0;
                    _reset(wipe);
                }
            }
            if (horizontal)
            {
                if ((i + (frame_counter % h_s)) % h_s == 0)
                {
                    _lines[i][j] = VMEM[1]; // color_scale(j,100,128);
                }
                if (_time_loop % h_s == h_s - 1)
                {
                    _lines[i][j] = 0;
                    VMEM[1] = random(0,256); //16 * random(0, 16) - random(0, 6);
                    VMEM[1] = color_cut(color_scale(VMEM[1], _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
                    _reset(wipe);
                    _time_loop = 0;
                }
            }
        }
    }
}

void VSpace::SquareInvasion(bool vertical = true, bool horizontal = true)
{
    uint8_t value = 0;
    if (_time_loop == 0)
    {
        //VMEM[0] = 16 * random(0, 16) - random(0, 6);
        VMEM[0] = color_cut(color_scale(random(0,256), _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
        VMEM[1] = random(0, width);
        VMEM[2] = random(VMEM[1], width);
        VMEM[3] = random(0, height);
        VMEM[4] = random(VMEM[3], height);
        VMEM[5] = 0;            // drawn
        VMEM[6] = millis() % 5; // mode (add / xor)
        VMEM[7] = 0;
    }

    if (!VMEM[5])
    {
        for (int i = 0; i < height; i += 1)
        {
            for (int j = 0; j < width; j += 1)
            {

                bool cond = true; //(i > VMEM[3] && i < VMEM[4] && j > VMEM[1] && j < VMEM[2]);
                if (horizontal)
                    cond = (i > VMEM[3] && i < VMEM[4]);
                if (vertical)
                    cond &= (j > VMEM[1] && j < VMEM[2]);

                if ((VMEM[6] < 3) && cond)
                {
                    _lines[i][j] = VMEM[0];
                    VMEM[5] = 1;
                }
                else if ((VMEM[6] == 3) && cond)
                {
                    if (_lines[i][j] != 0)
                        _lines[i][j] |= VMEM[0];
                    VMEM[5] = 1;
                }
                else if ((VMEM[6] == 4) && cond)
                {
                    _lines[i][j] = 0;
                    VMEM[5] = 1;
                }

                if (_lines[i][j] != 0)
                {
                    VMEM[7] += 1;
                }
            }
        }
    }
}

void VSpace::XORField()
{
    int a = 0;
    if (_time_loop == 0)
    {
        VMEM[0] = 16 * random(0, 16) - random(0, 8);
    }

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            //_lines[i][j] = (_lines[i][j] + prev_lines[i][j])/3;
            /*if(_mouv == i) {
                _lines[i][j] = color_scale(frame_counter%255, 134, 160);
            } else {
                //_lines[i][j] ^= prev_lines[abs(i-j)%height][abs(j-i)%width];
                _lines[i][j] ^= prev_lines[abs(i&j)%height][abs(i|j)%width];
            }*/

            if ((i ^ j) == _time_loop)
            {
                _lines[i][j] = VMEM[0];
            }
            else if (_time_loop == 0)
            {
                _lines[i][j] = 0;
            }
            //_lines[i][j] ^= prev_lines[abs(i-j)%height][abs(j-i)%width];
            //_lines[i][j] ^= prev_lines[abs(i&j)%height][abs(i|j)%width];
            //}

            //_lines[i][j] = color_scale((i/(_mouv+1))^(j/(_mouv+1)), 134, 206);
            //_lines[i][j] = color_scale((i/(_mouv+1))|(j/(_mouv+1)), 134, 206);
        }
    }
}

void VSpace::TestCard()
{

    /*if(_mouv == 0) {
        int p = VMEM[5];
        _reset(false);
        VMEM[5] = p+1;
    }

    if(VMEM[5] > 10) {
        _reset(true);
    }

    if (!VMEM[0]) {
        VMEM[0] = random(0, height);
        VMEM[1] = random(0, width);
        VMEM[4] = random(0,15);
        VMEM[2] = VMEM[4]*random(-1, 1);
        VMEM[3] = VMEM[4]*random(-1, 1);
        VMEM[6] = 15;
    }
    for(int i = 0; i < height; i+=1) {
        for(int j = 0; j < width; j+=1) {
            if ((i > VMEM[0] && i < (VMEM[0]+50) && j > VMEM[1] && j < (VMEM[1] + 50))) {
                if(_lines[i][j] == 0) {
                    _lines[i][j] = VMEM[6];
                } else {
                    _lines[i][j] ^= VMEM[6];
                }
            }

        }
    }
    VMEM[0] += VMEM[2];
    VMEM[1] += VMEM[3];*/

    unsigned int c = 0;
    for (int i = 0; i < 128; i += 8)
    {
        for (int j = 0; j < 128; j += 8)
        {
            for (int k = 0; k < 8; k++)
            {
                for (int l = 0; l < 8; l++)
                {
                    //_lines[64+i+k][64+j+l] = c;
                    _lines[i + k][j + l] = c;
                }
            }
            c++;
        }
    }
}

void VSpace::Linez()
{
    /*int offset = 100 * VINPUTS[1] / 4096;
    int n_lines = 100 * VINPUTS[1] / 4096;*/

    int n_lines = map(_preset_pot, 0, 4096, 50, 4);
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            int j_check = (j + _time_loop) % (n_lines * 2);
            int i_check = (i + _time_loop) % (n_lines * 2);
            /*if(
                (
                    (_preset_switch1 == 1) && (j+frame_counter)%(n_lines*2) < n_lines
                )
                &&
                !((i+frame_counter)%(n_lines*2) < n_lines))*/
            if (_preset_switch1 | _preset_switch2 == 1)
            //if (_preset_switch1 == 1 || _preset_switch2 == 1)
            {
                if (
                    ((_preset_switch1 != 0) && j_check < n_lines) ||
                    ((_preset_switch2 != 0) && i_check < n_lines))
                {
                    _lines[i][j] = _color_select_start;
                }
                else
                {
                    _lines[i][j] = 0;
                }
            }
            /*
            if ((_preset_switch1 == 2) || (_preset_switch2 == 2))
            {
                if (
                    ((_preset_switch1 != 0) && j_check == 0) ||
                    ((_preset_switch2 != 0) && i_check == 0))
                {
                    _lines[i][j] = 255;
                }
                else
                {
                    _lines[i][j] -= 1;
                }
            }*/

            /*if (_lines[i][j] != 0)
                _lines[i][j] = color_scale(_lines[i][j], _color_select_start, _color_select_end);*/
        }
    }
}


void VSpace::Linez2()
{
    int n_lines = 4*map(_preset_pot, 0, 4096, 50, 4);
    int w_lines = 2*map(_slit_index, 0, 4096, 0, 50) + 1;
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            int j_check = (j + _time_loop) % (n_lines * 2);
            int i_check = (i + _time_loop) % (n_lines * 2);
            
            
            //if ((_preset_switch1 == 1) || (_preset_switch2 == 1))
            if ((_preset_switch1) || (_preset_switch2))
            {
                if (
                    ((_preset_switch1 == 1) && j_check < w_lines) ||
                    ((_preset_switch2 == 1) && i_check < w_lines))
                {
                    _lines[i][j] = _color_select_start; //color_scale(_lines[i][j], _color_select_start, _color_select_end);;
                }
                else
                {
                    //_lines[i][j] -= 1;
                    _lines[i][j] = color_cut(color_scale(_lines[i][j] - 1, _color_select_start, _color_select_end), _color_cut_period, _color_cut_width);
                }
            }

            //if (_lines[i][j] != 0)
            //    _lines[i][j] = color_scale(_lines[i][j], _color_select_start, _color_select_end);
        }
    }
}

void VSpace::get_inputs()
{

    for (int i = 0; i < N_INPUTS; i++)
    {
        // VINPUTS[i] = (9*VINPUTS[i] + analogRead(PIN_INPUTS[i]))/10;
        VINPUTS[i] = analogRead(PIN_INPUTS[i]);
        /*if (frame_counter % 100 == 0)
        {
            printf("%d %d\n", i, VINPUTS[i]);
        }*/
    }

    _buttons = map(VINPUTS[0], 0, 4096, 0, 16); // all buttons
    // preset next : 5
    // preset prev : 10
    // white : 6 
    // black : 14

    if(_buttons == 5 && (frame_counter - _last_preset_switch) > 10) {
        _current_preset++;
        _last_preset_switch = frame_counter;
    } else if (_buttons == 10 && (frame_counter - _last_preset_switch) > 10) {
        _current_preset--;
        _last_preset_switch = frame_counter;
    }

    _preset_pot = 4095 - VINPUTS[6];                  // by preset
    _preset_switch1 = map(VINPUTS[3], 0, 4096, 0, 2); // tri switch
    _preset_switch2 = map(VINPUTS[2], 0, 4096, 0, 2); // tri switch

    

    //white button is pressed
    if(_buttons == 6) {
        _color_select_start = map(4095 - VINPUTS[10], 0, 4096, 0, 255); // 0 - 256
        _color_select_range = map(4095 - VINPUTS[12], 0, 4096, 0, 255); // 0 - 256
        _color_select_end = _color_select_start + _color_select_range;
        if (_color_select_end > 256) {
            _color_select_end = 255;
            _color_select_range = 255 - _color_select_start;
        }
    }

    _color_cut_period = map(4095 - VINPUTS[13], 0, 4096, 0, _color_select_range); // 0 - 256
    _color_cut_width = map(4095 - VINPUTS[11], 0, 4096, 0, _color_cut_period);  // 0 - _color_cut_period

    _time_loop_multiplier = map(4095 - VINPUTS[7], 0, 4096, 1, 10); // TBD
    if(_buttons == 14) {
    if (VINPUTS[8] == 0) {
        _time_loop_period = INT_MAX;
    } else {
        _time_loop_period = map(VINPUTS[8], 4096, 0, 2, 1000);
    }
    /*
    if (VINPUTS[8] < 4090)
    {
        _time_loop_period = 2 + (4095 - VINPUTS[8]); // 2 - TBD
    }
    else
    {
        _time_loop_period = INT_MAX;
    }*/
    }

                 // 0 - (width/height)
    _slit_mode = map(VINPUTS[4], 0, 4096, 0, 2); // tri switch

    if(_slit_mode == 1) {
        if (2*_time_loop < _time_loop_period) {
            _slit_index = VINPUTS[9];
        } else {
            _slit_index = 4095 - VINPUTS[9];
        }
    } else if (_slit_mode == 2) {

    } else {
        _slit_index = 4095 - VINPUTS[9];
    }

    _sym_vertical = map(VINPUTS[1], 0, 4096, 0, 2);   // tri switch
    _sym_horizontal = map(VINPUTS[5], 0, 4096, 0, 2); // tri switch

    

    if (frame_counter % 100 == 0)
    {
        printf("_preset_pot : %d\n", _preset_pot);
        printf("_preset_switch1 : %d\n", _preset_switch1);
        printf("_preset_switch2 : %d\n\n", _preset_switch2);

        printf("_color_cut_period : %d\n", _color_cut_period);
        printf("_color_cut_width : %d\n\n", _color_cut_width);

        printf("_color_select_start : %d\n", _color_select_start);
        printf("_color_select_range : %d\n\n", _color_select_range);

        printf("_time_loop_multiplier : %d\n", _time_loop_multiplier);
        printf("_time_loop_period : %d\n\n", _time_loop_period);

        printf("_slit_index : %d\n", _slit_index);
        printf("_slit_mode : %d\n\n", _slit_mode);

        printf("_sym_vertical : %d\n", _sym_vertical);
        printf("_sym_horizontal : %d\n\n", _sym_horizontal);

        printf("_buttons : %d\n", _buttons);
    }
}

void VSpace::update()
{
    get_inputs();
    frame_counter++;
    _time_loop = ((frame_counter * _time_loop_multiplier) % (2 + _time_loop_period));
    
    if (_sym_vertical) {
        //double_buf = true;
        true_width = width;
        width = width/2 + 2 ;
    }
    if (_sym_horizontal) {
        //double_buf = true;
        true_height = height;
        height = height/2 + 2;
    }

    switch (_current_preset%N_PRESETS) {
        case 0:
            LinesField();
            break;
        case 1:
            Linez2();
            break;
        case 2:
            NLinesScan();
            break;
        case 3:
            SquareScan();
            break;
        case 4:
            TriangleScan();
            break;
        case 5:
            SquareInvasion();
            break;
        default:
            TestCard();
    }

    
    // NLinesScan();
    // SquareScan();
    //Linez();
    
    
    //TestCard

    if (_sym_vertical) {
        width = true_width;
        for (int i = 0; i < height; i += 1)
        {
            for (int j = width/2; j <= width; j += 1)
            {
                _lines[i][j] = _lines[i][width - j];
            }
        }
    }

    if (_sym_horizontal) {   
        height = true_height;
        for (int i = height/2; i <= height; i += 1)
        {
            for (int j = 0; j < width; j += 1)
            {
                _lines[i][j] = _lines[height-i][j];
            }
        }
    }

    /*if(_slit_mode == 1) {  // horizontal
        double_buf = true;
        int __slit_index = map(_slit_index, 0, 4096, 0, height);
        for (int i = 0; i < height; i += 1)
        {
            if (i != __slit_index) {
                for (int j = 0; j < width; j += 1)
                {
                    _lines[i][j] = _lines[__slit_index][j];
                }
            }
            
        }
    }

    if(_slit_mode == 2) {  // vertical
        double_buf = true;
        int __slit_index = map(_slit_index, 0, 4096, 0, width);
        for (int j = 0; j < width; j += 1)
        {
            if (j != __slit_index) {
                for (int i = 0; i < height; i += 1)
                {
                    _lines[i][j] = _lines[i][__slit_index];
                }
            }
            
        }
    }*/

    if(_slit_mode == 0) {
        double_buf = false;
    }
    // SquareInvasion();
    //  TestCard();
    //  Mirrors();
    /*for(int i = 0; i < true_height; i++) {
        for(int j = 0; j < true_width; j++) {
            prev_lines[i][j] = _lines[i][j];
        }
    }*/
    if (double_buf)
        std::copy(&_lines[0][0], &_lines[0][0] + true_height * true_width, &prev_lines[0][0]);

    // Mirrors();
    // SquareScan();
    return;

    // uint8_t fc = ((frame_counter >> 1) << 1) / 2;

    // unsigned long start = millis();
    // for(int i = 0; i < height; i+=1) {
    //     set_xy(i,frame_counter%width, 255*sin(6.18*start/1000));
    // for(int j = 0; j < width; j+=1) {
    // set_xy(i,j,(frame_counter*3+i+j)/(j+1));
    // set_xy(i,j,255*sin(6.18*(start+i)/1000));
    //}
    //}
    // float x,y;

    // uint8_t t = (2*frame_counter)%240;

    // uint8_t t2 = 10+frame_counter%50;
    // uint8_t t2 = 60-frame_counter%50;
    int t2;
    int t2_scale = 120;
    if (frame_counter % (2 * t2_scale) < t2_scale)
    {
        t2 = frame_counter % t2_scale;
    }
    else
    {
        t2 = frame_counter % t2_scale;
        // t2 = t2_scale-frame_counter%t2_scale;
    }
    // t2 *= 10;

    uint8_t i_half, j_half;

    // std::copy(&_lines[0][0], &_lines[0][0]+true_width*true_height,&prev_lines[0][0]);
    /*unsigned int c = 0;
    for(int i = 0; i < 128; i+=8) {
        for(int j = 0; j < 128; j+=8) {
            for(int k = 0; k < 8; k++) {
                for(int l = 0; l < 8; l++) {
                    _lines[64+i+k][64+j+l] = color_cut(c, 5, 2);
                }
            }
            c++;

        }

    }*/

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            // i_half = i;//frame_counter%height;//height/2 - i;
            // j_half = j;//width/2 - j;
            /*x = (height/2.0) - ((float)i/height);
            y = (width/2.0) - ((float)j/width);
            set_xy(i, j, 16*sqrt((x*x) + (y*y)));*/
            // x = 0.5f - ((float)i/height);
            // y = 0.5f - ((float)j/width);
            // x = ((float)i/height);
            // y = ((float)j/width);

            // play with +/- i / j for direction
            // 0*i / j for hor/vert patterns

            /*if (abs(i-j) < i) {
            _lines[i][j] ^= _lines[abs(i-j)][j];
            }*/
            //_lines[i][j] = color_scale((uint8_t)(t*(i/10)*abs(i-j)/(j+1)), 134, 206);

            // set_xy(i, j, color_scale((uint8_t)((i+j+(frame_counter*2)))/(3), 134, 206));

            //_lines[i][j] = _lines[(i+j)%height][j];
            //_lines[(i+j)%height][j] = _lines[i][j];
            // if ((i > height/(1+(t/24))) && (i>j)) {
            // set_xy(i, j, _lines[abs(i-j)][j]);
            //    _lines[i][j] = _lines[abs(i-j)][j];
            //}

            /* if(j != 0) {
                 _lines[i][j] = color_scale((frame_counter*3 + i)/j, 134, 206);
             }
             else {
                 _lines[i][j] = 0;
             }*/

            //_lines[i][j] = color_scale((frame_counter*3 + i)/j, 134, 206);

            // if (j != 0) _lines[i][j] = color_scale((i*(i+frame_counter))/(t2 + j),134, 206);
            // if (random(0,30) == 0) _lines[i][random(0, width)] = (frame_counter/10);
            // if(i*(t2/10) > j) {
            //_lines[i][j] = color_scale((i/(t2+1))^(j/(t2+1)), 134, 206);
            // if (random(0,12) == 0) _lines[i][random(0, width)] = (frame_counter/100);

            /*
            //SCANLINES
            if ((i < t && i > t-30) || (0))
                _lines[i][j] = (frame_counter/10);
            if ((j < t && j > t-30) || (0))
                _lines[i][j] = (frame_counter/10);
            */

            //_lines[i][j] &= _lines[(i|j)%height][(j&i)%width];
            //_lines[i][j] &= _lines[(i&j)%height][(j&i)%width];
            //_lines[i][j] &= _lines[(i|j)%height][(j^i)%width];

            // FULL COLOR SCALE
            //_lines[i][j] = color_scale(_lines[i][j], 134, 206);

            // COOL
            // try changing bitwise op
            //_lines[i][j] = color_scale((i/(t2+1))|(j/(t2+1)), 134, 206);

            // VERY COOL
            //_lines[i][j] = color_cut(color_scale((uint8_t)((i+j+(frame_counter*2)))/(3), 134, 206), 5, 2);
            //_lines[i][j] = color_scale((uint8_t)((i-j+(frame_counter*3)))/(2), 100, 190);
            //_lines[i][j] = color_scale((uint8_t)((i+j+(frame_counter*2)))/(3), 134, 206);
            //_lines[i][j] = (uint8_t)((i+j+(frame_counter*2)))/(3);
            //_lines[i][j] = i+j;
            // if (j != 0) {
            //_lines[i][j] = color_scale((i*j)/(i+j+ 2 + (3*t2)), 134, 206);
            //_lines[i][j] = color_scale((i*j)/(2 + (3*t2)), 134, 206);
            //_lines[i][j] = color_cut(color_scale(((i*i)+(t2*t2))/(j), 134, 160), 32, 16);
            //_lines[i][j] = color_cut(color_scale(((i_half*i_half)+(t2*t2))/(1+j), 134, 160), 32, 16);
            //_lines[i][j] = color_scale(((i_half*i_half)+(t2*t2))/(1+j), 134, 160);
            //}
            // SYMETRIES
            /*if (i < height/2) {
                //_lines[i][j] = _lines[i - height/2][j];
                _lines[i][j] = _lines[height - i][j];
            }

            if (j < width/2) {
                //_lines[i][j] = _lines[i][j - width/2];
                _lines[i][j] = _lines[i][width - j];
            }*/

            /*if (i > height/2) {
                //_lines[i][j] = _lines[i - height/2][j];
                _lines[i][j] = _lines[height - i][j];
            }

            if (j > width/2) {
                //_lines[i][j] = _lines[i][j - width/2];
                 _lines[i][j] = _lines[i][width - j];
            }*/

            /*if (_lines[i][j] < 140) {
                _lines[i][j] = 0;
            }*/

            // x = ((float)i/height);
            // y = ((float)j/width);

            // int i_center = (i - height/2);
            // int j_center = (j - width/2);
            // if (j_center != 0) {
            //_lines[i][j] = 255*sqrt(x*x + y*y);
            // }
            // set_xy(i, j, (i+j)/30.0f);
            // set_xy(i, j, color_scale(32,32,(x+y)/2));
        }
    }

    /*for(int i = 0; i < true_height; i++) {
        memcpy(prev_lines[i], _lines[i], true_width);
    }*/

    // if(frame_counter%2 == 0) {
    /*for(int i = 0; i < height; i+=1) {
        for(int j = 0; j < width; j+=1) {
            /*x = (height/2.0) - ((float)i/height);
            y = (width/2.0) - ((float)j/width);
            set_xy(i, j, 16*sqrt((x*x) + (y*y)));*/
    // x = 0.5f - ((float)i/height);
    // y = 0.5f - ((float)j/width);
    // x = ((float)i/height);
    // y = ((float)j/width);
    /*if ((j+i < height) && (j-i > 0)) {
        _lines[i][j] = _lines[j+i][j-i];
    }*/

    //_lines[i][j] = prev_lines[(i+j)%height][abs(j-i)];
    /*if ((i > height/2) && (i>j)) {
        _lines[i][j] = 0;
    }*/
    // set_xy(i, j, (i+j)/30.0f);
    // set_xy(i, j, color_scale(32,32,(x+y)/2));
    //}
    //}
    // memcpy(prev_lines, _lines, lines_size);

    // memcpy(_lines, prev_lines, lines_size);
    // }
    /*unsigned int c = 0;
        for(int i = 0; i < 128; i+=8) {
            for(int j = 0; j < 128; j+=8) {
                for(int k = 0; k < 8; k++) {
                    for(int l = 0; l < 8; l++) {
                        set_xy(i+k, j+l, c);
                    }
                }
                c++;
            }
        }*/
    //_lines[231][349] = 127;
}
