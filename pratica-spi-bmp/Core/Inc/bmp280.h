#ifndef BMP280_H
#define BMP280_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#ifndef TEST_HOST
#include "main.h"
#endif

/* BMP280 Register Map (Datasheet Table 18) */
#define BMP280_REG_TEMP_XLSB       0xFC
#define BMP280_REG_TEMP_LSB        0xFB
#define BMP280_REG_TEMP_MSB        0xFA
#define BMP280_REG_PRESS_XLSB      0xF9
#define BMP280_REG_PRESS_LSB       0xF8
#define BMP280_REG_PRESS_MSB       0xF7
#define BMP280_REG_CONFIG          0xF5
#define BMP280_REG_CTRL_MEAS       0xF4
#define BMP280_REG_STATUS          0xF3
#define BMP280_REG_RESET           0xE0
#define BMP280_REG_ID              0xD0
#define BMP280_REG_CALIB_START     0x88

/* Constants */
#define BMP280_CHIP_ID             0x58
#define BMP280_RESET_VALUE         0xB6
#define BMP280_STANDARD_SEA_LEVEL_HPA 1013.25f

/* Calibration parameters structure (Datasheet Table 17) */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_CalibData;

#ifndef TEST_HOST
/* Device handle for STM32 HAL SPI */
typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef      *cs_port;
    uint16_t           cs_pin;
    BMP280_CalibData   calib;
    int32_t            t_fine;
} BMP280_Handle;
#endif

/* Pure calculation and protocol functions */
uint8_t bmp280_get_read_addr(uint8_t reg);
uint8_t bmp280_get_write_addr(uint8_t reg);
void bmp280_parse_calib(BMP280_CalibData *calib, const uint8_t *raw_buf);
float bmp280_compensate_temperature(int32_t adc_T, const BMP280_CalibData *calib, int32_t *t_fine);
float bmp280_compensate_pressure(int32_t adc_P, const BMP280_CalibData *calib, int32_t t_fine);
float bmp280_calculate_altitude(float pressure_hpa, float sea_level_hpa);

#ifndef TEST_HOST
/* Hardware functions for STM32 */
bool bmp280_init(BMP280_Handle *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);
bool bmp280_read_measurements(BMP280_Handle *dev, float *temperature, float *pressure, float *altitude);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BMP280_H */
