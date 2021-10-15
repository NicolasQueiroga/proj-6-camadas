#include "sw_uart.h"
#pragma GCC optimize("-O3")

void sw_uart_setup(due_sw_uart *uart, int rx, int tx, int stopbits, int databits, int paritybit)
{
	uart->pin_tx = tx;
	uart->stopbits = stopbits;
	uart->paritybit = paritybit;
	uart->databits = databits;
	pinMode(tx, OUTPUT);
	digitalWrite(tx, HIGH);
}

int calc_even_parity(char data)
{
	int ones = 0;
	for (int i = 0; i < 8; i++)
		ones += (data >> i) & 0x01;

	return ones % 2;
}

void _sw_uart_wait_half_T(due_sw_uart *uart)
{
	for (int i = 0; i < 1093; i++)
		asm("NOP");
}

void _sw_uart_wait_T(due_sw_uart *uart)
{
	_sw_uart_wait_half_T(uart);
	_sw_uart_wait_half_T(uart);
}

void sw_uart_write_data(due_sw_uart *uart, char *bufferData, int charLen)
{
	for (int i = 0; i < charLen; i++)
		sw_uart_write_byte(uart, bufferData[i]);
}

void sw_uart_write_string(due_sw_uart *uart, char *stringData)
{
	sw_uart_write_data(uart, stringData, strlen(stringData));
}

void sw_uart_write_byte(due_sw_uart *uart, char data)
{
	int parity = 0;
	if (uart->paritybit == SW_UART_EVEN_PARITY)
		parity = calc_even_parity(data);
	else if (uart->paritybit == SW_UART_ODD_PARITY)
		parity = !calc_even_parity(data);

	// Sending start bit
	digitalWrite(uart->pin_tx, LOW);
	_sw_uart_wait_T(uart);

	// Sending data
	for (int i = 0; i < uart->databits; i++)
	{
		// int bit = data >> i & 1;
		digitalWrite(uart->pin_tx, (data >> i & 1));
		_sw_uart_wait_T(uart);
	}

	if (uart->paritybit != SW_UART_NO_PARITY)
	{
		digitalWrite(uart->pin_tx, parity);
		_sw_uart_wait_T(uart);
	}

	// Sending stop bit
	for (int i = 0; i < uart->stopbits; i++)
	{
		digitalWrite(uart->pin_tx, HIGH);
		_sw_uart_wait_T(uart);
	}
}
