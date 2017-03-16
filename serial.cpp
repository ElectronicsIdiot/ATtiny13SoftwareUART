#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"

void serial_init() {
  #ifdef SERIAL_SEND
    DDRB |= 1 << SERIAL_SEND; // Set transmit as output
    PORTB |= 1 << SERIAL_SEND; // Set high (inactive)
  #endif

  #ifdef SERIAL_RECEIVE
    DDRB &= ~(1 << SERIAL_RECEIVE);

    TCCR0A |= 1 << WGM01; // clear on compare
    TCCR0B |= (1 << CS01) | (1 << CS00); // set timer clock to 0b011 (clock/64)

    GIMSK = 1 << PCIE; // general intertupt mask enable pin change int
    PCMSK |= 1 << SERIAL_RECEIVE; // Pin change mask?
    sei(); // set int enabled
  #endif
}

#ifdef SERIAL_SEND
  void serial_send(unsigned char val) {
    PORTB &= ~(1 << SERIAL_SEND); // Set low
    _delay_us(1000000 / SERIAL_RATE); // Wait

    for (char i = 0; i < 8; ++i) { // Loop bits
      PORTB = (PORTB & ~(1 << SERIAL_SEND)) | (((val >> i) & 1) << SERIAL_SEND); // Set port to each bit
      _delay_us(1000000 / SERIAL_RATE); // Wait
    }

    PORTB |= 1 << SERIAL_SEND; // Send stop bit
    _delay_us(1000000 / SERIAL_RATE); // Wait
  }
#endif

#ifdef SERIAL_RECEIVE
  volatile unsigned char position; // Current position within byte
  volatile unsigned char data; // Recieved byte

  /*
    WARNING: Receive uses inverted logic, designed to be used with an inverting level shifter
  */
  ISR(TIM0_COMPA_vect) { // Timer interrupt
    if (!(PINB & (1 << SERIAL_RECEIVE))) { // If current bit is set
      data |= (1 << position); // Set corresponding bit in recieved byte
    }

    if (++position == 8) { // If at end of byte
      TIMSK0 &= ~(1 << OCIE0A); // Disable interrupt for compare register A (this interrupt)
      serial_receive(data); // Alert user they have data
      PCMSK |= 1 << SERIAL_RECEIVE; // Enable pin change interrupt to start next byte
    } else {
      OCR0A = SERIAL_BIT_TIME; // Set delay time equal to the length of a single bit
    }
  }

  /*
    WARNING: Receive uses inverted logic, designed to be used with an inverting level shifter
  */
  ISR(PCINT0_vect) { // On pin change
    if (PINB & (1 << SERIAL_RECEIVE)) { // If is a start bit
      TCNT0 = 0; // Set counter to 0
      OCR0A = SERIAL_BIT_TIME * 1.5; // Call timer interrupt in middle of first bit
      position = 0; // Reset position and data
      data = 0;
      TIMSK0 |= 1 << OCIE0A; // Enable interrupt for compare register A (timer interrupt)
      TIFR0 |= 1 << OCF0A; // Clear timer interrupt flag to prevent it jumping directly there
      PCMSK &= ~(1 << SERIAL_RECEIVE); // Disable pin change interrupt
    }
  }
#endif
