#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)  //just more verbose macro, 
#define sPIN_PORT(x) (DDR##x)     //to set port's pins as either input or output

const int analogIn = A0;
int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module
uint8_t RawValue= 0;
int ACSoffset = 2500; 
double Voltage = 0;
double Amps = 0;

void setup(){ 
 Serial.begin(9600);
 adc_init(0);
}

void loop(){
 meas(0);
 RawValue = ADCH;
 Amps = (double) (RawValue - 128)/5.0;
 Voltage = (RawValue / 256.0) * 4450; // Gets you mV
// Amps = ((Voltage - ACSoffset) / mVperAmp);
 
 
 Serial.print("Raw Value = " ); // shows pre-scaled value 
 Serial.print(RawValue); 
 Serial.print("\t mV = "); // shows the voltage measured 
 Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
 Serial.print("\t Amps = "); // shows the voltage measured 
 Serial.println(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point
 delay(2500); 
}

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);               //Vref - Vcc, left adjust result
  ADCSRA = 0;                                       //resest register, for safety reasons
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS0 )|( 1<<ADPS1 );  //ADC on, 2 prescaler
  PIN_PORT(ADC_PORT) &= ~(1<<channels);             //set adc channel
}

//adc measuring function
inline void meas(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
}
