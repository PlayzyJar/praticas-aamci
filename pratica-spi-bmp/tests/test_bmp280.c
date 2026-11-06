#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "bmp280.h"

int main(void)
{
    printf("Running BMP280 unit tests...\n");

    // Test 1: SPI Address bit manipulation according to datasheet section 5.3
    // Read: bit 7 must be set (reg | 0x80)
    // Write: bit 7 must be cleared (reg & 0x7F)
    assert(bmp280_get_read_addr(0x88) == 0x88); // 0x88 has bit 7 set
    assert(bmp280_get_read_addr(0x75) == 0xF5); // 0x75 | 0x80 = 0xF5
    assert(bmp280_get_read_addr(0xD0) == 0xD0);
    assert(bmp280_get_write_addr(0xF4) == 0x74); // 0xF4 & 0x7F = 0x74
    assert(bmp280_get_write_addr(0xF5) == 0x75); // 0xF5 & 0x7F = 0x75
    printf("Test 1 (SPI Address formatting) PASSED\n");

    // Test 2: Calibration parsing from 24-byte raw buffer
    // dig_T1 is uint16_t (little-endian), dig_T2 is int16_t, etc.
    uint8_t raw_calib[24] = {
        0x70, 0x6B, // dig_T1 = 27504
        0x5B, 0x67, // dig_T2 = 26459
        0x18, 0xFC, // dig_T3 = -1000
        0x64, 0x90, // dig_P1 = 36964
        0xD0, 0xCE, // dig_P2 = -12600
        0xD0, 0x0B, // dig_P3 = 3024
        0x27, 0x0B, // dig_P4 = 2855
        0x8C, 0x00, // dig_P5 = 140
        0xF9, 0xFF, // dig_P6 = -7
        0x8C, 0x3C, // dig_P7 = 15500
        0xF8, 0xC6, // dig_P8 = -14600
        0x70, 0x17  // dig_P9 = 6000
    };

    BMP280_CalibData calib;
    bmp280_parse_calib(&calib, raw_calib);

    assert(calib.dig_T1 == 27504);
    assert(calib.dig_T2 == 26459);
    assert(calib.dig_T3 == -1000);
    assert(calib.dig_P1 == 36964);
    assert(calib.dig_P2 == -12592);
    assert(calib.dig_P3 == 3024);
    assert(calib.dig_P4 == 2855);
    assert(calib.dig_P5 == 140);
    assert(calib.dig_P6 == -7);
    assert(calib.dig_P7 == 15500);
    assert(calib.dig_P8 == -14600);
    assert(calib.dig_P9 == 6000);
    printf("Test 2 (Calibration Parsing) PASSED\n");

    // Test 3: Temperature compensation formula
    int32_t t_fine = 0;
    int32_t raw_temp = 519888; // 20-bit raw ADC
    float temp = bmp280_compensate_temperature(raw_temp, &calib, &t_fine);
    // Should be a plausible room temperature (~20°C to 30°C) and t_fine populated
    assert(temp > 20.0f && temp < 30.0f);
    assert(t_fine != 0);
    printf("Test 3 (Temperature Compensation: %.2f C, t_fine: %d) PASSED\n", temp, t_fine);

    // Test 4: Pressure compensation formula
    int32_t raw_press = 415148; // 20-bit raw ADC
    float press_hpa = bmp280_compensate_pressure(raw_press, &calib, t_fine);
    // Should be atmospheric pressure ~900 to 1050 hPa
    assert(press_hpa > 900.0f && press_hpa < 1050.0f);
    printf("Test 4 (Pressure Compensation: %.2f hPa) PASSED\n", press_hpa);

    // Test 5: Altitude calculation
    // At standard sea level pressure 1013.25 hPa, altitude should be 0 m
    float alt_sea_level = bmp280_calculate_altitude(1013.25f, 1013.25f);
    assert(fabsf(alt_sea_level - 0.0f) < 0.1f);

    // Higher altitude = lower pressure (e.g. 900 hPa ~ 988 meters)
    float alt_900 = bmp280_calculate_altitude(900.0f, 1013.25f);
    assert(alt_900 > 950.0f && alt_900 < 1050.0f);
    printf("Test 5 (Altitude Calculation: at 1013.25 hPa -> %.2f m, at 900 hPa -> %.2f m) PASSED\n", 
           alt_sea_level, alt_900);

    printf("ALL TESTS PASSED!\n");
    return 0;
}
