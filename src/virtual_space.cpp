#include "virtual_space.h"

extern "C"
void* MALLOC32(int size, const char* name);

VSpace::VSpace(uint8_t** __lines, int w, int h) : 
_lines(__lines), true_width(w), true_height(h) {
    printf("VSpace init with %d x %d\n", w, h);
    //printf("Sizeof _lines %d", sizeof(__lines));
    _state();

    //prev_lines = (uint8_t**)MALLOC32(true_width*true_height*sizeof(uint8_t*),"prev_lines");
    //lines_size = true_height*sizeof(uint8_t*);

}

void VSpace::_state() {
    width = true_width-w_offset_start-w_offset_end;
    height = true_height-h_offset_start-h_offset_end;
    inv_width = 1.0f/width;
    inv_height = 1.0f/height;
    //printf("VSpace state update with %d x %d\n", width, height);
}

void VSpace::set_xy(int x, int y, uint8_t value) {

    int final_x, final_y;
    final_x = h_offset_start+x;
    final_y = w_offset_start+y;
    if (
        final_y < width && 
        final_x < height
    ) {
        _lines[final_x][final_y] = value;
    }
}

void VSpace::set_xy(float x, float y, uint8_t value) {
    
}

int VSpace::color_scale(uint8_t value, uint8_t min, uint8_t max) {
    return map(value, 0, 255, min, max);
    //return min + range*value;
}
float InvSqrt(float x){
        float xhalf = 0.5f * x;
        int i = *(int*)&x;            // store floating-point bits in integer
        i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
        x = *(float*)&i;              // convert new bits into float
        x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
        return x;
    }

void VSpace::update() {
    frame_counter++;

    //uint8_t fc = ((frame_counter >> 1) << 1) / 2;
    
    //unsigned long start = millis();
    //for(int i = 0; i < height; i+=1) {
    //    set_xy(i,frame_counter%width, 255*sin(6.18*start/1000));
        //for(int j = 0; j < width; j+=1) {
            //set_xy(i,j,(frame_counter*3+i+j)/(j+1));
            //set_xy(i,j,255*sin(6.18*(start+i)/1000));
        //}
    //}
    float x,y;

    uint8_t t = frame_counter%240;

    //std::copy(&_lines[0][0], &_lines[0][0]+true_width*true_height,&prev_lines[0][0]);
    
    
    for(int i = 0; i < height; i+=1) {
        for(int j = 0; j < width; j+=1) {
            /*x = (height/2.0) - ((float)i/height);
            y = (width/2.0) - ((float)j/width);
            set_xy(i, j, 16*sqrt((x*x) + (y*y)));*/
            //x = 0.5f - ((float)i/height);
            //y = 0.5f - ((float)j/width);
            //x = ((float)i/height);
            //y = ((float)j/width);

            //play with +/- i / j for direction
            //0*i / j for hor/vert patterns

            // VERY COOL
            //_lines[i][j] = color_scale((uint8_t)((i+j+(frame_counter*2)))/(3), 134, 206);
            //_lines[i][j] = color_scale((uint8_t)((i-j+(frame_counter*3)))/(2), 100, 190);

            _lines[i][j] = color_scale((uint8_t)(t*abs(i-j)/(j+1)), 134, 206);


            //set_xy(i, j, color_scale((uint8_t)((i+j+(frame_counter*2)))/(3), 134, 206));
            
            //_lines[i][j] = _lines[(i+j)%height][j];
            //_lines[(i+j)%height][j] = _lines[i][j];
            //if ((i > height/(1+(t/24))) && (i>j)) {
                //set_xy(i, j, _lines[abs(i-j)][j]);
            //    _lines[i][j] = _lines[abs(i-j)][j];
            //}

            if (i < height/2) {
                _lines[i][j] = _lines[height - i][j];
            }

            if (j < width/2) {
                _lines[i][j] = _lines[i][width - j];
            }

            /*if (_lines[i][j] < 140) {
                _lines[i][j] = 0;
            }*/

            
            
            
            //set_xy(i, j, (i+j)/30.0f);
            //set_xy(i, j, color_scale(32,32,(x+y)/2));
        }
    }

    /*for(int i = 0; i < true_height; i++) {
        memcpy(prev_lines[i], _lines[i], true_width);
    }*/

    
    //if(frame_counter%2 == 0) {
    for(int i = 0; i < height; i+=1) {
        for(int j = 0; j < width; j+=1) {
            /*x = (height/2.0) - ((float)i/height);
            y = (width/2.0) - ((float)j/width);
            set_xy(i, j, 16*sqrt((x*x) + (y*y)));*/
            //x = 0.5f - ((float)i/height);
            //y = 0.5f - ((float)j/width);
            //x = ((float)i/height);
            //y = ((float)j/width);
            /*if ((j+i < height) && (j-i > 0)) {
                _lines[i][j] = _lines[j+i][j-i];
            }*/
            
            //_lines[i][j] = prev_lines[(i+j)%height][abs(j-i)];
            /*if ((i > height/2) && (i>j)) {
                _lines[i][j] = 0;
            }*/
            //set_xy(i, j, (i+j)/30.0f);
            //set_xy(i, j, color_scale(32,32,(x+y)/2));
        }
    }
    //memcpy(prev_lines, _lines, lines_size);

    //memcpy(_lines, prev_lines, lines_size);
    //}
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

