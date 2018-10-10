#ifndef _SFR_H_
#define _SFR_H_

#include "device.h"
#include "memory.h"

void      sfr_set(device_t *dev, uint32_t addr, uint32_t data);
uint32_t  sfr_get(device_t *dev, uint32_t addr);
uint32_t* sfr_ptr(device_t* dev, uint32_t addr);

#define SFR_TRISA 0xbf886000
#define SFR_PORTA 0xbf886010
#define SFR_LATA  0xbf886020
#define SFR_ODCA  0xbf886030

#define SFR_GPIO_TRIS 0x00
#define SFR_GPIO_PORT 0x10
#define SFR_GPIO_LAT  0x20
#define SFR_GPIO_ODC  0x30

#define SFR_UART_MODE 0x00
#define SFR_UART_STA  0x10
#define SFR_UART_TXR  0x20
#define SFR_UART_RXR  0x30
#define SFR_UART_BRG  0x40

#define SFR_U1MODE   0xbf806000
#define SFR_U1STA    0xbf806010
#define SFR_U1TXREG  0xbf806020
#define SFR_U1RXREG  0xbf806030
#define SFR_U1BRG    0xbf806040

#endif /* _SFR_H_ */
