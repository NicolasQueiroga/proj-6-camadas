#include "sw_uart.h"

due_sw_uart uart;
char letra = 'X';

void setup()
{
  Serial.begin(9600);
  sw_uart_setup(&uart, 4, 1, 8, SW_UART_EVEN_PARITY);
}

void loop()
{
  send_byte(&uart, letra);
  delay(600);
}
