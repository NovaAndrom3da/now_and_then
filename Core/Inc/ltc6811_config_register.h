//
// Created by gijsl on 4/13/2021.
//

#ifndef NOW_AND_THEN_LTC6811_CONFIG_REGISTER_H
#define NOW_AND_THEN_LTC6811_CONFIG_REGISTER_H


// config register values in datasheets
#define GPIO5_PULLUP 1
#define GPIO4_PULLUP 1
#define GPIO3_PULLUP 1
#define GPIO2_PULLUP 1
#define GPIO1_PULLUP 1
#define REFON 1
#define SWTRD 0
#define ADCOPT 0
#define VUV 1687 // 2.70 V
#define VOV 2656 // 4.25 V
#define DCC12 0
#define DCC11 0
#define DCC10 0
#define DCC9 0
#define DCC8 0
#define DCC7 0
#define DCC6 0
#define DCC5 0
#define DCC4 0
#define DCC4 0
#define DCC3 0
#define DCC2 0
#define DCC1 0
#define DCTO 0x0 // 30 seconds

// config register bytes
#define CFGR0 (GPIO5_PULLUP << 7) | (GPIO4_PULLUP << 6) | (GPIO3_PULLUP << 5) | (GPIO2_PULLUP << 4) | (GPIO1_PULLUP << 3) | (REFON << 2) | (SWTRD << 1) | (ADCOPT)
#define CFGR1 (VUV & 0x00FF)
#define CFGR2 ((VUV & 0x0F00) >> 8) | ((VOV & 0x000F) << 4)
#define CFGR3 (VOV & 0x0FF0) >> 4
#define CFGR4 (DCC8 << 7) | (DCC7 << 6) | (DCC6 << 5) | (DCC5 << 4) | (DCC4 << 3) | (DCC3 << 2) | (DCC2 << 1) | (DCC1)
#define CFGR5 ((DCTO | 0x0F) << 4) | (DCC12 << 3) | (DCC11 << 2) | (DCC10 << 1) | (DCC9)

#endif //NOW_AND_THEN_LTC6811_CONFIG_REGISTER_H
