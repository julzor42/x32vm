#include <stdio.h>
#include <string.h>

#include "hex.h"
#include "memory.h"
#include "mips.h"
#include "decoder.h"
#include "names.h"
#include "device.h"
#include "misa.h"
#include "sfr.h"
#include "gui.h"

///////////////////////////////////////////////////////////////////////////////

static device_callback_t *event_callbacks = 0;

///////////////////////////////////////////////////////////////////////////////

void device_create(device_t *dev)
{
  memset(dev, 0, sizeof(device_t));
}

void device_program(device_t *dev, const char *hex)
{
  hex_context_t cnt;

  hex_init(&cnt, dev->rom, dev->boot);
  hex_read(hex, &cnt);
}

void device_register_event(device_t *dev, DEVICE_FUNC func)
{
  device_callback_t *p = (device_callback_t *)malloc(sizeof(device_callback_t));
  p->func = func;
  p->next = event_callbacks;

  event_callbacks = p;
}

void device_event(device_t *dev, uint32_t event, void *p)
{
  device_callback_t *item;

  for (item = event_callbacks; item; item = item->next)
  {
    item->func(dev, event, p);
  }
}

void device_cycle(device_t *dev)
{
  mips_instr_t instr;

  if (!dev->skip)
  {
    decode_at(dev, &instr, dev->PC);
    misa_dump(dev, &instr);
    misa_call(dev, &instr);
  }
  else
    dev->skip -= 1;

  if (dev->Jump2 != 0xffffffff)
  {
    device_event(dev, DEVICE_EVENT_JUMP, 0);

    // Branch now
    dev->PC = dev->Jump2 - 4;
    dev->Jump2 = 0xffffffff;
  }

  if (dev->Jump != 0xffffffff)
  {
    // Branch on next cycle
    dev->Jump2 = dev->Jump & 0x1fffffff;
    dev->Jump = 0xffffffff;
    
    device_event(dev, DEVICE_EVENT_JUMP_PRE, 0);    
  }

  dev->cycles++;
  dev->PC += 4;

  device_event(dev, DEVICE_EVENT_CYCLE, 0);
}

void device_reset(device_t *dev)
{
  dev->Jump = 0xffffffff;
  dev->Jump2 = 0xffffffff;
  dev->halt = 0;
  dev->refresh = 0;
  dev->skip = 0;

  dev->PC = MEMORY_PHY_BOOT_ADDRESS;

  device_event(dev, DEVICE_EVENT_RESET, 0);
}

void device_loop(device_t *dev)
{
  while (!dev->halt)
  {
    device_cycle(dev);
  }
}

void device_set_boot(device_t *dev, uint8_t *boot, uint32_t size)
{
  dev->boot = boot;
  dev->bootSize = size;
}

void device_set_rom(device_t *dev, uint8_t *rom, uint32_t size)
{
  dev->rom = rom;
  dev->romSize = size;
}

void device_set_ram(device_t *dev, uint8_t *ram, uint32_t size)
{
  dev->ram = ram;
  dev->ramSize = size;
}

void device_set_sfr(device_t *dev, uint8_t *sfr, uint32_t size)
{
  dev->sfr = sfr;
  dev->sfrSize = size;
}

void device_write_sfr(device_t *dev, uint32_t addr, uint32_t data)
{
  uint32_t val = sfr_get(dev, addr);

  // Update value
  if (addr & 0xc)
  {
    // Invert bits
    val ^= data;
  }
  else if (addr & 0x8)
  {
    // Set bits
    val |= data;
  }
  else if (addr & 0x4)
  {
    // Clear bits
    val &= ~data;
  }
  else
  {
    // Set value
    val = data;
  }

  device_event_memory_t e;
  e.address = addr;
  e.value = val;

  device_event(dev, DEVICE_EVENT_SFRWRITE, &e);

  sfr_set(dev, addr, val);
}

void device_write(device_t *dev, uint32_t addr, uint8_t *data, uint32_t size)
{
  addr &= 0x1fffffff;
  device_event_memory_t e;

  e.length = size;
  e.value = 0;
  e.address = addr;

  switch (memtype(dev, addr))
  {
  case DEVM_SFR:
    device_write_sfr(dev, addr, *(uint32_t *)data);
    break;

  case DEVM_RAM:
    memcpy(&e.value, data, size);
    device_event(dev, DEVICE_EVENT_RAMWRITE, &e);

    memcpy(ptr_at(dev, addr), data, size);
    break;

  case DEVM_ROM:
    // Program ROM?
    break;

  case DEVM_BOOT:
    // Impossible?
    break;

  case DEVM_UNKNOWN:
    device_event(dev, DEVICE_EVENT_BADWRITE, &addr);
    break;
  }
}

void device_read(device_t *dev, uint32_t addr, uint8_t *data, uint32_t size)
{
  addr &= 0x1fffffff;
  uint32_t *p = (uint32_t *)data;
  device_event_memory_t e;

  e.address = addr;
  e.value = 0;
  e.length = size;

  switch (memtype(dev, addr))
  {
  case DEVM_SFR:
    *p = sfr_get(dev, addr);

    e.value = *p;
    device_event(dev, DEVICE_EVENT_SFRREAD, &e);
    break;

  case DEVM_RAM:
    memcpy(&e.value, ptr_at(dev, addr), size);
    device_event(dev, DEVICE_EVENT_RAMREAD, &e);

  case DEVM_ROM:
  case DEVM_BOOT:
    memcpy(data, ptr_at(dev, addr), size);
    break;

  case DEVM_UNKNOWN:
    device_event(dev, DEVICE_EVENT_BADREAD, &addr);
    break;
  }
}

void device_run(device_t* dev, const char* s)
{
  device_program(dev, s);
  device_reset(dev);
  device_loop(dev);
}