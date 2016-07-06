/*
	KIDGRID Watcher
	Electric meter LED blinks watcher (Arduino, ATMega328P)
	Author: max@huzm.ru
	(c) "KidGrid" project, 2016
	www.kidgrid.ru
*/

#define IRQ_PIN 2

const unsigned char irq = digitalPinToInterrupt(IRQ_PIN);
unsigned long last_time = millis();
unsigned short time_diff = 0;
volatile unsigned char irq_flag = 0;


void setup() 
{
  Serial.begin(9600);
  pinMode(IRQ_PIN, INPUT_PULLUP); // interrupt request pin
  pinMode(LED_PIN, OUTPUT);
  // LOW ENERGY PART
  // all pins to output
  for (int i=0; i<20; i++) pinMode(i, OUTPUT);
  // disable ADC
  ACSR &= ~(1<<3);  // disable interrupt
  ACSR |= 1<<7;
  // Put down MCU freq to 4 Mhz
  CLKPR = 0b10000000;
  CLKPR = 0x02;  
  // Set sleep params
  SMCR |= 1;          // enable sleep
  SMCR &= 0b11110001; // IDLE mode
  // INTERRUPT ATTACH
  attachInterrupt(irq, onwakeup, FALLING);
}


void loop()
{
  if (irq_flag) {
    if (millis()-last_time < 100) goto a;
    time_diff = millis() - last_time;
	  last_time = millis();
  	/*
      add to array or send to coordinator
    */
    a:
    irq_flag=0;
	  attachInterrupt(irq, onwakeup, FALLING);
  }
  __asm__  __volatile__("sleep");  // go sleep
}


void onwakeup() {
  detachInterrupt(irq);
  irq_flag++;
}
