/* 
 * Sonsorboard New Generation
 * GRN / Mai 2022
 * 
 * 
*/

#define F_CPU 8000000UL                 // set the CPU clock
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "st7735.h"
#include "grn_TWI.h"
#include "EEPROM_64.h"
#include "spi.h"


#define BACKLIGHT_ON PORTB |= (1<<PB2)
#define BACKLIGHT_OFF PORTB &= ~(1<<PB2)						

#define LED_OFF PORTC &= ~(1<<PC3)
#define LED_ON PORTC |= (1<<PC3)

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL)))-1)
//Buttons 


/* some RGB color definitions                                                 */
#define BLACK        0x0000      
#define RED          0x001F      
#define GREEN        0x07E0      
#define YELLOW       0x07FF      
#define BLUE         0xF800      
#define CYAN         0xFFE0      
#define White        0xFFFF     
#define BLUE_LIGHT   0xFD20      
#define TUERKISE     0xAFE5      
#define VIOLET       0xF81F		
#define WHITE		0xFFFF

#define SEK_POS 10,110

#define RELOAD_ENTPRELL 1 


#define TW_START 0xA4 // send start condition (TWINT,TWSTA,TWEN)
#define TW_READY (TWCR & 0x80) // ready when TWINT returns to logic 1.
#define TW_STATUS (TWSR & 0xF8) // returns value of status register
#define TW_SEND 0x84 // send data (TWINT,TWEN)

#define 	B_BLUE		!(PIND & (1<<PD6))
#define 	B_GREEN		!(PIND & (1<<PD2))
#define	B_YELLOW	!(PIND & (1<<PD5))

#define F_SCL 100000L // I2C clock speed 100 kHz
//#define F_CPU 800000L // I2C clock speed 100 kHz


// Pins already defined in st7735.c
extern int const DC;
extern int const MOSI;
extern int const SCK;
extern int const CS;
// Text scale and plot colours defined in st7735.c
extern int fore; 		// foreground colour
extern int back;      	// background colour
extern int scale;     	// Text size


volatile uint8_t ms10,ms100,sec,min, entprell;
volatile uint16_t speedtest, speedflag;

char stringbuffer[20]; // buffer to store string 
uint8_t addresse, position;//test twi ausgabe

ISR (TIMER1_COMPA_vect);
void twi_Init(void);
void twi_Start(void);
void twi_Stop(void);
void twi_Write(uint8_t u8data);
uint8_t twi_GetStatus(void);




//UART
void uart_send_char(char c);
void uart_send_string(volatile char *s);
ISR(USART0_RX_vect);

int main(void)
{
	DDRB |= (1<<DC) | (1<<CS) | (1<<MOSI) |( 1<<SCK); 	// All outputs
	PORTB = (1<<SCK) | (1<<CS) | (1<<DC);          		// clk, dc, and cs high
	DDRB |= (1<<PB2);									//lcd Backlight output
	PORTB |= (1<<CS) | (1<<PB2);                  		// cs high
	DDRC |= (1<<PC3);									//Reset Output
	DDRD |= (1<<PD7);									//Reset Output
	PORTD |= (1<<PD7);	
									//Reset High
	DDRD &= ~((1<<PD6) | (1<<PD2) | (1<<PD5)); 	//Taster 1-3
	PORTD |= ((1<<PD6) | (1<<PD2) | (1<<PD5)); 	//PUllups f??r Taster einschalten
	
	DDRD |= (1<<PD1)| (1<<PD0);//set TX0 as output
	DDRD &= ~(1<<PD0);//set Rx as Input
	//Timer 1 Configuration
	OCR1A = 1249;	//OCR1A = 0x3D08;==1sec
	    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A
    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // set prescaler to 64 and start the timer
    /* Enable SPI, Master, set clock rate fck/2 */
	SPCR = (1<<SPE)|(1<<MSTR); 
	SPSR = (1<<SPI2X);
	
	//UART0
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);	//Turn on RX and TX circuits RXCIE0 enables Interrupt when byte received
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);	//8-Bit Char size
	UBRR0H = (BAUD_PRESCALE >> 8);	//load upper 8-Bits of baud rate value into high byte of UBRR0H
	UBRR0L = BAUD_PRESCALE;			//load lower 8-Bits of Baud rate into low byte of UBRR0L
	
    sei();// enable interrupts
    
    ms10=0;
    ms100=0;
    sec=0;
    min=0;
    entprell=0;
	
	BACKLIGHT_ON;
	LED_ON;

	setup();
	//SPI_MasterInit();
	TWIInit();

	addresse=0;
	position=20;
		
	fore = WHITE; // White
	scale = 1;//font size
	
	speedtest=0;
	speedflag=1;
	
	MoveTo(0,80);
	fore = WHITE;//set color
	FillRect(40,40);
	
	fore = YELLOW;//set color
	uart_send_string("Hello\n\r");
	speedflag=0; // Stop speedcounter
	
	MoveTo(10,0);
	scale=2;
	sprintf(stringbuffer,"ms = %d",speedtest);
	PlotString(stringbuffer);
	
	fore=WHITE;
	/*MoveTo(10,10);
	sprintf(stringbuffer,"r:%d",234);
	PlotString(stringbuffer);*/
	scale=2;
	  for (;;)
	  {
			
			
			if(B_BLUE)sec++;
			if(B_GREEN)sec--;
			if(B_YELLOW)sec=0;
			
							
		}//end of for()
}//end of main

ISR (TIMER1_COMPA_vect)
{
	ms10++;
	
	if(speedflag==1)speedtest++;
			
	if(ms10==10)	//10ms
	{
		ms10=0;
		ms100++;
	}
    if(ms100==10)	//100ms
	{
		ms100=0;
		sec++;
	}
	if(sec==60)	//Minute
	{
		sec=0;
		min++;
	}
}

void uart_send_char(char c)
{
	while((UCSR0A & (1<<UDRE0)) == 0){};
    UDR0 = c;
}
void uart_send_string(volatile char *s)
{
	while(*s != 0x00)
	{
		uart_send_char(*s);
		s++;
	}
}//end of send_string
