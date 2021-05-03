//
// Created by gijsl on 3/28/2021.
//

#ifndef NOW_AND_THEN_LTC6811_H
#define NOW_AND_THEN_LTC6811_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This struct contains raw bits to/from the registers
 */
typedef struct {
    uint8_t CFGR[6];
    uint8_t CVAR[6];
    uint8_t CVBR[6];
    uint8_t CVCR[6];
    uint8_t CVDR[6];
    uint8_t AVAR[6];
    uint8_t AVBR[6];
    uint8_t STAR[6];
    uint8_t STBR[6];
    uint8_t COMM[6];
    uint8_t PWM[6];
} ltc68_reg_t;

void cmd_68(uint16_t cmd);

void write_68(uint16_t cmd, uint8_t *data, uint8_t data_len);

void clock_68(uint16_t cmd, uint8_t len);

bool read_68(uint8_t *data);

bool read_cell_volts(uint16_t* data);

bool read_temps(uint16_t* data);

void make_comm_reg_bytes(uint8_t mux_state, uint8_t led_state, uint8_t out[8]);

void set_mux_cmd(uint8_t mux_state);

#define BYTES_PER_REGISTER  6
#define COMMAND_WRCFGA              0b00000000001
#define COMMAND_WRCFGB              0b00000100100
#define COMMAND_RDCFGA              0b00000000010
#define COMMAND_RDCFGB              0b00000100110
#define COMMAND_RDCVA               0b00000000100
#define COMMAND_RDCVB               0b00000000110
#define COMMAND_RDCVC               0b00000001000
#define COMMAND_RDCVD               0b00000001010
#define COMMAND_RDAUXA              0b00000001100
#define COMMAND_RDAUXB              0b00000001110
#define COMMAND_RDSTATA             0b00000010000
#define COMMAND_RDSTATB             0b00000010010
#define COMMAND_ADCV_MD_DCP_CH      0b01001100000
#define COMMAND_ADOW_MD_PUP_DCP_CH  0b01000101000
#define COMMAND_CVST_MD_ST          0b01000000111
#define COMMAND_ADAX_MD_CHG         0b10001100000
#define COMMAND_ADAXD_MD_CHG        0b10000000000
#define COMMAND_AXST_MD_ST          0b10000000111
#define COMMAND_ADSTAT_MD_CHST      0b10001101000
#define COMMAND_ADSTATD_MD_CHST     0b10000001000
#define COMMAND_STATST_MD_ST        0b10000001111
#define COMMAND_ADCVAX_MD_DCP       0b10001101111
#define COMMAND_CLRCELL             0b11100010001
#define COMMAND_CLRAUX              0b11100010010
#define COMMAND_CLRSTAT             0b11100010011
#define COMMAND_PLADC               0b11100010100
#define COMMAND_DIAGN               0b11100010101
#define COMMAND_WRCOMM              0b11100100001
#define COMMAND_RDCOMM              0b11100100010
#define COMMAND_STCOMM              0b11100100011
#define COMMAND_WRSCTRL             0b00000010100
#define COMMAND_RDSCTRL             0b00000010110
#define MD_422HZ_1KHZ_MODE          (0b00 << 7)
#define MD_27HKHZ_FAST_14KHZ_MODE   (0b01 << 7)
#define MD_7KHZ_NORMAL_3KHZ_MODE    (0b10 << 7)
#define MD_26KHZ_FILT_2KHZ_MODE     (0b11 << 7)
#define DCP_DISCHARGE_NOT_PERMITTED (0b0 << 4)
#define DCP_DISCHARGE_PERMITTED     (0b1 << 4)
#define CH_ALL_CELLS                (0b000 << 0)
#define CH_CELL_1_7                 (0b001 << 0)
#define CH_CELL_2_8                 (0b010 << 0)
#define CH_CELL_3_9                 (0b011 << 0)
#define CH_CELL_4_10                (0b100 << 0)
#define CH_CELL_5_11                (0b101 << 0)
#define CH_CELL_6_12                (0b110 << 0)
#define PUP_PULL_DOWN_CURRENT       (0b0 << 6)
#define PUP_PULL_UP_CURRENT         (0b1 << 6)
#define ST_SELF_TEST_1              (0b01 << 5)
#define ST_SELF_TEST_2              (0b10 << 5)
#define CHG_GPIO_ALL                (0b000 << 0)
#define CHG_GPIO_1                  (0b001 << 0)
#define CHG_GPIO_2                  (0b010 << 0)
#define CHG_GPIO_3                  (0b011 << 0)
#define CHG_GPIO_4                  (0b100 << 0)
#define CHG_GPIO_5                  (0b101 << 0)
#define CHG_2ND_REF                 (0b110 << 0)
#define CHST_SOC_ITMP_VA_VD         (0b000 << 0)
#define CHST_SOC                    (0b001 << 0)
#define CHST_ITMP                   (0b010 << 0)
#define CHST_VA                     (0b011 << 0)
#define CHST_VD                     (0b100 << 0)
#define ICOM_WRITE_I2C_START        (0b0110)
#define ICOM_WRITE_I2C_STOP         (0b0001)
#define ICOM_WRITE_I2C_BLANK        (0b0000)
#define ICOM_WRITE_I2C_NO_TRANSMIT  (0b0111)
#define ICOM_READ_I2C_SDA_LOW       (0b0000)
#define ICOM_READ_I2C_SDA_HIGH      (0b0111)
#define FCOM_WRITE_I2C_MASTER_ACK   (0b0000)
#define FCOM_WRITE_I2C_MASTER_NACK   (0b1000)
#define FCOM_WRITE_I2C_MASTER_NACK_STOP (0b1001)
#define FCOM_READ_I2C_SLAVE_ACK     (0b0111)
#define FCOM_READ_I2C_SLAVE_NAK     (0b1111)

#ifdef __cplusplus
}
#endif

#endif //NOW_AND_THEN_LTC6811_H
