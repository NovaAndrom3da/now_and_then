#include <argz.h>
//
// Created by gijsl on 3/15/2021.
//

#include <stdbool.h>
#include "cmsis_os.h"
#include "ltc_task.h"
#include "LTC6811.h"
#include "LTC681x.h"
#include "bms_hardware.h"

#define ENABLED 1
#define DISABLED 0
#define DATALOG_ENABLED 1
#define DATALOG_DISABLED 0

//ADC Command Configurations. See LTC681x.h for options.
const uint8_t ADC_OPT = ADC_OPT_DISABLED; //!< ADC Mode option bit
const uint8_t ADC_CONVERSION_MODE = MD_27KHZ_14KHZ; //!< ADC Mode
const uint8_t ADC_DCP = DCP_DISABLED; //!< Discharge Permitted
const uint8_t CELL_CH_TO_CONVERT = CELL_CH_ALL; //!< Channel Selection for ADC conversion
const uint8_t AUX_CH_TO_CONVERT = AUX_CH_ALL; //!< Channel Selection for ADC conversion
const uint8_t STAT_CH_TO_CONVERT = STAT_CH_ALL; //!< Channel Selection for ADC conversion
const uint8_t SEL_ALL_REG = REG_ALL; //!< Register Selection
const uint8_t SEL_REG_A = REG_1; //!< Register Selection
const uint8_t SEL_REG_B = REG_2; //!< Register Selection

const uint16_t MEASUREMENT_LOOP_TIME = 500; //!< Loop Time in milliseconds(ms)

//Under Voltage and Over Voltage Thresholds
const uint16_t OV_THRESHOLD = 41000; //!< Over voltage threshold ADC Code. LSB = 0.0001 ---(4.1V)
const uint16_t UV_THRESHOLD = 30000; //!< Under voltage threshold ADC Code. LSB = 0.0001 ---(3V)

/************************************
  END SETUP
*************************************/

/******************************************************
 Global Battery Variables received from 681x commands.
 These variables store the results from the LTC6811
 register reads and the array lengths must be based
 on the number of ICs on the stack
 ******************************************************/
cell_asic BMS_IC[1]; //!< Global Battery Variable

/*********************************************************
 Set the configuration bits.
 Refer to the Configuration Register Group from data sheet.
**********************************************************/
bool REFON = true; //!< Reference Powered Up Bit
bool ADCOPT = ADC_OPT; //!< ADC Mode option bit
bool GPIOBITS_A[5] = {false, false, false, false, false}; //!< GPIO Pin Control // Gpio 1,2,3,4,5
uint16_t UV = UV_THRESHOLD; //!< Under-voltage Comparison Voltage
uint16_t OV = OV_THRESHOLD; //!< Over-voltage Comparison Voltage
//!< Discharge cell switch //Dcc 1,2,3,4,5,6,7,8,9,10,11,12
bool DCCBITS_A[12] = {false, false, false, false, false, false, false, false, false, false, false, false};
bool DCTOBITS[4] = {false, false, false, false}; //!< Discharge time value // Dcto 0,1,2,3 // Programed for 4 min
/*Ensure that Dcto bits are set according to the required discharge time. Refer to the data sheet */

uint32_t error = 0;
uint32_t time = 0;
uint8_t flag = 0;
int8_t also_error = 0;

_Noreturn void start_ltc_test(void *argument) {
    LTC6811_init_cfg(1, BMS_IC);
    LTC6811_set_cfgr(0, BMS_IC, REFON, ADCOPT, GPIOBITS_A, DCCBITS_A, DCTOBITS, UV, OV);
    LTC6811_reset_crc_count(1, BMS_IC);
    LTC6811_init_reg_limits(1, BMS_IC);

    wakeup_sleep(1);
    LTC6811_wrcfg(1, BMS_IC);
    wakeup_idle(1);


    uint32_t val1;
    while (1) {
        wakeup_sleep(1);
        LTC6811_adcvsc(ADC_CONVERSION_MODE, ADC_DCP);
        time = LTC6811_pollAdc();
        if (time != 0)
        {
            flag++;
        }
        wakeup_idle(1);
        error = LTC6811_rdcv(SEL_ALL_REG, 1, BMS_IC);
        wakeup_idle(1);
        also_error = LTC6811_rdstat(SEL_ALL_REG, 1, BMS_IC);
        val1 = BMS_IC[0].stat.stat_codes[0];

        vTaskDelay(pdMS_TO_TICKS(500));
    }

}