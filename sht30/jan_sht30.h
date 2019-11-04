#ifndef jan_sht30_h
#define jan_sht30_h

#include "Arduino.h"
#include "Wire.h"

class sht30 {
    public:
    sht30(uint8_t addr, int sda=-1, int scl=-1);
    enum repeatability {REP_LOW = 0, REP_MEDIUM = 1, REP_HIGH = 2};
    enum clock_stretch {ENABLE = 0x2C, DISABLE = 0x24};
    enum periodic_frequency {FREQ_0HZ5 = 0x20, FREQ_1HZ = 0x21, FREQ_2HZ = 0x22, FREQ_4HZ = 0x23, FREQ_10HZ = 0x27};
    void singleMeasurement(repeatability rep = REP_HIGH, clock_stretch clk_str = ENABLE);
    void startPeriodicMeasure(periodic_frequency freq, repeatability rep);
    void fetchPeriodicMeasure();
    void stopPeriodicMeasure();
    float temperature;
    float humidity;

    private:
    uint8_t _address;
    uint8_t _calculateCRC(uint8_t data[]);
    float _calculateTemp(uint8_t temp_msb, uint8_t temp_lsb);
    float _calculateHumidity(uint8_t hum_msb, uint8_t hum_lsb);
    bool _checkCRC(uint8_t data_msb, uint8_t data_lsb, uint8_t crc);
};

#endif