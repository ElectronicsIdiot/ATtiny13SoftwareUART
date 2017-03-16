#define SERIAL_SEND 3
#define SERIAL_RECEIVE 4
#define SERIAL_RATE 9600

#define SERIAL_BIT_TIME (1000000.0 / SERIAL_RATE) / (1.0 / 9.6 * 64)

void serial_init();

#ifdef SERIAL_SEND
  void serial_send(unsigned char data);
#endif

void serial_receive(unsigned char data);
