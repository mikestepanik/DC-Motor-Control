

/*
 * DC_Motor_Control.c
 *
 * 
 * 
 */ 
// COM3
#include <avr/io.h>

#define BAUD_RATE 250000
#define F_CPU 16e6
#define UBRR_VAL ((F_CPU/16/BAUD_RATE)-1)
int A, B, ALast, BLast, ARising, AFalling, BRising, BFalling, lastEdge, ticks;
int speed_direction = 1;
int pwm_counter = 0;

void uart_init()
{
UBRR0 = UBRR_VAL;
UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}


void pwm_init(void){
 
// Set OCR0B for 50% duty cycle by default
// Duty Cycle = 1 - (OCR0B/256)
OCR2B = 128;
 
// Enable PWM output on OCR0B
TCCR2A |= (1 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (1 << WGM20);
 
// Prescaler of 8
// If F_CPU = 16MHz, then PWM frequency is 61 Hz
TCCR2B |= (1 << CS21);
 
}

void uart_write(uint8_t data)
{
while(!(UCSR0A & (1 << UDRE0)))
{
asm("NOP");
}
UDR0 = data;
}

uint8_t uart_read()
{
while(!(UCSR0A & (1 << RXC0)))
{
asm("NOP");
}
return UDR0;
}

void uart_write_ascii(uint8_t data)
{
data=data+127;
static uint8_t lookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
uint8_t lower_nibble = data & 0x0F;  //Ands with 0000 1111 to cast off upper bit
uint8_t upper_nibble = (data >> 4) & 0x0F;
 
uint8_t lower_char = lookup[lower_nibble];
uint8_t upper_char = lookup[upper_nibble];
 
uart_write(upper_char);
uart_write(lower_char);
}

int main(void)
{

  DDRB |= (1 << PB1);
  DDRD |= (0 << PD2) | (0 << PD3) | (1 << PD5);
  uart_init();
  pwm_init();
  
  
  ///INITIALZIING Timer for 10ms period in CTC mode
  OCR0A = 156;
  //Set timer in CTC mode
  TCCR0A = (1 << WGM01);
  //Set pre-scaler to 1024 --> tried 128 and it didn't work as good
  TCCR0B |= (1 << CS00) | (1 << CS02);
  
  
  
  
  
  ///////////////////////// BEGININNG OF WHILE LOOP/////////////////////////////////////////////////////////////////////////////////
  while(1)
  {
  ALast = A;
  BLast = B;
  
  
  
   ///////READING IN PULSES from pins and CHANGING Variables accordingly///////////////////////////////////////////////////
  if((PIND & 1<< PD3) == 0)
  {
  A=0; //PD3 low means A was low
  }else
  {
  A=1; //otherwise A is high
  }
  
  if((PIND & 1<< PD2) == 0)
  {
  B=0;
  }else{
  B=1;
  }
  ///////////////////////////////////////////////////CHECKING EDGES///////////////////////////////////////////////////
  if(A != ALast && A == 1)
  {
  ARising = 1;
  AFalling = 0;
  
  }else if(A != ALast && A == 0)
  {
  ARising = 0;
  AFalling = 1;
  }else{
  ARising = 0;
  AFalling = 0;
  }
  
  if(B != BLast && B == 1)
  {
      BRising = 1;
  BFalling = 0;
    
  }else if(B != BLast && B == 0)
  {
  BRising = 0;
  BFalling = 1;   
  }else{
  BRising = 0;
  BFalling = 0;
  }
  ////////////////////////////////CHECKING CLOCKWISE OR COUNTER CLOCKWISE///////////////////////////////////////////////////////////////////////////
  
  //Codes For Each Edge Type
  
  //ARising = 0 , BRising = 1, AFalling = 2, BFalling = 3
  
  //^^^^LAST EDGE DETECTED CHANGES TO ONE OF THE ABOVE VALUES^^^^^^^^^^
  
  
  ///////////A RISING CASE///////////////////
  if(ARising == 1 || BRising == 1 || AFalling == 1 || BFalling == 1)
  {
  if(ARising == 1 && lastEdge == 3)	// For clockwise case to be true, then this must happen --->  CURRENT EVENT: A Rise   PREVIOUS EVENT: B Fall  (we know previous event because Bfalling=3 ---> lastedge=3) 
	//Current Event: A RISED  Previous Event: B FELL (LastEdge=3) 
  { 
  ticks++;									 // its going clockwise so increment ticks
  lastEdge = 0;								//Record what just happened
  }else if(ARising == 1 && lastEdge == 1)
  //Current Event: A RISED Previous Event: B RISED (LastEdge=1)
  {
  ticks--;									// its going counterclockwise so decrement ticks
  lastEdge = 0;								// Record what just happened (A RISED--> so change lastedge to 0)
  }else if(ARising == 1)
  {
  lastEdge = 0;								// None of those are the case so........ only record what just happened
  }
  
  ////////B RISING CASE/////////////////
  if(BRising == 1 && lastEdge == 0)
  {
  ticks++;
  lastEdge = 1;
  }else if(BRising == 1 && lastEdge == 2)
  {
  ticks--;
  lastEdge = 1;
  }else if(BRising == 1)
  {
  lastEdge = 1;
  }

///////A FALLING CASE//////////////
  if(AFalling == 1 && lastEdge == 1)
  {
  ticks++;
  lastEdge = 2;
  }else if(AFalling == 1 && lastEdge == 3)
  {
  ticks--;
  lastEdge = 2;
  }else if(AFalling == 1)
  {
  lastEdge = 2;
  }
  
////////B FALLING CASE/////////////
  if(BFalling == 1 && lastEdge == 2)
  {
  ticks++;
  lastEdge = 3;
  }else if(BFalling == 1 && lastEdge == 0)
  {
  ticks--;
  lastEdge = 3;
  }else if(BFalling == 1)
  {
  lastEdge = 3;
  }
  
  }
    
  
  
  
  ////////////////////  10ms EVENT BELOW if statement////////////////////////////////////////////////////
  
  if(TIFR0 & (1 << OCF0A))
  {
TIFR0= (1 << OCF0A);  

uart_write_ascii(ticks);
uart_write(',');
ticks=0;
//10ms event
 
 
 
 
 /////////////////////// Implementing a periodic speed change ///////////////////////////
if(speed_direction == 1)
{
pwm_counter +=1; /// slow down
 
}else{
pwm_counter -=1; //speed up
}
 
if(pwm_counter > 255)
{
 
speed_direction = 0;  // getting slower
}else if(pwm_counter < 1)
{
speed_direction = 1;  //getting faster
 
}
 
OCR2B = pwm_counter;
 
  }
  
  }
}

