

#define F_CPU 8000000UL			/* Define frequency here its 8MHz */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>




//#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
double oldX=0;
double oldY=0;

void USART_Init(long USART_BAUDRATE)
{
	UCSRB |= (1 << RXEN) | (1 << TXEN);/* Turn on transmission and reception */
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);/* Use 8-bit character sizes */
	UBRRL = BAUD_PRESCALE;		/* Load lower 8-bits of the baud rate value */
	UBRRH = (BAUD_PRESCALE >> 8);	/* Load upper 8-bits*/
}

unsigned char UART_RxChar()
{
	while ((UCSRA & (1 << RXC)) == 0);/* Wait till data is received */
	return(UDR);			/* Return the byte*/
}

void UART_TxChar(char ch)
{
	while (! (UCSRA & (1<<UDRE)));	/* Wait for empty transmit buffer*/
	UDR = ch ;
}

void UART_SendString(char *str)
{
	unsigned char j=0;
	
	while (str[j]!=0)		/* Send string till null */
	{
		UART_TxChar(str[j]);	
		j++;
	}
}

void ADC_Init() {
    DDRA = 0x00;
	ADCSRA |= (1 << ADEN); //Enable ADC (bit 7 = 1) enable ADC
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //Set prescaller to 128 (bits 2:0 = 111)
    ADMUX |= (1<<REFS0); //Selecting internal reference voltage
    
}

int ADC_Read(unsigned char channel) {
    ADMUX = (ADMUX & ~0b00001111) | (channel & 0b00001111); //Copy channel bits to 3:0 position
	//ADMUX = 1<< REFS0 | (channel & (1<< MUX0 | 1<< MUX1 | 1<< MUX2 | 1<< MUX3 | 1<< MUX4));//select all the channels to ADC
	//ADCSRA |= (1 << ADEN); //Enable ADC (bit 7 = 1)

    ADCSRA |= (1 << ADSC); //Start taking reading (bit 6 = 1)
    while(((ADCSRA>>ADSC) & 1) == 1) {} //Wait until bit 6 = 0

    return ADC;
}

void SendSerialLong(int64_t value) {
    char buffer[20];  // Adjust buffer size as needed
    snprintf(buffer, sizeof(buffer), "%lld", value);  // Convert long to string

    // Assuming you have a UART_SendString function:
    UART_SendString(buffer);  // Send the formatted string
}
void SendSerialDouble(double value)
{
	char buffer[10];
	dtostrf(value,4,0,buffer);
	//USART_SendString(str);							/* Send Name string */
	UART_SendString(buffer);						/* Send value */
	//USART_TxChar(unit);								/* Send unit char */
	//USART_TxChar('\t');								/* Send tab char */
	_delay_ms(10);
}

void send_uart(unsigned char val){
	while ((UCSRA & (1 << UDRE)) == 0) ; 	//wait until previous transmition is finished
	UDR = val;								//Transmit the charater
}



//FUNCTION to setup TCCR(Timer/Counter Control Register)
void initTimer0CTC(void){ //CTC mode; clock: ps/8 OC0 disconnected
    TCCR0 |= 1<<CS01 | 1<<WGM01; //WGM01 = 1 - CTC(Clear Timer on Compare Match) mode enabled so OCR0(The Output Compare Register) max can be defined ; 
    TIMSK |= 1<<OCIE0; //TIMSK = Timer Counter Interrupt Mask Register; OCI0 = Timer/Counter0 Output Compare Match Interrupt Enabled by setting it to 1
    OCR0=249; //Output Compare Register max value
}


//Interrupt Service Routing for External interrupt 0 ; START the clock INT0 = PD2
ISR(INT0_vect){
	
// this function is called when INT0 bit (PD2) is interrupted.

// You can also use INTERRUPT() function instead.

// SIG_INTERRUPT0 -> INT0 (PD2)

// SIG_INTERRUPT1 -> INT1 (PD3)

// While Button is pressed, LED is on
UART_SendString("stat");
_delay_ms(600);

}
ISR(INT1_vect){

UART_SendString("righ");
_delay_ms(600);

}
ISR(INT2_vect){

UART_SendString("left");
_delay_ms(600);

}

void externalInt(void) {
    DDRD &= ~(1 << PD3); // Setting up DDRD PIN3 as input
    DDRD &= ~(1 << PD2); // Setting up DDRD PIN2 as input
    DDRB &= ~(1 << PB2); // Setting up DDRB PIN2 (PB2) as input for INT2

    GICR |= (1 << INT1) | (1 << INT0) | (1 << INT2); // Enable external interrupt 0, 1, and 2
    MCUCR |= (1 << ISC01) | (1 << ISC10) | (1 << ISC11); // Set ISC10 bit to configure INT1 and INT0 as falling edge triggered interrupts

    // For INT2 (PB2), configure it for a falling or rising edge trigger (choose one):
    // MCUCSR |= (1 << ISC2); // For rising edge trigger
    // MCUCSR &= ~(1 << ISC2); // For falling edge trigger
}



int main(void) {


//////////////////////////////click

	//DDRB = (1<<PB3);  // put PortB bit 5 as output
	initTimer0CTC(); //FUNCTION to setup TCCR(Timer/Counter Control Register)
	sei(); // Enable Interrupts
	externalInt();
//////////////////////////////////////////////////////




   	char c;
    	ADC_Init();
	USART_Init(9600);					/* Initialize USART with 9600 Baud rate */
	char buffer[50];
    	while (1) {
        	double adcX = ADC_Read(1); //get PA0
		SendSerialDouble(adcX);
		_delay_ms(500);
        }            
	return 0;
}
