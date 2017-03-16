#include "serial.h"

unsigned char buffer[10]; // 10 byte buffer

int main() {
  serial_init(); // Initialise UART

  while(1); // Do a heap of useful calculations
}

unsigned char index; // Current index of buffer

void serial_receive(unsigned char value) { // Called when a byte is received
  buffer[index] = value; // Set buffer at index to value

  if (++index == 10) { // If buffer is full
    for (int i = 0; i < 10; ++i) { // Loop buffer
      serial_send(buffer[i]); // Send byte
    }

    index = 0; // Reset index
  }
}
