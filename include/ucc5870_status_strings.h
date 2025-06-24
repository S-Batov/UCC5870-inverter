#ifndef _UCC_STATUS_STRINGS
#define _UCC_STATUS_STRINGS

#include <Arduino.h>
#include "stdint.h"
#include "ucc5870_regs.h"

#ifdef DEBUG_MODE

static inline void print_status_header(uint16_t regVal) { return; };
static inline void print_status1_reg(uint16_t regVal) { return; };
static inline void print_status2_reg(uint16_t regVal) { return; };
static inline void print_status3_reg(uint16_t regVal) { return; };
static inline void print_status4_reg(uint16_t regVal) { return; };
static inline void print_status5_reg(uint16_t regVal) { return; };

#else //in RELEASE mode

void print_status_header(uint16_t regVal);
void print_status1_reg(uint16_t regVal);
void print_status2_reg(uint16_t regVal);
void print_status3_reg(uint16_t regVal);
void print_status4_reg(uint16_t regVal);
void print_status5_reg(uint16_t regVal);

#endif

#endif