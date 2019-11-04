#include <jan_mhz19b.h>

#ifdef ESP32
void MHZ19B::begin(HardwareSerial *serial, int rx, int tx) {
    _ser = serial;
    if (tx>0 && rx>0) {
    _ser->begin(9600, SERIAL_8N1, rx, tx);
    }
    else {
        _ser->begin(9600, SERIAL_8N1);
    }
}
#endif

#ifdef ESP8266
void MHZ19B::begin(HardwareSerial *serial) {
    _ser = serial;
    _ser->begin(9600);
}
#endif

int MHZ19B::getCO2() {
    if (_ser->available() > 0) {
        _flushSerial();
    }
    _ser->write(_CMD_REQUESTCO2, _MSG_LEN);
    delay(100);
    int n = _ser->available();
    if (n < 1) {
        return 0;
    }
    if (n > 9) {
        return 9999;
    }
    byte res[_MSG_LEN];
    _ser->readBytes(res, _MSG_LEN);
    if (res[8] != _getCheckSum(res)) {
        return 1;
    }
    CO2 = ((int) res[2])*256 + (int) res[3];
    T = (int) res[4] - 40;
    return CO2;
}

int MHZ19B::readCO2 () {
    int n = _ser->available();
    if (n < 1) {
        return 0;
    }
    if (n > 9) {
        return 9999;
    }
    byte response[_MSG_LEN];
    _ser->readBytes(response, _MSG_LEN);

    if (response[8] != _getCheckSum(response)) {
        return 0;
    }
    CO2 = ((int) response[2])*256 + (int) response[3];
    T = (int) response[4] - 40;
    return CO2;
}

void MHZ19B::requestCO2() {
    if (_ser->available() > 0) {
        _flushSerial();
    }
    _ser->write(_CMD_REQUESTCO2, _MSG_LEN);
}

byte MHZ19B::_getCheckSum(byte *msg) {
    byte i, chk = 0;
    for (i=1; i<8; i++) {
        chk += msg[i];
    }
    chk = ~chk+1;
    return chk;
}

void MHZ19B::setRange2000() {
    _ser->begin(9600);
    _ser->write(_CMD_RANGE2000, _MSG_LEN);
    _ser->end();
}

void MHZ19B::setRange5000() {
    _ser->begin(9600);
    _ser->write(_CMD_RANGE5000, _MSG_LEN);
    _ser->end();
}

void MHZ19B::baselineOff() {
    _ser->begin(9600);
    _ser->write(_CMD_BASELINEOFF, _MSG_LEN);
    _ser->end();
}

void MHZ19B::baselineOn() {
    _ser->begin(9600);
    _ser->write(_CMD_BASELINEON, _MSG_LEN);
    _ser->end();
}

void MHZ19B::calibrateZero() {
    _ser->begin(9600);
    _ser->write(_CMD_ZEROPOINTCAL, _MSG_LEN);
    _ser->end();
    }

void MHZ19B::calibrateSpan(int span) {
    byte HH = span/256;
    byte LL = span%256;
    _CMD_SPANPOINTCAL[3] = HH;
    _CMD_SPANPOINTCAL[4] = LL;
    byte checksum = _getCheckSum(_CMD_SPANPOINTCAL);
    _CMD_SPANPOINTCAL[8] = checksum;
    _ser->begin(9600);
    _ser->write(_CMD_SPANPOINTCAL, _MSG_LEN);
    _ser->end();
}

void MHZ19B::_flushSerial() {
    #ifdef ESP32
    _ser->flush();
    #endif
    #ifdef ESP8266
    while(_ser->available()) {
        _ser->read();
    }
    #endif
}