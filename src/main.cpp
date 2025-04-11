#include <Arduino.h>
#include <SPI.h>
#include "avr8-stub.h"
#include "ucc5870_regs.h"
#include "ucc5870.h"

//definirat GPIO pinove
#define IN_UH 13
#define IN_UL 12
#define MISO  11
#define MOSI  10
#define CLK   9
#define CS    8

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

  //inicijalizirat SPI
  SPI.begin();

  //kofigurirat lokalno registre
  Init_UCC5870_Regs();

  //konfigurirat drivere
  //int initValue = 
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