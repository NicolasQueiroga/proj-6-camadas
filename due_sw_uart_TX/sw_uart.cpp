#include "sw_uart.h"
#pragma GCC optimize("-O3")

/**
 * Inicializa os atributos da estrutura `due_sw_uart`
 * declarados em `sw_uart.h`.
 *
 * @param uart aponta para a estrutura uart com atributos
 * 		inicializados em `sw_uart.h`.
 * @param tx pino digital do arduino que representa o `tx`.
 * @param stopbits indica a quantidade de stop bits.
 * @param databits indica a quantidadde de bits da
 * 		mensagem a ser enviada.
 * @param paritybit indica qual a paridade da mensagem a ser
 * 		enviada, com suas possíveis definições em `sw_uart.h`.
 */
void sw_uart_setup(due_sw_uart *uart, int tx, int stopbits, int databits, int paritybit)
{
	uart->pin_tx = tx;
	uart->stopbits = stopbits;
	uart->databits = databits;
	uart->paritybit = paritybit;

	pinMode(tx, OUTPUT);
	digitalWrite(tx, HIGH);
}

/**
 * Checa a paridade do byte o qual um
 * cararctere é composto.
 *
 * @param data caractere a ser enviado.
 * @return 1 se ímpar, 0 se par.
 */
int calc_even_parity(char data)
{
	int ones = 0;
	for (int i = 0; i < 8; i++)
		ones += (data >> i) & 0x01;

	return ones % 2;
}

/**
 * Envia um byte pela porta digital `pin_tx`
 * no formato: 0 xxxxxxxx x 1
 *
 * @param uart aponta para a estrutura uart com atributos
 * 		inicializados em `sw_uart.h`.
 * @param letra caractere a ser enviado.
 */
void send_byte(due_sw_uart *uart, char letra)
{
	digitalWrite(uart->pin_tx, LOW);
	_sw_uart_wait_T(uart);

	for (int shift = 0; shift < uart->databits; shift++)
	{
		// https://www.educative.io/edpresso/how-to-convert-an-integer-into-a-specific-byte-array-in-cpp
		digitalWrite(uart->pin_tx, (letra >> shift & 1));
		_sw_uart_wait_T(uart);
	}

	int parity = 0;
	if (uart->paritybit == SW_UART_EVEN_PARITY)
		parity = calc_even_parity(letra);
	else if (uart->paritybit == SW_UART_ODD_PARITY)
		parity = !calc_even_parity(letra);

	if (uart->paritybit != SW_UART_NO_PARITY)
	{
		digitalWrite(uart->pin_tx, parity);
		_sw_uart_wait_T(uart);
	}

	for (int i = 0; i < uart->stopbits; i++)
	{
		digitalWrite(uart->pin_tx, HIGH);
		_sw_uart_wait_T(uart);
	}
}

/**
 * Realiza uma espera de T/2 (meio período)
 *
 * @param uart aponta para a estrutura uart com atributos
 * 		inicializados em `sw_uart.h`.
 */
void _sw_uart_wait_half_T(due_sw_uart *uart)
{
	for (int i = 0; i < 1093; i++)
		asm("NOP");
}

/**
 * Realiza uma espera de T (um período)
 *
 * @param uart aponta para a estrutura uart com atributos
 * 		inicializados em `sw_uart.h`.
 */
void _sw_uart_wait_T(due_sw_uart *uart)
{
	_sw_uart_wait_half_T(uart);
	_sw_uart_wait_half_T(uart);
}
