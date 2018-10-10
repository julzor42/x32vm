#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hex.h"
#include "memory.h"
#include "mips.h"
#include "decoder.h"
#include "names.h"
#include "device.h"
#include "misa.h"
#include "sfr.h"
#include "gui.h"

uint8_t g_rom[MEMORY_PHY_ROM_SIZE]    = {0x00};
uint8_t g_boot[MEMORY_PHY_BOOT_SIZE]  = {0x00};
uint8_t g_ram[MEMORY_PHY_RAM_SIZE]    = {0x00};
uint8_t g_sfr[MEMORY_PHY_SFR_SIZE]    = {0x00};
int sdelay = 0;

int st = 0;

void my_debugger(device_t *dev, uint32_t event, void *p)
{
  device_event_memory_t *mem;
  int IO = 1;

  switch (event)
  {
  case DEVICE_EVENT_NOT_IMPLEMENTED:
//    printf("ASM: NOT IMPLEMENTED %d at %#.8x\n", *(uint32_t *)p, dev->PC);
    break;

  case DEVICE_EVENT_ASM:
    if (st)
      printf("[%.6d] %.8x %s", dev->cycles, dev->PC, (char *)p);
    break;

  case DEVICE_EVENT_DEBUG:
    if (st)
    printf(" (%s)\n", (char *)p);
    break;

  case DEVICE_EVENT_JUMP:
//    printf("-----------------------------------------------------\n");
    break;

  case DEVICE_EVENT_SFRWRITE:
  case DEVICE_EVENT_SFRREAD:
    mem = (device_event_memory_t *)p;
//    printf("SFR I/O: %#.8x [%#.8x]\n", mem->address, mem->value);
    //getchar();
    break;

  case DEVICE_EVENT_RAMWRITE:
    mem = (device_event_memory_t *)p;
/*    if (mem->address > 0x1000)
      printf("[PUSH@%#.8x] -> %#.8x [%d]\n", mem->address, mem->value, mem->length);
    else
      printf("RAM WRITE @ %#.8x -> %#.8x [%d]\n", mem->address, mem->value, mem->length);
  //  getchar();*/
    break;

  case DEVICE_EVENT_RAMREAD:
    mem = (device_event_memory_t *)p;
//    printf("RAM READ @ %#.8x -> %#.8x [%d]\n", mem->address, mem->value, mem->length);
//    getchar();
    break;

  case DEVICE_EVENT_BADWRITE:
  case DEVICE_EVENT_BADREAD:
  //  printf("BAD ADDRESS! %#.8x\n", *(uint32_t *)p);
    exit(0);
    //usleep(10000000);
    break;
  }
}

void my_event(device_t *dev, uint32_t event, void *p)
{
  switch (event)
  {
  case DEVICE_EVENT_RESET:
    break;

  case DEVICE_EVENT_CYCLE:
    if (dev->PC > 0x1d000170)
    {
      st = 1;
    }

    if (!st)
    return;

    if (sdelay)
      usleep(sdelay);
   getchar();
    break;

  case DEVICE_EVENT_JUMP_PRE:
    if (dev->Jump2 == 0)
    {
      printf("Jump to 0!\n");
      getchar();
      //usleep(1000000);
      //dev->Jump2 = -1;
      //dev->skip = 1;
      sdelay = 200000;
    }
    break;
  }
}

int main(int ac, char **av)
{
  int display = 0;

  if (ac > 1)
    sdelay = atoi(av[1]) * 1000;

  if (ac > 2)
    display = atoi(av[2]);

  device_t p32;
  device_create(&p32);

  if (display)
    gui_init(&p32);

  device_register_event(&p32, my_event);
  device_register_event(&p32, my_debugger);

  device_set_boot(&p32, g_boot, sizeof(g_boot));
  device_set_rom(&p32, g_rom, sizeof(g_rom));
  device_set_ram(&p32, g_ram, sizeof(g_ram));
  device_set_sfr(&p32, g_sfr, sizeof(g_sfr));

  device_run(&p32, "firmware.hex");

  gui_free();

  return 0;
}
