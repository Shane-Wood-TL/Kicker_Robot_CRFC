#include "../include/ssd1306.h"


font::font(){
    //map the font to the ascii table
    font_map['0'] = zero;
    font_map['1'] = one;
    font_map['2'] = two;
    font_map['3'] = three;
    font_map['4'] = four;
    font_map['5'] = five;
    font_map['6'] = six;
    font_map['7'] = seven;
    font_map['8'] = eight;
    font_map['9'] = nine;
    font_map['#'] = hash;
    font_map['A'] = cap_A;
    font_map['B'] = cap_B;
    font_map['C'] = cap_C;
    font_map['D'] = cap_D;
    font_map['E'] = cap_E;
    font_map['F'] = cap_F;
    font_map['G'] = cap_G;
    font_map['H'] = cap_H;
    font_map['I'] = cap_I;
    font_map['J'] = cap_J;
    font_map['K'] = cap_K;
    font_map['L'] = cap_L;
    font_map['M'] = cap_M;
    font_map['N'] = cap_N;
    font_map['O'] = cap_O;
    font_map['P'] = cap_P;
    font_map['Q'] = cap_Q;
    font_map['R'] = cap_R;
    font_map['S'] = cap_S;
    font_map['T'] = cap_T;
    font_map['U'] = cap_U;
    font_map['V'] = cap_V;
    font_map['W'] = cap_W;
    font_map['X'] = cap_X;
    font_map['Y'] = cap_Y;
    font_map['Z'] = cap_Z;
    font_map['a'] = low_A;
    font_map['b'] = low_B;
    font_map['c'] = low_C;
    font_map['d'] = low_D;
    font_map['e'] = low_E;
    font_map['f'] = low_F;
    font_map['g'] = low_G;
    font_map['h'] = low_H;
    font_map['i'] = low_I;
    font_map['j'] = low_J;
    font_map['k'] = low_K;
    font_map['l'] = low_L;
    font_map['m'] = low_M;
    font_map['n'] = low_N;
    font_map['o'] = low_O;
    font_map['p'] = low_P;
    font_map['q'] = low_Q;
    font_map['r'] = low_R;
    font_map['s'] = low_S;
    font_map['t'] = low_T;
    font_map['u'] = low_U;
    font_map['v'] = low_V;
    font_map['w'] = low_W;
    font_map['x'] = low_X;
    font_map['y'] = low_Y;
    font_map['z'] = low_Z;
    font_map['-'] = Dash;
    font_map['.'] = Decimal;
    font_map['%'] = percent;
    font_map['|'] = verticalBar;
    font_map[' '] = space;
}

const uint16_t* font::get_letter(const uint8_t letter){
    if((letter < 20) || (letter > 124)){
        return font_map[' '];
    }else{
        return font_map[letter];
    }
    
}