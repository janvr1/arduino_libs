#include <esp32_ledmatrix64.h>
#include "characters.h"
#include "font_ibm.h"

// ledMatrix::ledMatrix() {
// }

void ledMatrix::init(int Ton, int Toff, int outputEnable, int latch, 
                     int line_A, int line_B, int line_C, int line_D, int analog_in) {
    _hspi = new SPIClass(HSPI);
    _hspi->begin();
    OE = outputEnable;
    LAT = latch;
    A = line_A;
    B = line_B;
    C = line_C;
    D = line_D;
    analog = analog_in;
    // pinMode(A0, INPUT);
    pinMode(OE, OUTPUT);
    pinMode(LAT, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);
    pinMode(analog, INPUT);

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
    _hspi->beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE0));

    //BLUE
    _hspi->write32(_frame[BLUE][j+16]>>32); //SPODNJA
    _hspi->write32(_frame[BLUE][j+16]);

    _hspi->write32(_frame[BLUE][j]>>32); //ZGORNJA
    _hspi->write32(_frame[BLUE][j]);
    //GREEN
    _hspi->write32(_frame[GREEN][j+16]>>32); //SPODNJA
    _hspi->write32(_frame[GREEN][j+16]);

    _hspi->write32(_frame[GREEN][j]>>32); //ZGORNJA
    _hspi->write32(_frame[GREEN][j]);

    //RED
    _hspi->write32(_frame[RED][j+16]>>32); //SPODNJA
    _hspi->write32(_frame[RED][j+16]);

    _hspi->write32(_frame[RED][j]>>32); //ZGORNJA
    _hspi->write32(_frame[RED][j]);

    _hspi->endTransaction();
}

void ledMatrix::clearFrame() {
    for (int i=0; i<3; i++) {
        for (int j=0; j<32; j++) {
            _frame[i][j] = 0;
            }
        }
    }

void ledMatrix::showFrame() {
    _Ton = _calculateTon();
    _Toff = _Ton<460 ? 460-_Ton : 0;
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
    int illum = analogRead(analog);
    if (illum > 3999) {
        return 500;
    }
    return (int) illum/8;
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


void ledMatrix::drawGUI(int HH, int mm, int ss, int DD, int MM, int YYYY, int CO2, int HUM, int T) {
    clearFrame();
    if (HUM>99) {
        HUM = 99;
    }
    int color1 = RED, color2 = RED, color3 = BLUE, color4 = GREEN;

    int YY = YYYY-2000;

    char time_str[10];
    char date_str[10];
    sprintf(time_str, "%02d:%02d:%02d", HH, mm, ss);
    sprintf(date_str, "%02d.%02d.%02d", DD, MM, YY);

    if (CO2>1500) {
        color4=RED;
    }
    int x_offset = CO2>1000 ? 4 : 8;

    if (_Ton < 10) {
        color3 = RED;
    }
    ready = false;
    drawText(0, 0, String(time_str), color1);
    drawText(0, 8, String(date_str), color2);
    drawText(0, 16, String(T)+"`C " + String(HUM) + "%", color3);
    drawText(x_offset, 24, String(CO2)+"ppm", color4);
    ready = true;
}
