#include <Arduino.h>
#include <SPI.h>
#include "avr8-stub.h"
#include "ucc5870_regs.h"
#include "ucc5870.h"

//definirat GPIO pinove
#define IN_UH 9
#define IN_UL 8
#define MISO  12
#define MOSI  11
#define CLK   13
#define CS    10

uint32_t PWM_GPIOs[DRIVER_NUM] =
{
  IN_UH,
  IN_UL
};

void setup() {
  debug_init();

  //konfigurirat GPIO pinove
  pinMode(IN_UH, OUTPUT);
  pinMode(IN_UL, OUTPUT);


  //inicijalizirat serijski port
  //Serial.begin(9600);
  //while(!Serial){
  //  ;
  //}

  // Initialize SPI with MODE1 (CPOL=0, CPHA=1)
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));

  breakpoint();
  //kofigurirat lokalno registre
  Init_UCC5870_Regs();

  breakpoint();
  //konfigurirat drivere
  int initValue = 
  Init_UCC5870(PWM_GPIOs);
/*
  if (!initValue)
    Serial.println("UCC1: PASS");
  else
    Serial.println("UCC2: FAIL");
*/
}

void loop() {
  //Provjeravat serijski port treba li pokrenit DPT
  digitalWrite(IN_UH, HIGH);
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