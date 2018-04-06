char msg[] = {'1','2','3','a','b','c',' ',' ',' '};
char *ptr = msg;
long index = 0;


void setup() {
  // put your setup code here, to run once:

  UBRR0H=0;
  UBRR0L=1;//0 = 2M baud rate. 1 = 1M baud. 3 = 0.5M. 7 = 250k 207 is 9600 baud rate.
  UCSR0A|=(1<<U2X0);//double speed aysnc
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);//Enable receiver and transmitter
  UCSR0C=6;//async 1 stop bit 8bit char no parity bits
}

void loop() {
  // put your main code here, to run repeatedly:
  for(uint16_t i = 0; i < 1000; i++){
    UDR0 = i;
    while(!( UCSR0A & (1<<UDRE0)));//wait for byte to transmit
  }
  _delay_ms(700);


}
