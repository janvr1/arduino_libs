#ifndef ledmatrix_h
#define ledmatrix_h

#include "Arduino.h"
#include "SPI.h"

class ledMatrix{
    public:
        void begin(int Ton=100, int Toff=0, int outputEnable=D4, int latch=D0,
                  int line_A=D1, int line_B=D2, int line_C=D8, int line_D=D6);
        void setPixel(int val, int x, int y, int c);
        void clearFrame(void);
        void drawGUI1(int HH, int mm, int ss, int DD, int MM, int YY, int CO2, int HUM, int T);
        void showFrame(void);
        void drawText(int x, int y, String text, int rgb);
        int RED = 0;
        int GREEN = 1;
        int BLUE = 2;
        int OE;
        int LAT;
        int A;
        int B;
        int C;
        int D;
    private:
        uint64_t _frame[3][32];
        void _transferLine(int i);
        void _selectLine(int i);
        void _latchOutput(void);
        void _outputEnable(void);
        void _outputDisable(void);
        int _calculateTon(void);
        int _Ton;
        int _Toff;
};

#endif