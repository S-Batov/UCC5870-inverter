#include <Arduino.h>
#include <SPI.h>
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
  while(!Serial)
#endif

  // Configure local registers
  Init_UCC5870_Regs();

#ifdef DEBUG_MODE
  breakpoint();
#endif

  // Configure driver registers
  Init_UCC5870(PWM_GPIOs);
}

void loop() {
  //Provjeravat serijski port treba li pokrenit DPT
  digitalWrite(IN_UH, HIGH);
  print_status();
//  print_status1_reg(ucc5870[UH].status1.all);
//  print_status1_reg(0);


  delay(10000);
}

/*
interrupt {
  kad se javi fault na nFlt1 ispiši errore
}
*/

/*
interrupt {
  kad se javi fault na nFlt2 ispiši errore
}
*/