#ifndef _UCC_STATUS_STRINGS
#define _UCC_STATUS_STRINGS

#include <Arduino.h>
#include "stdint.h"
#include "ucc5870_regs.h"

#define SERIAL_NOT_ACTIVE 1

#ifdef DEBUG_MODE
static inline uint16_t print_status1_reg(uint16_t regVal) { return 0; };
static inline uint16_t print_status2_reg(uint16_t regVal) { return 0; };
static inline uint16_t print_status3_reg(uint16_t regVal) { return 0; };
static inline uint16_t print_status4_reg(uint16_t regVal) { return 0; };
static inline uint16_t print_status5_reg(uint16_t regVal) { return 0; };

#else //in RELEASE mode
uint16_t print_status1_reg(uint16_t regVal);

uint16_t print_status2_reg(uint16_t regVal);

uint16_t print_status3_reg(uint16_t regVal);

uint16_t print_status4_reg(uint16_t regVal);

uint16_t print_status5_reg(uint16_t regVal);
#endif

#endif