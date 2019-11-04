#ifndef esp32_ledmatrix_h
#define esp32_ledatrix_h

#include <Arduino.h>
#include <SPI.h>

class ledMatrix{
    public:
        void init(int Ton=100, int Toff=0, int outputEnable=27, int latch=12,
                  int line_A=32, int line_B=33, int line_C=25, int line_D=26, int analog_in=34);
        void setPixel(int val, int x, int y, int c);
        void clearFrame(void);
        void drawGUI(int HH, int mm, int ss, int DD, int MM, int YY, int CO2, int HUM, int T);
        void showFrame(void);
        void drawText(int x, int y, String text, int rgb);
        boolean ready = true;
        int RED = 0;
        int GREEN = 1;
        int BLUE = 2;
        int OE;
        int LAT;
        int A;
        int B;
        int C;
        int D;
        int analog;
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
        SPIClass * _hspi = NULL;
};

#endif