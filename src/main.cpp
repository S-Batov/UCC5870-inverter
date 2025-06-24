#include <Arduino.h>
#include <SPI.h>
#include <PinChangeInterrupt.h>
#include "ucc5870_regs.h"
#include "ucc5870.h"
#include "io_pin_definitions.h"
#include "ucc5870_status_strings.h"

#ifdef DEBUG_MODE
  #include "avr8-stub.h"
#else
  #define debug_init() // Dummy macro
#endif

uint32_t PWM_GPIOs[DRIVER_NUM] =
{
  IN_UH,
  IN_UL
};

bool HS_fault = false;
bool LS_fault = false;

uint8_t temp;

void flt1HS_handler();
void flt2HS_handler();
void flt1LS_handler();
void flt2LS_handler();

void setup() {
  debug_init();

  // Configure GPIO pins
  pinMode(IN_UH, OUTPUT);
  pinMode(IN_UL, OUTPUT);

  // Initialize SPI with MODE1 (CPOL=0, CPHA=1)
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  
#ifndef DEBUG_MODE
  Serial.begin(115200);
  while(!Serial);
#endif

  attachPinChangeInterrupt(digitalPinToPCINT(nFLT1_HS), flt1HS_handler, CHANGE);
  attachPinChangeInterrupt(digitalPinToPCINT(nFLT2_HS), flt2HS_handler, CHANGE);
  attachPinChangeInterrupt(digitalPinToPCINT(nFLT1_LS), flt1LS_handler, CHANGE);
  attachPinChangeInterrupt(digitalPinToPCINT(nFLT2_LS), flt2LS_handler, CHANGE);

  // Configure local registers
  Init_UCC5870_Regs();

#ifdef DEBUG_MODE
  breakpoint();
#endif

  // Configure driver registers
  Init_UCC5870(PWM_GPIOs);

  digitalWrite(IN_UL, LOW);
  analogWrite(IN_UH, 127);
}

void loop() {
  if(HS_fault)
  {
  #ifndef DEBUG_MODE
    Serial.println("HS fault detected");
  #endif
    print_full_status();
    HS_fault = false;
  }
  if(LS_fault)
  {
  #ifndef DEBUG_MODE
    Serial.println("LS fault detected");
  #endif
    print_full_status();
    LS_fault = false;
  }

  delay(1000);
}

void flt1HS_handler() {
  if(digitalRead(nFLT1_HS) == 0)
    HS_fault = true;
}

void flt2HS_handler() {
  if(digitalRead(nFLT1_HS) == 0)
    HS_fault = true;
}

void flt1LS_handler() {
  if(digitalRead(nFLT1_HS) == 0)
    LS_fault = true;
}

void flt2LS_handler() {
  if(digitalRead(nFLT1_HS) == 0)
    LS_fault = true;
}
