#include "bmp280.h"
#include <string.h>
#include <math.h>

/* SPI address masking according to Datasheet Section 5.3:
 * In SPI mode, bit 7 of the address indicates Read (1) or Write (0).
 */
uint8_t bmp280_get_read_addr(uint8_t reg)
{
    return (uint8_t)((reg & 0x7F) | 0x80);
}

uint8_t bmp280_get_write_addr(uint8_t reg)
{
    return (uint8_t)(reg & 0x7F);
}

/* Parse 24 bytes of calibration trimming data from registers 0x88..0x9F */
void bmp280_parse_calib(BMP280_CalibData *calib, const uint8_t *raw_buf)
{
    calib->dig_T1 = (uint16_t)(raw_buf[0]  | (raw_buf[1]  << 8));
    calib->dig_T2 = (int16_t) (raw_buf[2]  | (raw_buf[3]  << 8));
    calib->dig_T3 = (int16_t) (raw_buf[4]  | (raw_buf[5]  << 8));

    calib->dig_P1 = (uint16_t)(raw_buf[6]  | (raw_buf[7]  << 8));
    calib->dig_P2 = (int16_t) (raw_buf[8]  | (raw_buf[9]  << 8));
    calib->dig_P3 = (int16_t) (raw_buf[10] | (raw_buf[11] << 8));
    calib->dig_P4 = (int16_t) (raw_buf[12] | (raw_buf[13] << 8));
    calib->dig_P5 = (int16_t) (raw_buf[14] | (raw_buf[15] << 8));
    calib->dig_P6 = (int16_t) (raw_buf[16] | (raw_buf[17] << 8));
    calib->dig_P7 = (int16_t) (raw_buf[18] | (raw_buf[19] << 8));
    calib->dig_P8 = (int16_t) (raw_buf[20] | (raw_buf[21] << 8));
    calib->dig_P9 = (int16_t) (raw_buf[22] | (raw_buf[23] << 8));
}

/* Compensate Temperature in °C according to Datasheet Section 3.11.3 */
float bmp280_compensate_temperature(int32_t adc_T, const BMP280_CalibData *calib, int32_t *t_fine)
{
    int32_t var1, var2;

    var1 = ((((adc_T >> 3) - ((int32_t)calib->dig_T1 << 1))) * ((int32_t)calib->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib->dig_T1)) * ((adc_T >> 4) - ((int32_t)calib->dig_T1))) >> 12) *
            ((int32_t)calib->dig_T3)) >> 14;

    *t_fine = var1 + var2;
    int32_t T = (*t_fine * 5 + 128) >> 8;
    return (float)T / 100.0f;
}

/* Compensate Pressure in hPa using 64-bit precision according to Datasheet Section 3.11.3 */
float bmp280_compensate_pressure(int32_t adc_P, const BMP280_CalibData *calib, int32_t t_fine)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib->dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib->dig_P5) << 17);
    var2 = var2 + (((int64_t)calib->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib->dig_P3) >> 8) + ((var1 * (int64_t)calib->dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib->dig_P1) >> 33;

    if (var1 == 0) {
        return 0.0f; // avoid division by zero
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib->dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib->dig_P7) << 4);

    // p is in Q24.8 format (Pa * 256).
    // Convert to hPa: (p / 256.0f) / 100.0f = p / 25600.0f
    return (float)p / 25600.0f;
}

/* Calculate Altitude in meters using the International Barometric Formula */
float bmp280_calculate_altitude(float pressure_hpa, float sea_level_hpa)
{
    if (pressure_hpa <= 0.0f || sea_level_hpa <= 0.0f) {
        return 0.0f;
    }
    // h = 44330 * (1 - (P / P0)^(1 / 5.255))
    return 44330.0f * (1.0f - powf(pressure_hpa / sea_level_hpa, 0.1902949f));
}

#ifndef TEST_HOST

/* Helper functions for SPI CS and Transfers */
static inline void bmp280_delay_us(void)
{
    for (volatile int i = 0; i < 50; i++) {
        __NOP();
    }
}

static inline void bmp280_flush_rx_fifo(BMP280_Handle *dev)
{
    while (__HAL_SPI_GET_FLAG(dev->hspi, SPI_FLAG_RXNE)) {
        (void)dev->hspi->Instance->DR;
    }
}

static inline void bmp280_cs_low(BMP280_Handle *dev)
{
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
}

static inline void bmp280_cs_high(BMP280_Handle *dev)
{
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
}

static bool bmp280_write_reg(BMP280_Handle *dev, uint8_t reg, uint8_t val)
{
    uint8_t tx_buf[2];
    tx_buf[0] = bmp280_get_write_addr(reg);
    tx_buf[1] = val;

    bmp280_flush_rx_fifo(dev);
    bmp280_cs_low(dev);
    bmp280_delay_us();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(dev->hspi, tx_buf, 2, HAL_MAX_DELAY);
    bmp280_delay_us();
    bmp280_cs_high(dev);
    bmp280_delay_us();

    return (status == HAL_OK);
}

static bool bmp280_read_regs(BMP280_Handle *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    if (len > 30) {
        return false;
    }

    uint8_t tx_buf[32] __attribute__((aligned(4))) = {0};
    uint8_t rx_buf[32] __attribute__((aligned(4))) = {0};

    tx_buf[0] = bmp280_get_read_addr(reg);

    bmp280_flush_rx_fifo(dev);
    bmp280_cs_low(dev);
    bmp280_delay_us();
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(dev->hspi, tx_buf, rx_buf, len + 1, HAL_MAX_DELAY);
    bmp280_delay_us();
    bmp280_cs_high(dev);
    bmp280_delay_us();

    if (status == HAL_OK) {
        memcpy(data, &rx_buf[1], len);
        return true;
    }

    return false;
}

/* Initialize BMP280 */
bool bmp280_init(BMP280_Handle *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    dev->hspi = hspi;
    dev->cs_port = cs_port;
    dev->cs_pin = cs_pin;
    dev->t_fine = 0;

    // Ensure CS is HIGH (idle)
    bmp280_cs_high(dev);
    HAL_Delay(10);

    // Read Chip ID from 0xD0 (expect 0x58)
    uint8_t chip_id = 0;
    if (!bmp280_read_regs(dev, BMP280_REG_ID, &chip_id, 1)) {
        return false;
    }

    if (chip_id != BMP280_CHIP_ID) {
        return false;
    }

    // Soft reset
    bmp280_write_reg(dev, BMP280_REG_RESET, BMP280_RESET_VALUE);
    HAL_Delay(50);

    // Read calibration parameters (24 bytes from 0x88 to 0x9F)
    uint8_t calib_buf[24] = {0};
    if (!bmp280_read_regs(dev, BMP280_REG_CALIB_START, calib_buf, 24)) {
        return false;
    }
    bmp280_parse_calib(&dev->calib, calib_buf);

    // Configure filter and standby:
    // config (0xF5):
    // t_sb = 001 (62.5 ms), filter = 010 (filter x4), spi3w_en = 0 (4-wire SPI)
    // 001 010 0 0 = 0x28
    bmp280_write_reg(dev, BMP280_REG_CONFIG, 0x28);

    // Configure oversampling and power mode:
    // ctrl_meas (0xF4):
    // osrs_t = 010 (x2), osrs_p = 101 (x16), mode = 11 (normal mode)
    // 010 101 11 = 0x57
    bmp280_write_reg(dev, BMP280_REG_CTRL_MEAS, 0x57);

    return true;
}

/* Read Temperature, Pressure, and Altitude */
bool bmp280_read_measurements(BMP280_Handle *dev, float *temperature, float *pressure, float *altitude)
{
    uint8_t raw_buf[6] = {0};

    // Burst read 6 bytes: press_msb (0xF7) to temp_xlsb (0xFC)
    if (!bmp280_read_regs(dev, BMP280_REG_PRESS_MSB, raw_buf, 6)) {
        return false;
    }

    int32_t adc_P = (int32_t)((((uint32_t)raw_buf[0]) << 12) |
                              (((uint32_t)raw_buf[1]) << 4)  |
                              (((uint32_t)raw_buf[2]) >> 4));

    int32_t adc_T = (int32_t)((((uint32_t)raw_buf[3]) << 12) |
                              (((uint32_t)raw_buf[4]) << 4)  |
                              (((uint32_t)raw_buf[5]) >> 4));

    *temperature = bmp280_compensate_temperature(adc_T, &dev->calib, &dev->t_fine);
    *pressure    = bmp280_compensate_pressure(adc_P, &dev->calib, dev->t_fine);
    *altitude    = bmp280_calculate_altitude(*pressure, BMP280_STANDARD_SEA_LEVEL_HPA);

    return true;
}

#endif
