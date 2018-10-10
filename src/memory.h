#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#include "hex.h"
#include "device.h"

#define MEMORY_PHY_ROM_ADDRESS   0x1D000000
#define MEMORY_PHY_ROM_SIZE      0x00008000

#define MEMORY_PHY_BOOT_ADDRESS  0x1FC00000
#define MEMORY_PHY_BOOT_SIZE     0x00002FEF

#define MEMORY_PHY_RAM_ADDRESS   0x00000000
#define MEMORY_PHY_RAM_SIZE      0x0007FFFF

#define MEMORY_PHY_SFR_ADDRESS   0x1F800000
#define MEMORY_PHY_SFR_SIZE      0x008FFFFF

#define MEMORY_IS(a, f, sz)  (((a) >= (f)) && ((a) < ((f) + (sz))))
#define MEMORY_ISRAM(a)   MEMORY_IS(a, MEMORY_PHY_RAM_ADDRESS,  MEMORY_PHY_RAM_SIZE)
#define MEMORY_ISROM(a)   MEMORY_IS(a, MEMORY_PHY_ROM_ADDRESS,  MEMORY_PHY_ROM_SIZE)
#define MEMORY_ISBOOT(a)  MEMORY_IS(a, MEMORY_PHY_BOOT_ADDRESS, MEMORY_PHY_BOOT_SIZE)
#define MEMORY_ISSFR(a)   MEMORY_IS(a, MEMORY_PHY_SFR_ADDRESS,  MEMORY_PHY_SFR_SIZE)

#define MEMORY_FROMRAM(a)  (a)
#define MEMORY_FROMROM(a)  ((a) - MEMORY_PHY_ROM_ADDRESS)
#define MEMORY_FROMBOOT(a) ((a) - MEMORY_PHY_BOOT_ADDRESS)
#define MEMORY_FROMSFR(a)  ((a) - MEMORY_PHY_SFR_ADDRESS)

enum
{
  DEVM_UNKNOWN,
  DEVM_RAM,
  DEVM_BOOT,
  DEVM_ROM,
  DEVM_SFR,
};

uint32_t  word_at(device_t* dev, uint32_t addr);
uint8_t*  ptr_at (device_t* dev, uint32_t addr);

uint8_t   memtype(device_t* dev, uint32_t addr);
uint8_t*  membase(device_t* dev, uint8_t type);
uint32_t  memrel(device_t* dev, uint32_t addr);


#endif /* _MEMORY_H_ */
