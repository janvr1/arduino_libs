#include <ledmatrix64.h>
#include "characters.h"
#include "font_ibm.h"

// ledMatrix::ledMatrix() {
// }

void ledMatrix::begin(int Ton, int Toff, int outputEnable, int latch, 
                     int line_A, int line_B, int line_C, int line_D) {
    SPI.begin();
    OE = outputEnable;
    LAT = latch;
    A = line_A;
    B = line_B;
    C = line_C;
    D = line_D;
    pinMode(A0, INPUT);
    pinMode(OE, OUTPUT);
    pinMode(LAT, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);

    _Ton = Ton;
    _Toff = Toff;

    clearFrame();
}

void ledMatrix::_latchOutput() {
    digitalWrite(LAT, LOW);
    digitalWrite(LAT, HIGH);
    delayMicroseconds(5);
    digitalWrite(LAT, LOW);
}

void ledMatrix::_outputEnable() {
    digitalWrite(OE, LOW);
}

void ledMatrix::_outputDisable() {
    digitalWrite(OE, HIGH);
}

void ledMatrix::_selectLine(int i) {
    digitalWrite(A, i&1);
    digitalWrite(B, (i>>1)&1);
    digitalWrite(C, (i>>2)&1);
    digitalWrite(D, (i>>3)&1);
}

void ledMatrix::_transferLine(int j) {
    SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE0));

    //BLUE
    SPI.write32(_frame[BLUE][j+16]>>32); //SPODNJA
    SPI.write32(_frame[BLUE][j+16]);

    SPI.write32(_frame[BLUE][j]>>32); //ZGORNJA
    SPI.write32(_frame[BLUE][j]);
    //GREEN
    SPI.write32(_frame[GREEN][j+16]>>32); //SPODNJA
    SPI.write32(_frame[GREEN][j+16]);

    SPI.write32(_frame[GREEN][j]>>32); //ZGORNJA
    SPI.write32(_frame[GREEN][j]);

    //RED
    SPI.write32(_frame[RED][j+16]>>32); //SPODNJA
    SPI.write32(_frame[RED][j+16]);

    SPI.write32(_frame[RED][j]>>32); //ZGORNJA
    SPI.write32(_frame[RED][j]);

    SPI.endTransaction();
}

void ledMatrix::clearFrame() {
    for (int i=0; i<3; i++) {
        for (int j=0; j<32; j++) {
            _frame[i][j] = 0;
            }
        }
    }

void ledMatrix::showFrame() {
    // _Ton = _calculateTon();
    // _Toff = 512-_Ton;
    for (int i = 0; i<16; i++) {
        _selectLine(i);
        _transferLine(i);
        _latchOutput();
        _outputEnable();
        delayMicroseconds(_Ton);
        _outputDisable();
        delayMicroseconds(_Toff);
    }
}

void ledMatrix::setPixel(int val, int x, int y, int c) {
    uint64_t row = (uint64_t) 1;

    if (!val) {
        _frame[c][y] &= ~(row << (63-x));
    }
    if (val) {
        _frame[c][y] |= (row << (63-x));
    }
}

int ledMatrix::_calculateTon() {
    int illum = analogRead(A0);
    if (illum<50) {return illum/3;}
    if (illum<25) {return 4;}
    if (illum<15) {return 2;}
    if (illum<10) {return 1;}
    return illum/2;
}


void ledMatrix::drawText(int x, int y, String text, int rgb) {
    int charheight = 8;
    int charwidth = 8;
    int len = text.length();

    for (int i = 0; i<len; i++) {
        int idx = text.charAt(i);
        if (idx==96) {idx=144;}
        for (int j=0; j<charheight; j++) {
            uint8_t line = font[idx][j];
            for (int k=charwidth; k>-1; k--) {
                int px = (line>>k)&1;
                int curr_x = x+charwidth-k+i*charwidth;
                int curr_y = y+j;
                setPixel(px, curr_x, curr_y, rgb);
            }
        }
    }
}


void ledMatrix::drawGUI1(int HH, int mm, int ss, int DD, int MM, int YYYY, int CO2, int HUM, int T) {
    clearFrame();
    if (HUM>99) {
        HUM = 99;
    }
    int co2_color;
    int YY = YYYY-2000;

    char time_str[9];
    char date_str[8];
    sprintf(time_str, "%02d:%02d:%02d", HH, mm, ss);
    sprintf(date_str, "%02d.%02d.%02d", DD, MM, YY);

    drawText(0, 0, String(time_str), RED);
    drawText(0, 8, String(date_str), RED);

    // drawText(0, 0, String(HH)+":"+String(mm)+":"+String(ss), RED);
    // drawText(0, 8, String(DD)+"."+String(MM)+"."+String(YY), RED);
    drawText(0, 16, String(T)+"`C " + String(HUM) + "%", BLUE);

    if (CO2>1500) {
        co2_color=RED;
        }
    else {
        co2_color = GREEN;
    }
    drawText(8, 24, String(CO2)+"ppm", co2_color);

}
