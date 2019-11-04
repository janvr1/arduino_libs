#ifndef mhz19b_h
#define mhz19b_h

#include "Arduino.h"
#include <HardwareSerial.h>

class MHZ19B {
    public:
        #ifdef ESP32
        void begin(HardwareSerial *serial, int rx=-1, int tx=-1);
        #endif
        #ifdef ESP8266
        void begin(HardwareSerial *serial);
        #endif
        int getCO2(void);
        void setRange2000(void);
        void setRange5000(void);
        void baselineOn(void);
        void baselineOff(void);
        void calibrateZero(void);
        void calibrateSpan(int span);
        void requestCO2(void);
        int readCO2(void);
        int CO2;
        int T;
    private:
        byte _getCheckSum(byte *msg);
        void _flushSerial(void);
        byte _CMD_REQUESTCO2[9]   = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
        byte _CMD_BASELINEOFF[9]  = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86};
        byte _CMD_BASELINEON[9]   = {0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6};
        byte _CMD_RANGE2000[9]    = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x07, 0xD0, 0x8F};
        byte _CMD_RANGE5000[9]    = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x13, 0x88, 0xCB};
        byte _CMD_ZEROPOINTCAL[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
        byte _CMD_SPANPOINTCAL[9] = {0xFF, 0x01, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        int _MSG_LEN = 9;
        HardwareSerial *_ser;
};

#endif