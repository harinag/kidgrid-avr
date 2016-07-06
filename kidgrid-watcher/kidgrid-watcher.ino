/*
	KIDGRID Watcher
	Electric meter LED blinks watcher (Arduino, ATMega328P)
	Author: max@huzm.ru
	(c) "KidGrid" project, 2016
	www.kidgrid.ru
*/

#define IRQ_PIN 2
#define LED_PIN 13
#define IRQ_NO 0
#define LOG_SIZE 5

uint64_t last_time = millis();
uint32_t time_diff = 0;
volatile uint8_t irq_flag = 0;
// IRQ detected millis array
uint32_t blinklog[LOG_SIZE];
uint8_t  logcntr = 0;


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
  //CLKPR = 0b10000000;
  //CLKPR = 0x02;  
  // Set sleep params
  SMCR |= 1;          // enable sleep
  SMCR &= 0b11110001; // IDLE mode
  // INTERRUPT ATTACH
  attachInterrupt(IRQ_NO, onwakeup, FALLING);
}


void loop()
{
  if (irq_flag) {
    if (millis()-last_time < 100) goto a;
    time_diff = millis() - last_time;
	  last_time = millis();
  	/* Add timediff to array */
    blinklog[logcntr++] = time_diff;
    if (logcntr == LOG_SIZE) {
      // send <LOG_SIZE> array values to UART
      for (int i=0; i<LOG_SIZE; i++) {
        Serial.print(blinklog[i]);
        Serial.print('|');
      }
      Serial.write('#');  // end of data pack
      digitalWrite(LED_PIN, HIGH);
      Serial.flush();
      digitalWrite(LED_PIN, LOW);
      logcntr=0;
    }
    a:
    irq_flag=0;
	  attachInterrupt(IRQ_NO, onwakeup, FALLING);
  }
  __asm__  __volatile__("sleep");  // go sleep
}


void onwakeup() {
  detachInterrupt(IRQ_NO);
  irq_flag++;
}
