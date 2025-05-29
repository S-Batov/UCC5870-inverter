#ifndef _UCC_STATUS_STRINGS
#define _UCC_STATUS_STRINGS

#ifndef DEBUG_MODE

#include <Arduino.h>
#include "stdint.h"
#include "ucc5870_regs.h"
#include "ucc5870_status_strings.h"

#define SERIAL_NOT_ACTIVE 1

uint16_t print_status1_reg(uint16_t regVal)
{
    if(!Serial)
        return SERIAL_NOT_ACTIVE;
    
    UCC5870_STATUS1_REG_BITS* bits = (UCC5870_STATUS1_REG_BITS*)&regVal;

    Serial.println(F("--------------------------------------"));
    Serial.println("STATUS1 register");
    Serial.println(F("--------------------------------------"));

    Serial.print("GD_TWN_FAULT       : ");
    Serial.println(bits->GD_TWN_FAULT ? "FAULT" : "OK");
    
    Serial.print("PWM_COMP_CHK_FAULT : ");
    Serial.println(bits->PWM_COMP_CHK_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("OPM                : "));
    switch(bits->OPM) {
        case 1:
            Serial.print(F("CONFIGURATION1"));
            break;
        case 2:
            Serial.print(F("CONFIGURATION2"));
            break;
        case 3:
            Serial.print(F("ACTIVE"));
            break;
        default:
            Serial.print(F("ERROR"));
            break;
    }
    Serial.print(F(" (0x"));
    Serial.print(bits->OPM, HEX);
    Serial.println(F(")"));
    
    Serial.print("EN_STATE           : ");
    Serial.println(bits->EN_STATE ? "ASC ENABLED" : "ASC DISABLED");
    
    Serial.print("INN_STATE          : ");
    Serial.println(bits->INN_STATE ? "HIGH" : "LOW");
    
    Serial.print("INP_STATE          : ");
    Serial.println(bits->INP_STATE ? "HIGH" : "LOW");

    return 0;
}

uint16_t print_status2_reg(uint16_t reg_val)
{
    if(!Serial)
        return SERIAL_NOT_ACTIVE;
    
    UCC5870_STATUS2_REG_BITS* bits = (UCC5870_STATUS2_REG_BITS*)&reg_val;
    
    Serial.println(F("--------------------------------------"));
    Serial.println(F("TATUS2 Register 2:"));
    Serial.println(F("--------------------------------------"));
    
    Serial.print(F("OR_NFLT2_PRI   : "));
    Serial.println(bits->OR_NFLT2_PRI ? "ACTIVE" : "INACTIVE");
    
    Serial.print(F("OR_NFLT1_PRI   : "));
    Serial.println(bits->OR_NFLT1_PRI ? "ACTIVE" : "INACTIVE");
    
    Serial.print(F("DRV_EN_RCVD    : "));
    Serial.println(bits->DRV_EN_RCVD ? "RECEIVED" : "NOT RECEIVED");
    
    // CRC and fault conditions
    Serial.print(F("TRIM_CRC_FAULT : "));
    Serial.println(bits->TRIM_CRC_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("CFG_CRC_FAULT  : "));
    Serial.println(bits->CFG_CRC_FAULT ? "FAULT" : "OK");
    
    // Monitoring faults
    Serial.print(F("CLK_MON_FAULT  : "));
    Serial.println(bits->CLK_MON_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("BIST_FAULT     : "));
    Serial.println(bits->BIST_FAULT ? "FAIL" : "PASS");
    
    // Communication faults
    Serial.print(F("INT_COMM_FAULT : "));
    Serial.println(bits->INT_COMM_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("INT_REG_FAULT  : "));
    Serial.println(bits->INT_REG_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("SPI_FAULT      : "));
    Serial.println(bits->SPI_FAULT ? "FAULT" : "OK");
    
    // Protection faults
    Serial.print(F("STP_FAULT      : "));
    Serial.println(bits->STP_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("OVLO1_FAULT    : "));
    Serial.println(bits->OVLO1_FAULT ? "OVERVOLTAGE" : "OK");
    
    Serial.print(F("UVLO1_FAULT    : "));
    Serial.println(bits->UVLO1_FAULT ? "UNDERVOLTAGE" : "OK");
    
    // Ready status
    Serial.print(F("PRI_RDY        : "));
    Serial.println(bits->PRI_RDY ? "READY" : "NOT READY");

    return 0;
}

uint16_t print_status3_reg(uint16_t reg_val) 
{
    if(!Serial)
        return SERIAL_NOT_ACTIVE;
    
    UCC5870_STATUS3_REG_BITS* bits = (UCC5870_STATUS3_REG_BITS*)&reg_val;
    
    Serial.println(F("--------------------------------------"));
    Serial.print(F("STATUS3 Register:"));
    Serial.println(F("--------------------------------------"));
    
    // Protection faults
    Serial.print(F("DESAT_FAULT        : "));
    Serial.println(bits->DESAT_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("OC_FAULT           : "));
    Serial.println(bits->OC_FAULT ? "OVER CURRENT" : "OK");
    
    Serial.print(F("SC_FAULT           : "));
    Serial.println(bits->SC_FAULT ? "SHORT CIRCUIT" : "OK");
    
    Serial.print(F("VREG2_ILIMIT_FAULT : "));
    Serial.println(bits->VREG2_ILIMIT_FAULT ? "CURRENT LIMIT" : "OK");
    
    // Thermal faults
    Serial.print(F("PS_TSD_FAULT       : "));
    Serial.println(bits->PS_TSD_FAULT ? "OVERTEMP" : "OK");
    
    // Voltage monitoring
    Serial.print(F("VCEOV_FAULT        : "));
    Serial.println(bits->VCEOV_FAULT ? "OVERVOLTAGE" : "OK");
    
    Serial.print(F("UVLO2_FAULT        : "));
    Serial.println(bits->UVLO2_FAULT ? "UNDERVOLTAGE" : "OK");
    
    Serial.print(F("OVLO2_FAULT        : "));
    Serial.println(bits->OVLO2_FAULT ? "OVERVOLTAGE" : "OK");
    
    Serial.print(F("UVLO3_FAULT        : "));
    Serial.println(bits->UVLO3_FAULT ? "UNDERVOLTAGE" : "OK");
    
    Serial.print(F("OVLO3_FAULT        : "));
    Serial.println(bits->OVLO3_FAULT ? "OVERVOLTAGE" : "OK");
    
    // System states
    Serial.print(F("MCLP_STATE         : "));
    Serial.println(bits->MCLP_STATE ? "ACTIVE" : "INACTIVE");
    
    // Secondary protection
    Serial.print(F("INT_COMM_SEC_FAULT : "));
    Serial.println(bits->INT_COMM_SEC_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("INT_REG_SEC_FAULT  : "));
    Serial.println(bits->INT_REG_SEC_FAULT ? "FAULT" : "OK");
    
    // Gate monitor
    Serial.print(F("GM_FAULT           : "));
    Serial.println(bits->GM_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("GM_STATE           : "));
    Serial.println(bits->GM_STATE ? "ACTIVE" : "INACTIVE");

    return 0;
}

uint16_t print_status4_reg(uint16_t reg_val) 
{
    if(!Serial)
        return SERIAL_NOT_ACTIVE;
    
    UCC5870_STATUS4_REG_BITS* bits = (UCC5870_STATUS4_REG_BITS*)&reg_val;
    
    Serial.println(F("--------------------------------------"));
    Serial.println(F("STATUS4 Register:"));
    Serial.println(F("--------------------------------------"));
    
    // Ready status
    Serial.print(F("SEC_RDY           : "));
    Serial.println(bits->SEC_RDY ? "READY" : "NOT READY");
    
    // CRC and monitoring faults
    Serial.print(F("RIM_CRC_SEC_FAULT : "));
    Serial.println(bits->RIM_CRC_SEC_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("CFG_CRC_SEC_FAULT : "));
    Serial.println(bits->CFG_CRC_SEC_FAULT ? "FAULT" : "OK");
    
    Serial.print(F("CLK_MON_SEC_FAULT : "));
    Serial.println(bits->CLK_MON_SEC_FAULT ? "FAULT" : "OK");
    
    // Built-in self test
    Serial.print(F("BIST_SEC_FAULT    : "));
    Serial.println(bits->BIST_SEC_FAULT ? "FAIL" : "PASS");
    
    // Overload faults
    Serial.print(F("OR_NFLT2_SEC      : "));
    Serial.println(bits->OR_NFLT2_SEC ? "ACTIVE" : "INACTIVE");
    
    Serial.print(F("OR_NFLT1_SEC      : "));
    Serial.println(bits->OR_NFLT1_SEC ? "ACTIVE" : "INACTIVE");
    
    // Gate driver faults
    Serial.print(F("GD_TSD_SEC_FAULT  : "));
    Serial.println(bits->GD_TSD_SEC_FAULT ? "OVERTEMP" : "OK");
    
    Serial.print(F("GD_TWN_SEC_FAULT  : "));
    Serial.println(bits->GD_TWN_SEC_FAULT ? "FAULT" : "OK");
    
    // Voltage state
    Serial.print(F("VCE_STATE         : "));
    Serial.println(bits->VCE_STATE ? "HIGH" : "LOW");

    return 0;
}

uint16_t print_status5_reg(uint16_t reg_val)
{
    if(!Serial)
        return SERIAL_NOT_ACTIVE;

    UCC5870_STATUS5_REG_BITS* bits = (UCC5870_STATUS5_REG_BITS*)&reg_val;

    Serial.println(F("--------------------------------------"));
    Serial.println(F("STATUS5 Register:"));
    Serial.println(F("--------------------------------------"));
    
    Serial.print(F("ADC Status       : "));
    Serial.println(bits->ADC_FAULT ? "FAULT" : "OK");

    return 0;
}

#endif // NOT DEBUG_MODE

#endif