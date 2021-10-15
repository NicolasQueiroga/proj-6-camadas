#include "sw_uart.h"
#pragma GCC optimize("-O3")

/**
 * Inicializa os atributos da estrutura `due_sw_uart`
 * declarados em `sw_uart.h`.
 *
 * @param uart aponta para a estrutura uart com atributos
 * 		inicializados em `sw_uart.h`.
 * @param rx pino digital do arduino que representa o `rx`.
 * @param stopbits indica a quantidade de stop bits.
 * @param databits indica a quantidadde de bits da
 * 		mensagem a ser enviada.
 * @param paritybit indica qual a paridade da mensagem a ser
 * 		enviada, com suas possíveis definições em `sw_uart.h`.
 */
void sw_uart_setup(due_sw_uart *uart, int rx, int stopbits, int databits, int paritybit)
{
  uart->pin_rx = rx;
  uart->stopbits = stopbits;
  uart->paritybit = paritybit;
  uart->databits = databits;
  pinMode(rx, INPUT);
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
 * Recebe um byte pela porta digital `pin_rx`
 * no formato: 0 xxxxxxxx x 1
 *
 * @param uart aponta para o objeto uart.
 * @param data aponta para um caractere na memória (char array ou string).
 */
int sw_uart_receive_byte(due_sw_uart *uart, char *data)
{
  while (digitalRead(uart->pin_rx) == HIGH)
  {
  }

  Serial.println("\nchegou byte");
  _sw_uart_wait_half_T(uart);
  if (digitalRead(uart->pin_rx) == HIGH)
    return SW_UART_ERROR_FRAMING;

  _sw_uart_wait_T(uart);

  char aux = 0x00;
  for (int i = 0; i < uart->databits; i++)
  {
    aux |= digitalRead(uart->pin_rx) << i;
    _sw_uart_wait_T(uart);
  }

  int rx_parity = 0;
  if (uart->paritybit != SW_UART_NO_PARITY)
  {
    rx_parity = digitalRead(uart->pin_rx);
    _sw_uart_wait_T(uart);
  }

  for (int i = 0; i < uart->stopbits; i++)
  {
    if (digitalRead(uart->pin_rx) == LOW)
      return SW_UART_ERROR_FRAMING;
    _sw_uart_wait_T(uart);
  }

  int parity = 0;
  if (uart->paritybit == SW_UART_EVEN_PARITY)
    parity = calc_even_parity(aux);
  else if (uart->paritybit == SW_UART_ODD_PARITY)
    parity = !calc_even_parity(aux);

  if (parity != rx_parity)
    return SW_UART_ERROR_PARITY;

  *data = aux;
  return SW_UART_SUCCESS;
}

/**
 * Realiza uma espera de T/2 (meio período)
 *
 * @param uart aponta para o objeto uart.
 */
void _sw_uart_wait_half_T(due_sw_uart *uart)
{
  for (int i = 0; i < 1093; i++)
    asm("NOP");
}

/**
 * Realiza uma espera de T (um período)
 *
 * @param uart aponta para o objeto uart.
 */
void _sw_uart_wait_T(due_sw_uart *uart)
{
  _sw_uart_wait_half_T(uart);
  _sw_uart_wait_half_T(uart);
}
