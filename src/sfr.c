#include <stdio.h>

#include "device.h"
#include "sfr.h"

uint32_t *sfr_ptr(device_t *dev, uint32_t addr)
{
  uint32_t pos = 0;

  switch (addr & 0xfffffff0)
  {
  case SFR_TRISA:
    pos = 0;
    break;

  case SFR_PORTA:
    pos = 1;
    break;

  case SFR_ODCA:
    pos = 2;
    break;

  case SFR_LATA:
    pos = 3;
    break;

  case SFR_U1MODE:
    pos = 4;
    break;
  case SFR_U1STA:
    pos = 5;
    break;
  case SFR_U1TXREG:
    pos = 6;
    break;
  case SFR_U1RXREG:
    pos = 7;
    break;
  case SFR_U1BRG:
    pos = 8;
    break;
  }

  return (uint32_t *)&dev->sfr[pos * 4];

  //  return (uint32_t *)&dev->sfr[pos];
}

void sfr_port(device_t *dev, uint32_t addr, uint32_t data)
{
  switch (addr & 0x00f0)
  {
  case SFR_GPIO_TRIS:
//    printf("TRIS");
    break;

  case SFR_GPIO_PORT:
//    printf("PORT");
    break;

  case SFR_GPIO_LAT:
//    printf("LAT");
    break;

  case SFR_GPIO_ODC:
//    printf("ODC");
    break;
  }

//  printf(" = %#.4x\n", data);
}

void sfr_uart(device_t *dev, uint32_t addr, uint32_t data)
{
  switch (addr & 0x00f0)
  {
  case SFR_UART_MODE:
    break;

  case SFR_UART_STA:
    break;

  case SFR_UART_TXR:
    putchar((unsigned char)data);
    break;

  case SFR_UART_RXR:
    break;

  case SFR_UART_BRG:
    break;
  }
}

void sfr_set(device_t *dev, uint32_t addr, uint32_t data)
{
  uint32_t *p = sfr_ptr(dev, addr);

  if ((addr & 0xfffff000) == 0x1f886000) // TODO
    sfr_port(dev, addr, data);

  if ((addr & 0xfffff000) == 0x1f806000)  // TODO
    sfr_uart(dev, addr, data);

  // Update data
  *p = data;

  //printf("SFR = %#.8x\n", addr);
}

uint32_t sfr_get(device_t *dev, uint32_t addr)
{
  uint32_t *p = sfr_ptr(dev, addr);
  return *p;
}
