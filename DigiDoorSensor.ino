/*
Wireless Door/Windows Sensor
Reed Switch, RF 433 Transmiter And Digispark TYNY85
*/
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
#define RF_DATA_PIN 2 // RF433 transmitter plugged on P2

const int switchPin = 0;
const int ledPin = 1;

void setup() {
 pinMode(switchPin, INPUT);
 pinMode(ledPin, OUTPUT);
 digitalWrite(switchPin, HIGH);
 
 mySwitch.enableTransmit(RF_DATA_PIN);
 // Optional set protocol (default is 1, will work for most outlets)
 // mySwitch.setProtocol(2);

 // Optional set pulse length.
 // mySwitch.setPulseLength(320);
  
 // Optional set number of transmission repetitions.
 // mySwitch.setRepeatTransmit(15);
}

void loop() {
 flashLED(ledPin,200);
 switch (digitalRead(switchPin)) {
  case LOW:
    //digitalWrite(ledPin, LOW);
    //Show status on Led
    /* See Example: TypeA_WithDIPSwitches */
    mySwitch.switchOn("11111", "00010");
    /* Same switch as above, but using decimal code */
    //mySwitch.send(5393, 24);
    /* Same switch as above, but using binary code */
    //mySwitch.send("000000000001010100010001");
    /* Same switch as above, but tri-state code */ 
    //mySwitch.sendTriState("00000FFF0F0F");
    break;
  case HIGH:
    //Show status on Led
    //digitalWrite(ledPin, HIGH);
    /* See Example: TypeA_WithDIPSwitches */
    mySwitch.switchOff("11111", "00010");
    /* Same switch as above, but using decimal code */
    //mySwitch.send(5396, 24);
    /* Same switch as above, but using binary code */
    //mySwitch.send("000000000001010100010100");
    /* Same switch as above, but tri-state code */ 
    //mySwitch.sendTriState("00000FFF0FF0");
    break;
  }
 sleepTillChg();
 // continue where we left off
}

void sleepTillChg() {  // 0.02ma drain while sleeping here
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT0);                   // Use PB0 (was PB3) as interrupt pin
    // Turn off unnecessary peripherals
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    ACSR |= _BV(ACD); // Disable analog comparator

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep ... Zzzz

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT0);                  // Turn off PB0 (was PB3) as interrupt pin
    sleep_disable();                        // Clear SE bit
//    ADCSRA |= _BV(ADEN);                    // ADC on
    sei();                                  // Enable interrupts
}

ISR(PCINT0_vect) {
    // This is called when the interrupt occurs, but we don't need to do anything in it
}

void flashLED(byte ledNum, int msecs) {
    digitalWrite(ledNum, HIGH);  // Flash it ON
    delay(msecs);
    digitalWrite(ledNum, LOW);  // off
    delay(msecs);
    digitalWrite(ledNum, HIGH);  // Flash it ON
    delay(msecs);
    digitalWrite(ledNum, LOW);  // off
    delay(msecs);
}
