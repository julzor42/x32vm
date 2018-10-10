#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hex.h"
#include "memory.h"

static uint32_t parsenum(const char** s, uint32_t size, uint32_t base)
{
  char tmp[8];

  memset(tmp, 0, sizeof(tmp));
  strncpy(tmp, *s, size);
  
  *s += size;
  
  return strtol(tmp, NULL, base);
}

static int hex_parse(const char* s, hex_entry_t* p)
{
  uint8_t n;
  uint8_t* ptr;
  
  memset(p, 0, sizeof(hex_entry_t));
  
  if (*s != ':')
    return 0;

  // skip :
  s++;

  p->size = parsenum(&s, 2, 16);
  p->addr = parsenum(&s, 4, 16);
  p->type = parsenum(&s, 2, 16);

  p->data = (uint8_t*)malloc(p->size);
  ptr = p->data;
  for (n = 0; n < p->size; n++, ptr++)
    {
      *ptr = parsenum(&s, 2, 16);
    }

  p->checksum = parsenum(&s, 2, 16);
  return 1;
}

static void hex_free(hex_entry_t* p)
{
  if (p->data)
    free(p->data);
}

static void hex_dump(hex_entry_t* p, hex_context_t* cnt)
{
  int i;
  
  printf("[%.8x] ", cnt->offset + p->addr);
  for (i = 0; i < p->size; i++)
    printf("%.2x ", p->data[i]);
  printf("\n");
}

static void hex_write_rom(hex_entry_t *p, hex_context_t* cnt)
{
  uint32_t f = MEMORY_FROMROM(p->addr + cnt->offset);

  if ((f + p->size) > cnt->rom_used)
    cnt->rom_used = f + p->size;
  
  memcpy(cnt->rom + f, p->data, p->size);
}

static void hex_write_boot(hex_entry_t* p, hex_context_t* cnt)
{
  uint32_t f = MEMORY_FROMBOOT(p->addr + cnt->offset);

  if ((f + p->size) > cnt->boot_used)
    cnt->boot_used = f + p->size;

  memcpy(cnt->boot + f, p->data, p->size);      
}

static void hex_process(hex_entry_t* p, hex_context_t* cnt)
{
  uint32_t addr;
  
  switch (p->type)
    {
    case HEXENT_TYPE_END:
      break;
      
    case HEXENT_TYPE_EXTLINR:
      cnt->offset = p->data[0] << 24 | p->data[1] << 16;
      break;

    case HEXENT_TYPE_DATA:
      addr = cnt->offset + p->addr;
      if (MEMORY_ISBOOT(addr))	   hex_write_boot(p, cnt);
      else if (MEMORY_ISROM(addr)) hex_write_rom(p, cnt);
      else printf("Where is it? -> %.8x\n", addr);
      //hex_dump(p, cnt);
      break;

    default:
      printf("Unhandled hex record type: %d\n", p->type);
    }
}

void hex_init(hex_context_t* context, uint8_t* rom, uint8_t* boot)
{
  memset(context, 0, sizeof(hex_context_t));

  context->rom = rom;
  context->boot = boot;
}

int hex_read(const char* filename, hex_context_t* context)
{
  FILE*        fp;
  hex_entry_t  ent;
  char         line[256];

  memset(line, 0, sizeof(line));
  
  fp = fopen(filename, "r");
  if (fp == NULL)
    return -1;

  while (fgets(line, sizeof(line) - 1, fp))
    {
      if (hex_parse(line, &ent))
	{
	  hex_process(&ent, context);
	  hex_free(&ent);
	}      
    }

  fclose(fp);

  return 0;
}

