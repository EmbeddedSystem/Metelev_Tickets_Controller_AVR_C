#include "main.h"

void OpenTourniquet(void)
{
	sbi(PORTB, PB4);
	sbi(DDRB, PB4);
}

void CloseTourniquet(void)
{
	cbi(PORTB, PB4);
	sbi(DDRB, PB4);
}

void ErrorLedOn(void)
{
	sbi(PORTB, PB2);
	sbi(DDRB, PB2);
}

void ErrorLedOff(void)
{
	cbi(PORTB, PB2);
	sbi(DDRB, PB2);
}

void uartInit(void)
{
	// Set baud rate
	UBRRH = 0;
	UBRRL = 15;
	//Enable receiver and transmitter
	UCSRB = (1 << RXEN) | (1 << TXEN);
}

void uartSendByte(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSRA & (1 << UDRE)))
		;
	/* Put data into buffer, sends the data */
	UDR = data;
}

unsigned char uartGetByte(void)
{
	/* Wait for data to be received */
	if (!(UCSRA & (1 << RXC)))
		return 0xFF;
	else
		/* Get and return received data from buffer */
		return UDR;
}

int main(void)
{
	static u08 state;

	PORTA = 0xFF; // Подтяжки включить
	PORTB = 0xFF;
	PORTD = 0xFF;
	DDRA = 0x00;
	DDRB = 0x00;
	DDRD = 0x00;

	uartInit();
	CloseTourniquet();
	ErrorLedOff();

	/*
	 uartSendByte('S');
	 uartSendByte('T');
	 uartSendByte('A');
	 uartSendByte('R');
	 uartSendByte('T');
	 uartSendByte(' ');
	 */
	while (1)
	{
		switch (state)
		{
		case 0: // Турникет закрыт
			if ('1' == uartGetByte()) // Команда открыть
			{
				//uartSendByte('O');
				OpenTourniquet();
				state = 1;
			}
			if ('4' == uartGetByte()) // Зажечь светодиод ошибки
			{
				ErrorLedOn();
			}
			if ('5' == uartGetByte()) // Погасить светодиод ошибки
			{
				ErrorLedOff();
			}
			break;
		case 1: // Турникет открыт
			if ('2' == uartGetByte()) // Команда закрыть
			{
				//uartSendByte('3');
				CloseTourniquet();
				state = 0;
			}
			if (0 == (PINB & (1 << PB3))) // Сработал датчик прохода
			{
				uartSendByte(53);
				uartSendByte(13);
				uartSendByte(10);
				CloseTourniquet();
				state = 0;
			}
			break;
		}

	}
	return 0;
}
