#include <stdio.h>
#include <string.h>

#include "device.h"
#include "decoder.h"
#include "misa.h"
#include "names.h"

extern misaf_entry_t misaf_list[];

int32_t b18int(uint32_t n, uint32_t w)
{
  n = (n << 2) + w;
  return (n & (1 << 17)) ? -((~n & 0x1ffff) + 1) : n;
}

misaf_entry_t* misa_find(const char* name)
{
  int n = 0;
  misaf_entry_t* ent;

  while (1)
    {
      ent = &misaf_list[n++];
      if (!ent->name)
	break;
      
      if (strcmp(ent->name, name) == 0)
	return ent;
    }

  return NULL;
}

misaf_entry_t* misa_get(uint32_t instruction)
{
  int n = 0;
  misaf_entry_t* ent;

  while (1)
    {
      ent = &misaf_list[n++];
      if (!ent->name)
	break;

      if (ent->id == instruction)
	return ent;
    }

  return NULL;
}

int misa_dump(device_t* dev, mips_instr_t* p)
{
  misaf_entry_t* ent;

  ent = misa_get(p->instruction);
  if (ent == NULL)
    {
//      device_event(dev, DEVICE_EVENT_NOT_IMPLEMENTED, &p->instruction);
      return -1;
    }  

  dev->run = 0;
  dev->dump = 1;
  ent->func(ent, dev, p);

#ifdef USE_DISAS
  char szTmp[64];
  sprintf(szTmp, "%-8s %s", ent->name, ent->info);
  device_event(dev, DEVICE_EVENT_ASM, szTmp);
#endif

  return 0;
}

int misa_call(device_t* dev, mips_instr_t* p)
{
  misaf_entry_t* ent;

  ent = misa_get(p->instruction);
  if (ent == NULL)
    {
      device_event(dev, DEVICE_EVENT_NOT_IMPLEMENTED, &p->instruction);      
      return -1;
    }

  dev->run = 1;
  dev->dump = 0;
  ent->func(ent, dev, p);
  
  return 0;
}
