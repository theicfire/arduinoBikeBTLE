/*********************************************************************
This is an example for our nRF8001 Bluetooth Low Energy Breakout

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1697

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Kevin Townsend/KTOWN  for Adafruit Industries.
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

// This version uses call-backs on the event and RX so there's no data handling in the main loop!

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9
#define LIGHTS 7
#define SIREN 6
#define CHAIN 5

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
bool chainoff = true;


/**************************************************************************/
/*!
    This function is called whenever select ACI events happen
*/
/**************************************************************************/
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
    case ACI_EVT_DEVICE_STARTED:
      Serial.println(F("Advertising started"));
      break;
    case ACI_EVT_CONNECTED:
      Serial.println(F("Connected!"));
      uart.print("connected");
      break;
    case ACI_EVT_DISCONNECTED:
      Serial.println(F("Disconnected or advertising timed out"));
      break;
    default:
      break;
  }
}

/**************************************************************************/
/*!
    This function is called whenever data arrives on the RX channel
*/
/**************************************************************************/
void rxCallback(uint8_t *buffer, uint8_t len)
{
  Serial.print(F("Received "));
  Serial.print(len);
  Serial.print(F(" bytes: "));
  for(int i=0; i<len; i++)
   Serial.print((char)buffer[i]); 

  Serial.print(F(" ["));

  for(int i=0; i<len; i++)
  {
    Serial.print(" 0x"); Serial.print((char)buffer[i], HEX); 
  }
  Serial.println(F(" ]"));
  if (compare(buffer, (uint8_t*) "lon", 3)) {
      digitalWrite(7, HIGH);
  } else if (compare(buffer, (uint8_t*) "loff", 4)) {
      digitalWrite(7, LOW);
  } else if (compare(buffer, (uint8_t*) "chainon", 7)) {
      chainoff = false;
  } else if (compare(buffer, (uint8_t*) "sirshort", 8)) {
      digitalWrite(SIREN, HIGH);
      delay(100);
      digitalWrite(SIREN, LOW);
  } else if (compare(buffer, (uint8_t*) "sirmedium", 9)) {
      digitalWrite(SIREN, HIGH);
      delay(1000);
      digitalWrite(SIREN, LOW);
  } else if (compare(buffer, (uint8_t*) "sirforever", 10)) {

      digitalWrite(SIREN, HIGH);
  }

  /* Echo the same data back! */
  uart.write(buffer, len);
}

bool compare(uint8_t* b1, uint8_t* b2, uint8_t len) {
  for (int i = 0; i < len; i++) {
    if (b1[i] != b2[i]) {
      return false;
    }
  }
  return true;
}

/**************************************************************************/
/*!
    Configure the Arduino and start advertising with the radio
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Callback Echo demo"));

  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  // uart.setDeviceName("NEWNAME"); /* 7 characters max! */
  uart.begin();
  pinMode(LIGHTS, OUTPUT);
  digitalWrite(LIGHTS, LOW);
  pinMode(SIREN, OUTPUT);
  digitalWrite(SIREN, LOW);
  pinMode(CHAIN, INPUT);
}

/**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/
void loop()
{
  uart.pollACI();
  if (!chainoff && digitalRead(CHAIN) == LOW) {
    uart.print("chainoff");
    chainoff = true;
  }
}
