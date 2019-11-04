#ifndef jan_bme280_h
#define jan_bme280_h

#include <Arduino.h>
#include <Wire.h>

#define BME280_ADDR_CTRL_MEAS 0xF4
#define BME280_ADDR_CTRL_HUM 0xF2
#define BME280_ADDR_CONFIG 0xF5
#define BME280_ADDR_STATUS 0xF3
#define BME280_ADDR_RESET 0xE0
#define BME280_ADDR_ID 0xD0


typedef struct {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t dig_H6;
} bme280_calib_data;

class bme280 {
    public:
    bme280(uint8_t address);
    enum mode{
        MODE_SLEEP = 0b00,
        MODE_NORMAL = 0b11,
        MODE_FORCED = 0b01};
    enum oversampling{
        SAMPLING_NONE = 0b000,
        SAMPLING_X1   = 0b001,
        SAMPLING_X2   = 0b010,
        SAMPLING_X4   = 0b011,
        SAMPLING_X8   = 0b100,
        SAMPLING_X16  = 0b101};
    enum iir_filter{
        FILTER_OFF = 0b000,
        FILTER_X2  = 0b001,
        FILTER_X4  = 0b010,
        FILTER_X8  = 0b011,
        FILTER_X16 = 0b100};
    enum time_standby{
        STANDBY_MS_0_5 = 0b000,
        STANDBY_MS_10 = 0b110,
        STANDBY_MS_20 = 0b111,
        STANDBY_MS_62_5 = 0b001,
        STANDBY_MS_125 = 0b010,
        STANDBY_MS_250 = 0b011,
        STANDBY_MS_500 = 0b100,
        STANDBY_MS_1000 = 0b101};
    void begin(int sda=-1, int scl=-1);
    void setMode(mode m);
    void setTemperatureOverSampling(oversampling osrs);
    void setHumidityOverSampling(oversampling osrs);
    void setPressureOverSampling(oversampling osrs);
    void setIIRFilter(iir_filter filter);
    void setStandbyTime(time_standby t_sb);
    void measurement();
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;

    private:
    struct config {
        uint8_t t_sb;
        uint8_t filter;
        uint8_t get() {
            return ((t_sb<<5)|(filter<<2));
            }
        void set(uint8_t reg) {
            t_sb = (reg&0b11100000)>>5;
            filter = (reg&0b00011100)>>2;
        }
    };
    config reg_config;

    struct ctrl_meas {
        uint8_t osrs_t;
        uint8_t osrs_p;
        uint8_t mode;
        uint8_t get() {
            return ((osrs_t<<5)|(osrs_p<<2)|mode);
            }
        void set(uint8_t reg) {
            osrs_t = (reg&0b11100000)>>5;
            osrs_p = (reg&0b00011100)>>2;
            mode   = (reg&0b00000011);
        }
    };
    ctrl_meas reg_ctrl_meas;

    struct ctrl_hum {
        uint8_t osrs_h;
        uint8_t get() {return osrs_h;};
        void set(uint8_t reg) {osrs_h = reg;};
    };
    ctrl_hum reg_ctrl_hum;

    struct temp_struct {
        float T;
        int32_t t_fine;
    };

    bme280_calib_data _loadCalibrationData();
    bme280_calib_data _cal;
    uint8_t _i2c_addr;
    uint16_t _readCalDig(uint8_t addr);
    void _readMeasurementBlock();
    float _pressureCompensation(int32_t adc_P, int32_t t_fine);
    temp_struct _temperatureCompensation(int32_t adc_T);
    float _humidityCompensation(int32_t adc_H, int32_t t_fine);
    uint8_t _readRegister(uint8_t addr);
    void _setRegister(uint8_t addr, uint8_t data);
};

#endif