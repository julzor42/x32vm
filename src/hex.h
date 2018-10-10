#ifndef _HEX_H_
#define _HEX_H_

#include <stdint.h>

typedef struct hex_entry_s
{
  uint8_t      size;
  uint16_t     addr;
  uint8_t      type;
  uint8_t*     data;
  uint8_t      checksum;  
}              hex_entry_t;

typedef struct hex_context_s
{
  uint32_t     offset;
  uint8_t*     rom;
  uint8_t*     boot;

  uint32_t     rom_used;
  uint32_t     boot_used;
}              hex_context_t;

#define HEXENT_TYPE_DATA     0x00
#define HEXENT_TYPE_END      0x01
#define HEXENT_TYPE_EXTADDR  0x02
#define HEXENT_TYPE_SSAR     0x03
#define HEXENT_TYPE_EXTLINR  0x04
#define HEXENT_TYPE_SLAR     0x05

void hex_init(hex_context_t* context, uint8_t* rom, uint8_t* boot);
int hex_read(const char* filename, hex_context_t* context);

#endif /* _HEX_H_ */
