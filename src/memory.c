#include <stdio.h>
#include <stdint.h>
#include "memory.h"

/*static int is_empty_line(const uint8_t* block, uint32_t size)
{
  while (size--)
    if (*block++)
      return 0;

  return 1;
}*/

/*void dump_memory(uint32_t offset, const uint8_t* block, uint32_t size, uint8_t div)
{
  uint32_t addr;
  uint32_t n;
  uint8_t byte;
  uint32_t skip;

  addr = 0;
  skip = 0;
  
  
  for (n = 0; n < size; n++)
    {
      addr = offset + n;
      
      if (n % div == 0)
	{
	  if (is_empty_line(block + n, div))
	    {
	      // empty area
	      n += div - 1;
	      skip++;
	      continue;
	    }	  
	  
	  if (skip)
	    printf("\n\n[skipped %d bytes]", skip * div);

	  skip = 0;	  
	  printf("\n[%.8x] ", addr);
	}
      
      byte = block[n];
      printf("%.2x ", byte);
    }

  printf("\n");
}*/

#define word_ptr(p) (*((uint32_t*)p))
#define _ptr(p)     (((uint8_t*)p))

uint32_t word_at(device_t* dev, uint32_t addr)
{
  addr &= 0x1FFFFFFF;
  return word_ptr(ptr_at(dev, addr));
}

uint8_t   memtype(device_t* dev, uint32_t addr)
{
  addr &= 0x1FFFFFFF;

  if (MEMORY_ISRAM(addr))  return DEVM_RAM;
  if (MEMORY_ISROM(addr))  return DEVM_ROM;
  if (MEMORY_ISBOOT(addr)) return DEVM_BOOT;
  if (MEMORY_ISSFR(addr))  return DEVM_SFR;

  return DEVM_UNKNOWN;
}

uint32_t memrel(device_t* dev, uint32_t addr)
{
  addr &= 0x1FFFFFFF;

  switch (memtype(dev, addr))
  {
    case DEVM_RAM: return addr - MEMORY_PHY_RAM_ADDRESS;
    case DEVM_ROM: return addr - MEMORY_PHY_ROM_ADDRESS;
    case DEVM_SFR: return addr - MEMORY_PHY_SFR_ADDRESS;
    case DEVM_BOOT:return addr - MEMORY_PHY_BOOT_ADDRESS;
  }

  return -1;
}

uint8_t* membase(device_t* dev, uint8_t type)
{
  switch (type)
  {
    case DEVM_RAM:  return dev->ram;
    case DEVM_ROM:  return dev->rom;
    case DEVM_BOOT: return dev->boot;
    case DEVM_SFR:  return dev->sfr;
  }

  return 0;
}

uint8_t* ptr_at(device_t* dev, uint32_t addr)
{
  uint8_t* pBase;
  
  addr &= 0x1FFFFFFF;
  pBase = membase(dev, memtype(dev, addr));

  return pBase ? pBase + memrel(dev, addr) : 0;
}

