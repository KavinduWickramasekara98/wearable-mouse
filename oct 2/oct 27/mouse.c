

#define F_CPU 8000000UL			/* Define frequency here its 8MHz */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

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
int main(void) {
   	char c;
    	ADC_Init();
	USART_Init(9600);					/* Initialize USART with 9600 Baud rate */
	char buffer[50];
    	while (1) {
        	double adcX = ADC_Read(1); //get PA0
		SendSerialDouble(adcX);
		_delay_ms(90);
        }            
	return 0;
}
