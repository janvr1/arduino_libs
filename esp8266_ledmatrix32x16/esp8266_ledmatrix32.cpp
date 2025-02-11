#include "esp8266_ledmatrix32.h"
#include "characters.h"


// ledMatrix::ledMatrix() {
// }

void ledMatrix::init(int Ton, int Toff, int outputEnable, int latch, 
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
    digitalWrite(LAT, HIGH);
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

void ledMatrix::_transferLine(int i) {
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    SPI.write32(_frame[GREEN][i]);
    SPI.write32(_frame[RED][i]);
    SPI.endTransaction();
}

void ledMatrix::clearFrame() {
    for (int i=0; i<2; i++) {
        for (int j=0; j<16; j++) {
            _frame[i][j] = (uint32_t) 0xFFFFFFFF;
            }
        }
    }

void ledMatrix::showFrame() {
    _Ton = _calculateTon();
    _Toff = 500-_Ton;

    if (_Ton > 450) {
        _outputDisable();
        for (int i = 0; i<16; i++) {
            _transferLine(i);
            _outputDisable();
            _selectLine(i);
            _latchOutput();
            _outputEnable();
            delayMicroseconds(_Ton);
            // delayMicroseconds(_Toff);
        }
    } else {
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
}

void ledMatrix::setPixel(int val, int x, int y, int c) {
    uint32_t row = (uint32_t) val;
    if (row) {
        _frame[c][y] &= ~(1 << (31-x));
    }
    if (!row) {
        _frame[c][y] |= (1 << (31-x));
    }
}

int ledMatrix::_calculateTon() {
    static unsigned int i = 0;
    static int _illum0 = 0;
    static int _illum1 = 0;
    static int _illum2 = 0;
    static int _illum3 = 0;
    static int _illum4 = 0;


    int illum;

    if (i%20 == 0) {
        illum = analogRead(A0);
        // Serial.print("AnaloagRead");
        // Serial.println(illum);
        _illum4 = _illum3;
        _illum3 = _illum2;
        _illum2 = _illum1;
        _illum1 = _illum0;
        _illum0 = illum;
    }

    int illum_avg = (_illum0 + _illum1 + _illum2 + _illum3 + _illum4)/5;
    i++;
    
    // Serial.println(illum_avg);
    if (illum_avg > 900) {
        return 500;
    }
    // if (illum_avg < 5) {
    //     return 0;
    // }
    if (illum_avg < 11) {
        return 1;
    }


    // return (int) (illum-33)*1.25;
    return (int) round(illum_avg/1.8-5);
}


void ledMatrix::drawGUI1(int HH, int mm, int ss, int DD, int MM, int YYYY) {
    clearFrame();
    int YY = YYYY-2000;
    //TIME
    drawChar(2,  2, nums[HH/10], RED);
    drawChar(6,  2, nums[HH%10], RED);
    drawChar(9,  2, colon, RED);
    drawChar(12, 2, nums[mm/10], RED);
    drawChar(16, 2, nums[mm%10], RED);
    drawChar(19, 2, colon, RED);
    drawChar(22, 2, nums[ss/10], RED);
    drawChar(26, 2, nums[ss%10], RED);
    //DATE
    drawChar(2,  9, nums[DD/10], RED);
    drawChar(6,  9, nums[DD%10], RED);
    drawChar(9,  9, dot, RED);
    drawChar(12, 9, nums[MM/10], RED);
    drawChar(16, 9, nums[MM%10], RED);
    drawChar(19, 9, dot, RED);
    drawChar(22, 9, nums[YY/10], RED);
    drawChar(26, 9, nums[YY%10], RED);
}

void ledMatrix::drawGUI2(int co2, int hum, int temp) {
    if (hum>99) {
        hum=99;
    }
    clearFrame();
    //TEMPERATURE
    drawChar(2,  2, nums[temp/10], RED);
    drawChar(6,  2, nums[temp%10], RED);
    drawChar(10,  2, celsius, RED, 5);
    //HUMIDITY
    drawChar(18, 2, nums[hum/10], RED);
    drawChar(22, 2, nums[hum%10], RED);
    drawChar(26, 2, procent, RED, 4);
    //CO2
    int clr = GREEN;
    if (co2 > 1500) {
        clr=RED;
        }
    if(co2>1000) {
        drawChar(0, 9, nums[co2/1000], clr);
        }
    drawChar(4, 9, nums[(co2/100)%10], clr);
    drawChar(8, 9, nums[(co2/10)%10], clr);
    drawChar(12, 9, nums[co2%10], clr);
    drawChar(17, 9, p, clr);
    drawChar(21, 9, p, clr);
    drawChar(25, 9, m, clr, 5);
}

void ledMatrix::drawChar(int x, int y, byte character[5], int color, int width) {
    int startx = x;
    int starty = y;
    for (int iy = 0; iy<5; iy++) {
        byte line = character[iy];
        for (int ix=width-1; ix>-1; ix--) {
            byte px = ((~line>>ix)&1);
            setPixel(px, startx+width-ix-1, iy+starty, color);
        }
    }

}
