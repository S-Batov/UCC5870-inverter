#define  _UCC5870
#include "ucc5870.h"
#include <SPI.h>
#include <Arduino.h>

// =============================================================================
// UCC5870 gate driver chip address definitions
#define  UH_ADDRESS   1U
#define  UL_ADDRESS   2U
#define  BROADCAST    15U

// =============================================================================
// UCC5870_Vars array indexing definitions
#define  UH  0
#define  UL  1

// =============================================================================
// UCC5870 status fault mask definitions
#define  STATUS1_FAULT_MASK      ((1 * GD_TWN_FAULT_MASK       )  | \
                                  (1 * PWM_COMP_CHK_FAULT_MASK )   )

#define  STATUS2_FAULT_MASK      ((0 * OR_NFLT2_MASK        )     | \
                                  (1 * OR_NFLT1_MASK        )     | \
                                  (0 * TRIM_CRC_FAULT_MASK  )     | \
                                  (0 * CFG_CRC_FAULT_MASK   )     | \
                                  (0 * CLK_MON_FAULT_MASK   )     | \
                                  (0 * BIST_FAULT_MASK      )     | \
                                  (0 * INT_COMM_FAULT_MASK  )     | \
                                  (0 * INT_REG_FAULT_MASK   )     | \
                                  (0 * SPI_FAULT_MASK       )     | \
                                  (0 * STP_FAULT_MASK       )     | \
                                  (0 * OVLO1_FAULT_MASK     )     | \
                                  (0 * UVLO1_FAULT_MASK     )      )

#define  STATUS3_FAULT_MASK      ((1 * DESAT_FAULT_MASK        )  | \
                                  (1 * OC_FAULT_MASK           )  | \
                                  (1 * SC_FAULT_MASK           )  | \
                                  (1 * VREG2_ILIMIT_FAULT_MASK )  | \
                                  (1 * PS_TSD_FAULT_MASK       )  | \
                                  (1 * VCEOV_FAULT_MASK        )  | \
                                  (1 * UVLO2_FAULT_MASK        )  | \
                                  (1 * OVLO2_FAULT_MASK        )  | \
                                  (1 * UVLO3_FAULT_MASK        )  | \
                                  (1 * OVLO3_FAULT_MASK        )  | \
                                  (0 * MCLP_STATE_MASK         )  | \
                                  (1 * INT_COMM_SEC_FAULT_MASK )  | \
                                  (1 * INT_REG_SEC_FAULT_MASK  )  | \
                                  (1 * GM_FAULT_MASK           )   )

#define  STATUS4_FAULT_MASK      ((0 * RIM_CRC_SEC_FAULT_MASK  )  | \
                                  (0 * CFG_CRC_SEC_FAULT_MASK  )  | \
                                  (0 * CLK_MON_SEC_FAULT_MASK  )  | \
                                  (0 * BIST_SEC_FAULT_MASK     )  | \
                                  (0 * OR_NFLT2_SEC_MASK       )  | \
                                  (1 * OR_NFLT1_SEC_MASK       )  | \
                                  (0 * GD_TSD_SEC_FAULT_MASK   )  | \
                                  (0 * GD_TWN_SEC_FAULT_MASK   )   )

#define  STATUS5_FAULT_MASK       (1 * ADC_FAULT_MASK)

// =============================================================================
// Local enumerations
enum {
    FAIL = 0,
    PASS = 1
};

// =============================================================================
//
// Function prototypes
//
// prototypes for external use
void               Init_UCC5870_Regs(void);
uint16_t           diagnose_UCC5870(uint16_t i);
UCC5870_Status_e   Init_UCC5870(uint32_t * pwm_gpio, uint32_t * pwm_gpio_cfg);

// local prototypes for use within this file
void       cmdUCC5870 (uint16_t cmd);
uint16_t   readRegUCC5870 (uint16_t CA, uint16_t RA);
void       writeRegUCC5870(uint16_t CA, uint16_t RA, uint16_t  data);
uint16_t   writeVerifyRegUCC5870 (uint16_t CA, uint16_t RA, uint16_t  data);
uint16_t   writeVerify_UCC5870(uint16_t CA);

// =============================================================================
// Local function macros
#define  UCC5870_DRV_EN(CA)       sendCmdUCC5870((CA << 12) | 0x009);
#define  UCC5870_DRV_DIS(CA)      sendCmdUCC5870((CA << 12) | 0x00a);
#define  UCC5870_CFG_IN(CA)       sendCmdUCC5870((CA << 12) | 0x222);
#define  UCC5870_NOP(CA)          sendCmdUCC5870((CA << 12) | 0x542);
#define  UCC5870_SWRESET(CA)      sendCmdUCC5870((CA << 12) | 0x708);

#define  UCC5870_WR_CA(CA)        sendCmdUCC5870((BROADCAST << 12) | (0x0da0 + CA));
#define  UCC5870_WR_REG(CA, RA)   sendCmdUCC5870((CA << 12) | (0x0c00 + RA));
#define  UCC5870_WR_DH(CA, DH)    sendCmdUCC5870((CA << 12) | (0x0a00 + DH));
#define  UCC5870_WR_DL(CA, DL)    sendCmdUCC5870((CA << 12) | (0x0b00 + DL));
#define  UCC5870_RD_REG(CA, RA)   sendCmdUCC5870((CA << 12) | (0x0100 + RA));

#define  CLEAR_FAULTS(GATE_DRV)   writeRegUCC5870 (GATE_DRV, CONTROL2, 0x8000)

// =============================================================================
//
// Global variables
//
UCC5870_Vars  ucc5870[DRIVER_NUM] =
  {
    UCC5870_DEFAULTS,
    UCC5870_DEFAULTS
  };

// =============================================================================
uint16_t     tx_data;  // to monitor cmd in debug window - can be commented out

uint16_t     GD[DRIVER_NUM] =
  {
    UH_ADDRESS,
    UL_ADDRESS
  };

// =============================================================================
uint16_t  statusRegAdrs[5] =
        {
          STATUS1, STATUS2, STATUS3, STATUS4, STATUS5
        },
        statusFaultMask[5] =
        {
          STATUS1_FAULT_MASK, STATUS2_FAULT_MASK, STATUS3_FAULT_MASK,
          STATUS4_FAULT_MASK, STATUS5_FAULT_MASK
        };

// =============================================================================
//
// Function definitions
//
uint16_t sendCmdUCC5870(uint16_t cmd)
{
  uint16_t tmp;
  tx_data = cmd;      // to monitor cmd in debug window - can be commented out  
  digitalWrite(10, LOW);
  tmp = SPI.transfer16(tx_data);
  digitalWrite(10, HIGH);

  return tmp;
}

// =============================================================================
uint16_t readRegUCC5870(uint16_t chipAddress,
                        uint16_t regAddress)
{
    uint16_t tmp;
    UCC5870_RD_REG(chipAddress, regAddress);
    tmp = UCC5870_NOP(chipAddress);

    return(tmp);
}

// =============================================================================
void writeRegUCC5870(uint16_t chipAddress,
                     uint16_t regAddress,
                     uint16_t  data)
{
  UCC5870_WR_REG(chipAddress,  regAddress);
  UCC5870_WR_DH (chipAddress, (data / 256  ));
  UCC5870_WR_DL (chipAddress, (data % 256  ));

  return;
}

// =============================================================================
// Initialize UCC5870 registers

UCC5870_STATUS1_REG * status1[DRIVER_NUM];
UCC5870_STATUS2_REG * status2[DRIVER_NUM];
UCC5870_STATUS3_REG * status3[DRIVER_NUM];
UCC5870_STATUS4_REG * status4[DRIVER_NUM];

void Init_UCC5870_Regs(void)
{
  uint16_t  i, j;
  uint16_t  *srcPtr, *dstPtr;
    //
    // CFG1 register settings
    //
    ucc5870[UH].cfg1.bit.GD_TWN_DIS     = OT_warning_enable;
    ucc5870[UH].cfg1.bit.IO_DEGLITCH    = io_deglitch_70ns;
    ucc5870[UH].cfg1.bit.NFLT2_DOUT_MUX = nFLT2_DOUT_nFLT2;
    ucc5870[UH].cfg1.bit.OV1_DIS        = vcc1_ovlo_enable;
    ucc5870[UH].cfg1.bit.UV1_DIS        = vcc1_uvlo_enable;
    ucc5870[UH].cfg1.bit.OVLO1_LEVEL    = vcc1_3V3;
    ucc5870[UH].cfg1.bit.UVLO1_LEVEL    = vcc1_3V3;
    ucc5870[UH].cfg1.bit.TDEAD          = Tdead(245);  // in ns

    //
    // CFG2 register settings
    //
    ucc5870[UH].cfg2.bit.PWM_CHK_FAULT      = report_fault_on_nFLT1;     //  PWM check fault
    ucc5870[UH].cfg2.bit.VREG1_ILIMIT_FAULT = report_fault_on_nFLT1;     //  Vreg Ilimit fault
    ucc5870[UH].cfg2.bit.GD_TWN_FAULT       = warning_on_nFLT1;          //  temp warning fault
    ucc5870[UH].cfg2.bit.BIST_FAULT         = report_fault_on_nFLT1;     //  Analog BIST fault
    ucc5870[UH].cfg2.bit.TRIM_CRC_FAULT     = report_fault_on_nFLT1;     //  CRC fault
    ucc5870[UH].cfg2.bit.INT_REG_FAULT      = report_fault_on_nFLT1;     //  Internal regulator fault
    ucc5870[UH].cfg2.bit.CFG_CRC_FAULT      = report_fault_on_nFLT1;     //  reg CRC fault
    ucc5870[UH].cfg2.bit.SPI_FAULT          = report_SPI_fault_on_nFLT1; //dont_report_SPI_fault;  //  SPI fault
    ucc5870[UH].cfg2.bit.CLK_MON_FAULT      = report_fault_on_nFLT1;     //  Clock monitor fault
    ucc5870[UH].cfg2.bit.STP_FAULT          = report_fault_on_nFLT1;     //  STP fault
    ucc5870[UH].cfg2.bit.UVLO1_FAULT        = report_fault_on_nFLT1;     //  UVLO fault
    ucc5870[UH].cfg2.bit.OVLO1_FAULT        = report_fault_on_nFLT1;     //  OVLO fault
    ucc5870[UH].cfg2.bit.INT_COMM_FAULT     = report_comm_fault_on_nFLT1;//  Inter-die comm failure

    //
    // CFG3 register settings
    //
    ucc5870[UH].cfg3.bit.AI_IZTC_SEL             = AI1_AI3_AI5_bias_currents_off; //
    ucc5870[UH].cfg3.bit.FS_STATE_CFG_CRC_FAULT  = OUTx_LOW; //
    ucc5870[UH].cfg3.bit.ITO2_EN                 = CURRENT_SRC_OUT_AI2_4_6_DISABLED; //
    ucc5870[UH].cfg3.bit.ITO1_EN                 = CURRENT_SRC_OUT_AI1_3_5_DISABLED; //
    ucc5870[UH].cfg3.bit.FS_STATE_INT_COMM_FAULT = OUTx_LOW; //
    ucc5870[UH].cfg3.bit.FS_STATE_INT_REG_FAULT  = OUTx_LOW; //
    ucc5870[UH].cfg3.bit.FS_STATE_SPI_FAULT      = FS_SPI_OUTx_LOW; //
    ucc5870[UH].cfg3.bit.FS_STATE_STP_FAULT      = FS_STP_OUTx_LOW; //
    ucc5870[UH].cfg3.bit.FS_STATE_PWM_CHK        = OUTx_LOW; //
    ucc5870[UH].cfg3.bit.FS_STATE_OVLO1_FAULT    = OUTx_LOW; //
    ucc5870[UH].cfg3.bit.FS_STATE_UVLO1_FAULT    = OUTx_LOW; //

    //
    // CFG4 register settings
    //
    ucc5870[UH].cfg4.bit.UVOV3_EN        = VEE2_uvlo_ovlo_enable; //
    ucc5870[UH].cfg4.bit.PS_TSD_EN       = Switch_thermal_SD_disable; //
    ucc5870[UH].cfg4.bit.OCP_DIS         = OCP_disable; //
    ucc5870[UH].cfg4.bit.SCP_DIS         = SCP_disable; //
    ucc5870[UH].cfg4.bit.DESAT_EN        = DESAT_det_disable;//DESAT_det_enable; //
    ucc5870[UH].cfg4.bit.VCECLP_EN       = VCE_CLAMP_enable; //
    ucc5870[UH].cfg4.bit.MCLP_DIS        = MILLER_CLAMP_enable; //
    ucc5870[UH].cfg4.bit.GM_EN           = Gate_Volt_Mon_enable;
    ucc5870[UH].cfg4.bit.GM_BLK          = Gate_volt_mon_blank_2500ns; //
    ucc5870[UH].cfg4.bit.MCLP_CFG        = MILLER_CLAMP_internal; //
    ucc5870[UH].cfg4.bit.OV2_DIS         = VCC2_ovlo_enable; //
    ucc5870[UH].cfg4.bit.DESAT_DEGLITCH  = DESAT_deglitch_316ns; //
    ucc5870[UH].cfg4.bit.PS_TSD_DEGLITCH = TSD_1000ns; //
    ucc5870[UH].cfg4.bit.UV2_DIS         = VCC2_uvlo_enable; //

    //
    // CFG5 register settings
    //
    ucc5870[UH].cfg5.bit.PWM_MUTE_EN     = PWM_MUTE_FOR_SC_OC_OT_FAULTS_DISABLED;
    ucc5870[UH].cfg5.bit.TLTOFF_STO_EN   = STO_FOR_SC_DESAT_OC;
    ucc5870[UH].cfg5.bit.STO_CURR        = SOFT_TURN_OFF_0_3A;
    ucc5870[UH].cfg5.bit.MCLPTH          = MILLER_CLAMP_VTH_3V;
    ucc5870[UH].cfg5.bit.DESAT_DCHG_EN   = DESAT_DISCHARGE_DISABLE;//DESAT_DISCHARGE_ENABLE;
    ucc5870[UH].cfg5.bit.DESAT_CHG_CURR  = BLANK_CAP_CHRG_CUR_600uA;
    ucc5870[UH].cfg5.bit.DESATTH         = DESAT_VTH_2p5;
    ucc5870[UH].cfg5.bit.GM_STO2LTO_DIS  = GATE_MON_FLT_DURING_STO_TLTOFF_ENABLED;

    //
    // CFG6 register settings
    //
    ucc5870[UH].cfg6.bit.PS_TSDTH  = TSD_TH_2p75;
    ucc5870[UH].cfg6.bit.TEMP_CURR = TEMP_CURR_0p1A;
    ucc5870[UH].cfg6.bit.OC_BLK    = OCD_BLANK_TIME_0p5us;
    ucc5870[UH].cfg6.bit.SC_BLK    = SCD_BLANK_TIME_0p2us;
    ucc5870[UH].cfg6.bit.SCTH      = SCD_VTH_1p25V;
    ucc5870[UH].cfg6.bit.OCTH      = OCD_VTH_0p5V;

    //
    // CFG7 register settings
    //
    ucc5870[UH].cfg7.bit.FS_STATE_ADC_FAULT = FS_ADC_FLT_OUT_IGNORE;
    ucc5870[UH].cfg7.bit.ADC_FAULT_P        = dont_report_adcfault_on_nFLT1;
    ucc5870[UH].cfg7.bit.ADC_SAMP_DLY       = ADC_SAMP_DELAY_1120ns;
    ucc5870[UH].cfg7.bit.ADC_SAMP_MODE      = ADC_SAMP_CENTER_ALIGN;
    ucc5870[UH].cfg7.bit.ADC_EN             = ADC_SAMP_DISABLE;
    ucc5870[UH].cfg7.bit.OVLO3TH            = VEE2_OVLO_VTH_M10V;
    ucc5870[UH].cfg7.bit.UVLO3TH            = VEE2_UVLO_VTH_M3V;
    ucc5870[UH].cfg7.bit.OVLO2TH            = VCC2_OVLO_VTH_P23V;
    ucc5870[UH].cfg7.bit.UVLO2TH            = VCC2_UVLO_VTH_P12V;

    //
    // CFG8 register settings
    //
    ucc5870[UH].cfg8.bit.IOUT_SEL         = GATE_DRIVE_STRENGTH_FULL;
    ucc5870[UH].cfg8.bit.AI_ASC_MUX       = AI_ASC_MUX_AI;
    ucc5870[UH].cfg8.bit.VREF_SEL         = VREF_INTERNAL;
    ucc5870[UH].cfg8.bit.GD_2LOFF_STO_EN  = GD_2LOFF_STO_ENABLE;
    ucc5870[UH].cfg8.bit.CRC_DIS          = CRC_CHECK_ENABLE;
    ucc5870[UH].cfg8.bit.GD_2LOFF_CURR    = GD_2LOFF_DISCHARGE_CURR_0p9A;
    ucc5870[UH].cfg8.bit.GD_2LOFF_TIME    = GD_2LOFF_PLATEAU_TIME_150ns;
    ucc5870[UH].cfg8.bit.GD_2LOFF_VOLT    = GD_2LOFF_PLATEAU_VOLT_6V;

    //
    // CFG9 register settings
    //
    ucc5870[UH].cfg9.bit.CLK_MON_SEC_FAULT  = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.VREG2_ILIMIT_FAULT = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.BIST_SEC_FAULT     = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.INT_REG_SEC_FAULT  = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.TRIM_CRC_SEC_FAULT = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.CFG_CRC_SEC_FAULT  = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.INT_COMM_SEC_FAULT = report_comm_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.GD_TSD_FAULT       = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.PS_TSD_FAULT       = dont_report_TSD_on_nFLT1;
    ucc5870[UH].cfg9.bit.OVLO23_FAULT       = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.UVLO23_FAULT       = report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.GM_FAULT           = GM_FAULT_ON_nFLT1;  //GM_FAULT_IGNORE;  //
    ucc5870[UH].cfg9.bit.OC_FAULT           = dont_report_fault_on_nFLT1;
    ucc5870[UH].cfg9.bit.SC_FAULT           = dont_report_fault_on_nFLT1;

    //
    // CFG10 register settings
    //
    ucc5870[UH].cfg10.bit.FS_STATE_INT_COMM_SEC  = PULLED_LOW;
    ucc5870[UH].cfg10.bit.FS_STATE_GM            = PULLED_LOW;
    ucc5870[UH].cfg10.bit.FS_STATE_GD_TSD        = PULLED_LOW;
    ucc5870[UH].cfg10.bit.FS_STATE_PS_TSD        = NO_ACTION;
    ucc5870[UH].cfg10.bit.FS_STATE_OCP           = NO_ACTION;
    ucc5870[UH].cfg10.bit.FS_STATE_INT_REG_FAULT = PULLED_LOW;
    ucc5870[UH].cfg10.bit.FS_STATE_DESAT_SCP     = PULLED_LOW;
    ucc5870[UH].cfg10.bit.GD_TWN_SEC_EN          = GD_TWN_SEC_ENABLED;

    //
    // CFG11 register settings
    //
    ucc5870[UH].cfg11.bit.FS_STATE_CLK_MON_SEC_FAULT  = PULLED_LOW;
    ucc5870[UH].cfg11.bit.VCE_CLMP_HLD_TIME           = VCE_CLAMP_HOLD_TIME_100ns;
    ucc5870[UH].cfg11.bit.FS_STATE_CFG_CRC_SEC_FAULT  = PULLED_LOW;
    ucc5870[UH].cfg11.bit.FS_STATE_TRIM_CRC_SEC_FAULT = PULLED_LOW;
    ucc5870[UH].cfg11.bit.FS_STATE_OVLO3              = PULLED_LOW;
    ucc5870[UH].cfg11.bit.FS_STATE_UVLO3              = PULLED_LOW;
    ucc5870[UH].cfg11.bit.FS_STATE_OVLO2              = PULLED_LOW;
    ucc5870[UH].cfg11.bit.FS_STATE_UVLO2              = PULLED_LOW;

    //
    // ADCCFG register settings
    //
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_7  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_6  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_5  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_4  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_3  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_2  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_ON_CH_SEL_1  = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_7 = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_6 = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_5 = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_4 = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_3 = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_2 = DONT_SAMPLE_CH;
    ucc5870[UH].adccfg.bit.ADC_OFF_CH_SEL_1 = DONT_SAMPLE_CH;

    //
    // DOUTCFG register settings
    //
    ucc5870[UH].doutcfg.bit.DOUT_TO_AI1 = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.DOUT_TO_AI3 = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.DOUT_TO_AI5 = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.DOUT_TO_AI2 = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.DOUT_TO_AI4 = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.DOUT_TO_AI6 = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.DOUT_TO_TJ  = DONT_CONNECT_TO_DOUT;
    ucc5870[UH].doutcfg.bit.FREQ_DOUT   = FREQ_DOUT_111p4KHz;
    ucc5870[UH].doutcfg.bit.AI6OCSC_EN  = PROT_DISABLED;
    ucc5870[UH].doutcfg.bit.AI4OCSC_EN  = PROT_DISABLED;
    ucc5870[UH].doutcfg.bit.AI2OCSC_EN  = PROT_DISABLED;
    ucc5870[UH].doutcfg.bit.AI5OT_EN    = PROT_DISABLED;
    ucc5870[UH].doutcfg.bit.AI3OT_EN    = PROT_DISABLED;
    ucc5870[UH].doutcfg.bit.AI1OT_EN    = PROT_DISABLED;

    //
    // copying the configuration settings for all devices
    //
  srcPtr = (uint16_t *) &ucc5870[0];
  for (j=1; j<DRIVER_NUM; j++)
  {
    dstPtr = (uint16_t *) &ucc5870[j];
    for (i=0; i<sizeof(UCC5870_Vars); i++)
    {
      dstPtr[i] = srcPtr[i];
    }
  }

  // new
  for (i=0; i<DRIVER_NUM; i++)
  {
    status1[i] = &ucc5870[i].status1;
    status2[i] = &ucc5870[i].status2;
    status3[i] = &ucc5870[i].status3;
    status4[i] = &ucc5870[i].status4;
  }

  return;
}

// =============================================================================
// Initialize UCC5870
uint16_t initStatus[DRIVER_NUM] = {0, 0},
          diagStatus[DRIVER_NUM] = {0, 0},
          initFault,
          statusFault,
          priReadyFault,
          secReadyFault;

UCC5870_Status_e Init_UCC5870(uint32_t *pwm_gpio)
{
  UCC5870_Status_e   status;

    //
    // Setup Epwm pins (UH, UL, VH, VL, WH and WL) as GPIO outputs
    //

    // =============================================================================
    // Postavit GPIO pin koji se spaja na IN+ kao output (?)
    // Mogu priskočit?

    //GPIO_setMasterCore(pwm_gpio[UH], GPIO_CORE_CPU1);
    //GPIO_setPadConfig(pwm_gpio[UH], GPIO_PIN_TYPE_STD);
    //GPIO_setDirectionMode(pwm_gpio[UH], GPIO_DIR_MODE_OUT);
    //GPIO_writePin(pwm_gpio[UH], 0);
    //GPIO_setPinConfig(pwm_gpio_cfg[UH]);

    // =============================================================================

    //
    // SPI comm and device config
    // Setup Chip Addresses for all UCC5870s - UH, UL, VH, VL, WH and WL
    //
    digitalWrite(pwm_gpio[UH], HIGH);
    UCC5870_WR_CA(GD[UH]); //64929
    digitalWrite(pwm_gpio[UH], LOW);
    
    digitalWrite(pwm_gpio[UL], HIGH);
    UCC5870_WR_CA(GD[UL]); //64930
    digitalWrite(pwm_gpio[UL], LOW);

    //
    // Reset drivers and put it in CONFIG mode
    //
    UCC5870_DRV_DIS(BROADCAST);  // DRV_DIS and SWRESET sequence helps to ..
    UCC5870_SWRESET(BROADCAST);  //  .. reset and restart the drivers
    UCC5870_CFG_IN(BROADCAST);   // put the drivers in config mode
    CLEAR_FAULTS(BROADCAST);     // clear faults at reset before configuring

    //
    // Configure UCC5870 -UH, UL, VH, VL, WH and WL
    //
    initFault = 0;  // logic HIGH bit positions indicate the driver at fault
    initStatus[UH] = writeVerify_UCC5870(UH);
    initFault    += (initStatus[UH] == FAIL) << UH;

    initStatus[UL] = writeVerify_UCC5870(UL);
    initFault    += (initStatus[UL] == FAIL) << UL;
    //
    // check if any driver's written data is not verified
    //
    if (initFault)
    {
        return INIT_FAULT;
    }
    else
    {
      status = inverterDiagnostics();
      if (status == ALL_GOOD)
      {
        UCC5870_DRV_EN(GD[UH]);
        UCC5870_DRV_EN(GD[UL]);
      }
      return(status);
    }
}


// =============================================================================
// Inverter fault diagnostics UCC5870
UCC5870_Status_e inverterDiagnostics(void)
{
  //
  // Diagnose gate driver status and readiness before enabling
  //
  statusFault   = 0;  // logic HIGH bit positions indicate driver at fault
  priReadyFault = 0;  // logic HIGH bit positions indicate driver at fault
  secReadyFault = 0;  // logic HIGH bit positions indicate driver at fault

  diagStatus[UH] = diagnose_UCC5870(UH);
  statusFault   += (diagStatus[UH] == FAIL) << UH;
  priReadyFault += (ucc5870[UH].status2.bit.PRI_RDY == NOT_READY);
  secReadyFault += (ucc5870[UH].status4.bit.SEC_RDY == NOT_READY);

  diagStatus[UL] = diagnose_UCC5870(UL);
  statusFault   += (diagStatus[UL] == FAIL) << UL;
  priReadyFault += (ucc5870[UL].status2.bit.PRI_RDY == NOT_READY);
  secReadyFault += (ucc5870[UL].status4.bit.SEC_RDY == NOT_READY);
  //
  // check if any driver's status register indicates fault
  //
  if (statusFault)
  {
    return STATUS_FAULT;
  }

  //
  // check if any driver's primary ready flag is not set
  //
  if (priReadyFault)
  {
    return PRI_RDY_FAULT;
  }

  //
  // check if any driver's secondary ready flag is not set
  //
  if (secReadyFault)
  {
    return SEC_RDY_FAULT;
  }

  return ALL_GOOD;
}

// =============================================================================
// UCC5870 fault diagnostics
uint16_t diagnose_UCC5870 (uint16_t i)
{
  uint16_t   j,
              faults[5] = {0, 0, 0, 0, 0},
              faultAcc;
  uint16_t * statusReg;

  statusReg = (uint16_t *) &ucc5870[i].status1;
  faultAcc = 0;
  for (j=0; j<4; j++)  // ADC status not checked
  {
    //
    // read STATUSx register / check / accumulate fault flags (HIGH bits)
    //
    statusReg[j] = readRegUCC5870 (GD[i], statusRegAdrs[j]);
    faults[j]    = statusReg[j] & statusFaultMask[j];
    faultAcc    += faults[j];
  }

  return ( (faultAcc == 0) ? PASS : FAIL );  // faultAcc = 0 if all ok
}


// =============================================================================
uint16_t writeVerifyRegUCC5870(uint16_t chipAddress,
                               uint16_t regAddress,
                               uint16_t  data)
{
  uint16_t tmp;

  //Serial.print("Data: ");
  //Serial.println(data);

  writeRegUCC5870(chipAddress, regAddress, data);
  tmp = readRegUCC5870(chipAddress, regAddress);

  //Serial.print("Tmp: ");
  //Serial.println(tmp);

  //(tmp == data) ? Serial.println("PASS") : Serial.println("FAIL");

  return ( (tmp == data) ? PASS : FAIL );
}

// =============================================================================
uint16_t writeVerify_UCC5870(uint16_t i)
{
    uint16_t   failCnt = 0;
    uint16_t * ptr = (uint16_t *) &ucc5870[i];

    //
    // set up the basis for enabling various functions
    //
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG2,    ptr[CFG2]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG3,    ptr[CFG3]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG6,    ptr[CFG6]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG7,    ptr[CFG7]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG9,    ptr[CFG9]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG11,   ptr[CFG11]  ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], ADCCFG,  ptr[ADCCFG] ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], DOUTCFG, ptr[DOUTCFG]) == FAIL);

    //
    // various function enable bits present in these CFG registers
    //
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG1,    ptr[CFG1]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG4,    ptr[CFG4]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG5,    ptr[CFG5]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG8,    ptr[CFG8]   ) == FAIL);
    failCnt += (writeVerifyRegUCC5870 (GD[i], CFG10,   ptr[CFG10]  ) == FAIL);

    return ( (failCnt == 0) ? PASS : FAIL );
}


