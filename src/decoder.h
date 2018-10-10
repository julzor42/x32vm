#ifndef _DECODER_H_
#define _DECODER_H_

#include "mips.h"

typedef struct mips_instr_s
{
  uint32_t instruction;
  uint32_t address;
  uint16_t immediate;
  uint8_t  shift;
  uint8_t  rs;
  uint8_t  rt;
  uint8_t  rd;

  uint8_t  opcode;
  uint8_t  funct;
  uint8_t  mode;
} mips_instr_t;

void conv_word(mips_word_t* word, mips_instr_t* instr);

void decode_at(device_t* dev, mips_instr_t* instr, uint32_t address);

//#define ADDR_REL(a, h)   ((((a) << 2) & 0x003fffff) + ((h) & 0xffc0ffff))

#define ADDR_REL(a, h) (((a) << 2) & 0x3ffff) + ((h) & 0xffffc000)

#endif /* _DECODER_H_ */
