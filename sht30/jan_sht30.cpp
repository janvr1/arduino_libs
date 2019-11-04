#include <jan_sht30.h>

sht30::sht30 (uint8_t addr, int sda, int scl) {
    _address = addr;
    if (sda>-1 && scl>-1) {
        Wire.begin(sda, scl);
    }
    else {
        Wire.begin();
    }
}

void sht30::singleMeasurement(repeatability rep, clock_stretch  clk_str) {
    uint8_t MSB, LSB;
    MSB = clk_str;
    switch (clk_str) {
        case DISABLE:
            switch (rep) {
                case REP_LOW: LSB = 0x16; break;
                case REP_MEDIUM: LSB = 0x0B; break;
                case REP_HIGH: LSB = 0x00; break;
                default: return;
            };
            break;
        case ENABLE:
            switch (rep) {
                case REP_LOW: LSB = 0x10; break;
                case REP_MEDIUM: LSB = 0x0D; break;
                case REP_HIGH: LSB = 0x06; break;
                default: return;
        };
        break;
        default: return;
    }

    uint8_t cmd[2] = {MSB, LSB};
    Serial.print(MSB);
    Serial.println(LSB);
    Wire.beginTransmission(_address);
    Wire.write(cmd, 2);
    if (Wire.endTransmission()!=0) {
        temperature = 0;
        humidity = 0;
        return;
    }

    #ifdef ESP8266
    delay(30);
    #endif
    delay(50);

    uint8_t res[6];
    Wire.requestFrom(_address, (uint8_t) 6);
    Wire.readBytes(res, 6);
    if (Wire.available() != 0) {
        temperature = 0;
        humidity = 0;
        return;
    }

    uint8_t temp_msb = res[0], temp_lsb = res[1], temp_crc = res[2];
    uint8_t hum_msb = res[3], hum_lsb = res[4], hum_crc = res[5];

    if (_checkCRC(temp_msb, temp_lsb, temp_crc) && _checkCRC(hum_msb, hum_lsb, hum_crc)) {
        temperature = _calculateTemp(temp_msb, temp_lsb);
        humidity = _calculateHumidity(hum_msb, hum_lsb);
    }
    else {
        temperature = 0;
        humidity = 0;
    }
}

void sht30::startPeriodicMeasure(periodic_frequency freq, repeatability rep) {
    uint8_t MSB, LSB;
    MSB = freq;
    switch (freq) {
        case FREQ_0HZ5:
            switch (rep) {
                case REP_LOW: LSB = 0x32; break;
                case REP_MEDIUM: LSB = 0x24; break;
                case REP_HIGH: LSB = 0x2F; break;
            }
        case FREQ_1HZ:
            switch (rep) {
                case REP_LOW: LSB = 0x30; break;
                case REP_MEDIUM: LSB = 0x26; break;
                case REP_HIGH: LSB = 0x2D; break;
            }
        case FREQ_2HZ:
            switch (rep) {
                case REP_LOW: LSB = 0x36; break;
                case REP_MEDIUM: LSB = 0x20; break;
                case REP_HIGH: LSB = 0x2B; break;
            }
        case FREQ_4HZ:
            switch (rep) {
                case REP_LOW: LSB = 0x34; break;
                case REP_MEDIUM: LSB = 0x22; break;
                case REP_HIGH: LSB = 0x29; break;
            }
        case FREQ_10HZ:
            switch (rep) {
                case REP_LOW: LSB = 0x37; break;
                case REP_MEDIUM: LSB = 0x21; break;
                case REP_HIGH: LSB = 0x2A; break;
            }
        default: return;
    }

    uint8_t cmd[2] = {MSB, LSB};
    Wire.beginTransmission(_address);
    Wire.write(cmd, 2);
    Wire.endTransmission();
}

float sht30::_calculateTemp(uint8_t temp_msb, uint8_t temp_lsb) {
    return ((((uint16_t) temp_msb)<<8) | temp_lsb)*175/65535.0 - 45;
}

float sht30::_calculateHumidity(uint8_t hum_msb, uint8_t hum_lsb) {
    return ((((uint16_t) hum_msb)<<8) | hum_lsb)*100/65535.0;
}

uint8_t sht30::_calculateCRC(uint8_t data[2]) {
    uint8_t crc_val = 0xFF; // inital value
    uint8_t crc_poly = 0x31; // CRC polynomial x^8 + x^5 + x^4 + 1

    for (int j = 0; j<2; j++) {
        crc_val = crc_val^data[j];
        for (int i = 0; i<8; i++) {
            if (crc_val & 0x80) {
                crc_val = (crc_val<<1)^crc_poly;
            }
            else {
                crc_val = crc_val << 1;
            }
        }
    }
    return crc_val;
}

bool sht30::_checkCRC(uint8_t data_msb, uint8_t data_lsb, uint8_t crc) {
    uint8_t data[2] = {data_msb, data_lsb};
    return _calculateCRC(data)==crc;
}