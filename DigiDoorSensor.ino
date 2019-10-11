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

// Utility macros
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

// Variables for the WDT Sleep/power down modes:
volatile boolean f_wdt = 0;
volatile boolean f_switch = 0;

void setup() {
 pinMode(switchPin, INPUT);
 //pinMode(ledPin, OUTPUT);
 digitalWrite(switchPin, HIGH);
 adc_disable(); // ADC uses ~320uA
 set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 setup_watchdog(9); // approximately 0.5 seconds sleep
}

void loop() {
  if (f_wdt==1) {
   f_wdt=0;
   switch (digitalRead(switchPin)) {
    case LOW:
      /* See Example: TypeA_WithDIPSwitches */
      mySwitch.switchOn("11111", "00010");
      break;
    case HIGH:
      /* See Example: TypeA_WithDIPSwitches */
      mySwitch.switchOff("11111", "00010");
      break;
    }
  }

 if (f_switch==1) {
   f_switch=0;
   switch (digitalRead(switchPin)) {
    case LOW:
      /* See Example: TypeA_WithDIPSwitches */
      mySwitch.switchOn("11111", "00010");
      break;
    case HIGH:
      /* See Example: TypeA_WithDIPSwitches */
      mySwitch.switchOff("11111", "00010");
      break;
    }
  }
  
 sleepTillChg();
}

void sleepTillChg() {  // 0.02ma drain while sleeping here
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT0);                   // Use PB0 (was PB3) as interrupt pin
    // Turn off unnecessary peripherals
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    ACSR |= _BV(ACD); // Disable analog comparator
    cli();                                  // Disable interrupts
    digitalWrite(ledPin, LOW);  // LED off
    pinMode(ledPin, INPUT);
    mySwitch.disableTransmit();             //deactivate the transmitter
//    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sleep_bod_disable();
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep ... Zzzz
    //ZZzzzzzzzzzZZZZZZzzzzzzzz --> WAKE UP!!!
    sleep_disable();                        // Clear SE bit - System continues execution here
    PCMSK &= ~_BV(PCINT0);                  // Turn off PB0 (was PB3) as interrupt pin
    ADCSRA |= _BV(ADEN);                    // ADC on
    sei();                                  // Enable interrupts
    mySwitch.enableTransmit(RF_DATA_PIN);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);  // LED ON
}

ISR(PCINT0_vect) {
   f_switch = 1; // This is called when the interrupt occurs, but we don't need to do anything in it
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

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
 
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}
