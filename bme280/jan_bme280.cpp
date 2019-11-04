#include "jan_bme280.h"

bme280::bme280(uint8_t address) {
    _i2c_addr = address;
}

void bme280::begin(int sda, int scl) {
    if (sda>-1 && scl>-1) {
        Wire.begin(sda, scl);
    } else {
        Wire.begin();
    }
    _cal = _loadCalibrationData();
    reg_ctrl_meas.set(_readRegister(BME280_ADDR_CTRL_MEAS));
    reg_ctrl_hum.set(_readRegister(BME280_ADDR_CTRL_HUM));
    reg_config.set(_readRegister(BME280_ADDR_CONFIG));
}

void bme280::setMode(mode m) {
    reg_ctrl_meas.set(_readRegister(BME280_ADDR_CTRL_MEAS));
    reg_ctrl_meas.mode = m;
    _setRegister(BME280_ADDR_CTRL_MEAS, reg_ctrl_meas.get());
}

void bme280::setIIRFilter(iir_filter filter) {
    reg_config.set(_readRegister(BME280_ADDR_CONFIG));
    reg_config.filter = filter;
    _setRegister(BME280_ADDR_CONFIG, reg_config.get());
}

void bme280::setTemperatureOverSampling(oversampling osrs) {
    reg_ctrl_meas.set(_readRegister(BME280_ADDR_CTRL_MEAS));
    reg_ctrl_meas.osrs_t = osrs;
    _setRegister(BME280_ADDR_CTRL_MEAS, reg_ctrl_meas.get());
}

void bme280::setPressureOverSampling(oversampling osrs) {
    reg_ctrl_meas.set(_readRegister(BME280_ADDR_CTRL_MEAS));
    reg_ctrl_meas.osrs_p = osrs;
    _setRegister(BME280_ADDR_CTRL_MEAS, reg_ctrl_meas.get());
}

void bme280::setHumidityOverSampling(oversampling osrs) {
    reg_ctrl_hum.set(_readRegister(BME280_ADDR_CTRL_HUM));
    reg_ctrl_hum.osrs_h = osrs;
    _setRegister(BME280_ADDR_CTRL_HUM, reg_ctrl_hum.get());
}

void bme280::setStandbyTime(time_standby t_sb) {
    reg_config.set(_readRegister(BME280_ADDR_CONFIG));
    reg_config.t_sb = t_sb;
    _setRegister(BME280_ADDR_CONFIG, reg_config.get());
}

void bme280::measurement() {
    if (reg_ctrl_meas.mode == MODE_NORMAL) {
        _readMeasurementBlock();
    } else {
        setMode(MODE_FORCED);
        while((_readRegister(BME280_ADDR_STATUS)&0x08) == 1) {
            delay(1);
        }
        _readMeasurementBlock();
    }
}

uint8_t bme280::_readRegister(uint8_t addr){
    Wire.beginTransmission(_i2c_addr);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(_i2c_addr, (byte) 1);
    uint8_t data = Wire.read();
    return data;
}

void bme280::_setRegister(uint8_t addr, uint8_t data) {
    Wire.beginTransmission(_i2c_addr);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint16_t bme280::_readCalDig(uint8_t addr) {
    Wire.beginTransmission(_i2c_addr);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(_i2c_addr, (byte) 2);
    uint16_t LSB = Wire.read();
    uint16_t MSB = Wire.read();
    uint16_t data = (MSB<<8)|LSB;
    return data;
}
bme280_calib_data bme280::_loadCalibrationData() {
    bme280_calib_data cal_data;
    cal_data.dig_T1 = (uint16_t) _readCalDig(0x88);
    cal_data.dig_T2 = (int16_t) _readCalDig(0x8A);
    cal_data.dig_T3 = (int16_t) _readCalDig(0x8C);
    cal_data.dig_P1 = (uint16_t) _readCalDig(0x8E);
    cal_data.dig_P2 = (int16_t) _readCalDig(0x90);
    cal_data.dig_P3 = (int16_t) _readCalDig(0x92);
    cal_data.dig_P4 = (int16_t) _readCalDig(0x94);
    cal_data.dig_P5 = (int16_t) _readCalDig(0x96);
    cal_data.dig_P6 = (int16_t) _readCalDig(0x98);
    cal_data.dig_P7 = (int16_t) _readCalDig(0x9A);
    cal_data.dig_P8 = (int16_t) _readCalDig(0x9C);
    cal_data.dig_P9 = (int16_t) _readCalDig(0x9E);
    cal_data.dig_H1 = (uint8_t) _readRegister(0xA1);
    cal_data.dig_H2 = (int16_t) _readCalDig(0xE1);
    cal_data.dig_H3 = (uint8_t) _readRegister(0xE3);
    uint16_t dig_H4_11_4 = (uint16_t) _readRegister(0xE4)<<4;
    uint16_t dig_H4_3_0 = (uint16_t) _readRegister(0xE5)&0b00001111;
    cal_data.dig_H4 = (int16_t) dig_H4_11_4 | dig_H4_3_0;
    uint16_t dig_H5_3_0 = (uint16_t) _readRegister(0xE5)&0b11110000>>4;
    uint16_t dig_H5_11_4 = (uint16_t) _readRegister(0xE6)<<4;
    cal_data.dig_H5 = (int16_t) dig_H5_11_4 | dig_H5_3_0;
    cal_data.dig_H6 = (int8_t) _readRegister(0xE7);
    return cal_data;
}

void bme280::_readMeasurementBlock() {
    uint32_t temp = 0, press = 0;
    uint16_t hum = 0;

    Wire.beginTransmission(_i2c_addr);
    Wire.write(0xF7);
    if (Wire.endTransmission(false)!=0) {
        temperature = 0;
        pressure = 0;
        humidity = 0;
        return;
    };
    Wire.requestFrom(_i2c_addr, (byte) 8);
    if (Wire.available() < 8) {
        temperature = 0;
        pressure = 0;
        humidity = 0;
        return;
    }
    press = Wire.read();
    press <<= 8;
    press |= Wire.read();
    press <<= 4;
    press |= Wire.read()&0b11110000;

    temp = Wire.read();
    temp <<= 8;
    temp |= Wire.read();
    temp <<= 4;
    temp |= Wire.read()&0b11110000;

    hum = Wire.read();
    hum <<= 8;
    hum |= Wire.read();

    struct temp_struct temp_s = _temperatureCompensation((int32_t) temp);
    temperature = temp_s.T;
    int32_t t_fine = temp_s.t_fine;
    pressure = _pressureCompensation(press, t_fine);
    humidity = _humidityCompensation(hum, t_fine);
}

bme280::temp_struct bme280::_temperatureCompensation(int32_t adc_T) {
    int32_t var1, var2, T, t_fine;
    var1 = ((((adc_T>>3) - ((int32_t)_cal.dig_T1<<1))) * ((int32_t)_cal.dig_T2))>>11;
    var2 = (((((adc_T>>4) - ((int32_t)_cal.dig_T1)) * ((adc_T>>4) - ((int32_t)_cal.dig_T1))) >> 12) * ((int32_t)_cal.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    struct temp_struct t;
    t.T = T/100.0;
    t.t_fine = t_fine;
    return t;
}

float bme280::_pressureCompensation(int32_t adc_P, int32_t t_fine) {
    int64_t var1, var2, p;
    var1 = ((int64_t) t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_cal.dig_P6;
    var2 = var2 + ((var1*(int64_t)_cal.dig_P5)<<17);
    var2 = var2 + (((int64_t)_cal.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_cal.dig_P3)>>8) + ((var1* (int64_t)_cal.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1)) * ((int64_t)_cal.dig_P1)>>33;
    if (var1 == 0) {
        return 0;
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)_cal.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_cal.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)_cal.dig_P7)<<4);
    return ((uint32_t) p)/256.0;
}

float bme280::_humidityCompensation(int32_t adc_H, int32_t t_fine) {
    int32_t var;
    var = (t_fine - ((int32_t)76800));
    var = (((((adc_H << 14) - (((int32_t)_cal.dig_H4) << 20) - (((int32_t)_cal.dig_H5) * var)) +
    ((int32_t)16384)) >> 15) * (((((((var * ((int32_t)_cal.dig_H6)) >> 10) * (((var * ((int32_t)_cal.dig_H3)) >> 11) +
    ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)_cal.dig_H2) + 8192) >> 14));
    var = (var - (((((var >> 15) * (var >> 15)) >> 7) * ((int32_t)_cal.dig_H1)) >> 4));
    var = (var < 0 ? 0 : var);
    var = (var > 419430400 ? 419430400 : var);
    uint32_t hum = (uint32_t) (var>>12);
    return hum/1024.0;
}
