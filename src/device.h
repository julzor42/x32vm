#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdint.h>

typedef struct device_s
{
  uint32_t PC;
  uint32_t Jump;
  uint32_t Jump2;
  uint32_t GPR[32];

  uint32_t CP0R[32];

  uint8_t *boot;
  uint8_t *rom;
  uint8_t *ram;
  uint8_t *sfr;

  uint32_t bootSize;
  uint32_t romSize;
  uint32_t ramSize;
  uint32_t sfrSize;

  uint8_t run;
  uint8_t dump;
  uint8_t halt;
  uint8_t refresh;
  uint8_t skip;

  uint32_t cycles;
} device_t;

enum
{
  DEVICE_EVENT_RESET = 0,

  // Program flow
  DEVICE_EVENT_CYCLE,
  DEVICE_EVENT_JUMP_PRE,
  DEVICE_EVENT_JUMP,

  // Memory
  DEVICE_EVENT_SFRWRITE,
  DEVICE_EVENT_SFRREAD,
  DEVICE_EVENT_RAMWRITE,
  DEVICE_EVENT_RAMREAD,

  // Errors
  DEVICE_EVENT_BADWRITE,
  DEVICE_EVENT_BADREAD,

  // Debug
  DEVICE_EVENT_ASM,
  DEVICE_EVENT_DEBUG,
  DEVICE_EVENT_NOT_IMPLEMENTED,
};

typedef struct device_event_memory_s
{
  uint32_t address;
  uint32_t value;
  uint32_t length;
} device_event_memory_t;

typedef void (*DEVICE_FUNC)(device_t*, uint32_t, void*);

typedef struct device_callback_s
{
  DEVICE_FUNC func;
  struct device_callback_s* next;
} device_callback_t;

void device_program(device_t *dev, const char *hex);
void device_loop(device_t *dev);

void device_write(device_t *dev, uint32_t addr, uint8_t *data, uint32_t size);
void device_read(device_t *dev, uint32_t addr, uint8_t *data, uint32_t size);
void device_write_sfr(device_t *dev, uint32_t addr, uint32_t data);

#define device_write_word(dev, addr, data) device_write(dev, addr, (uint8_t *)data, 4)
#define device_write_half(dev, addr, data) device_write(dev, addr, (uint8_t *)data, 2)
#define device_write_byte(dev, addr, data) device_write(dev, addr, (uint8_t *)data, 1)

// User functions
void device_create(device_t *dev);
void device_set_boot(device_t *dev, uint8_t *boot, uint32_t size);
void device_set_rom(device_t *dev, uint8_t *rom, uint32_t size);
void device_set_ram(device_t *dev, uint8_t *ram, uint32_t size);
void device_set_sfr(device_t *dev, uint8_t *sfr, uint32_t size);
void device_reset(device_t *dev);
void device_run(device_t* dev, const char* s);
void device_register_event(device_t* dev, DEVICE_FUNC func);

#endif /* _DEVICE_H_ */
