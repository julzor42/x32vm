#include <string.h> 

#include "memory.h"
#include "decoder.h"
#include "names.h"
#include "device.h"

#define _INS(n, i) case n: instr->instruction = MIPS_##i; break
#define _INX(n, i) case n: instr->instruction = MIPS_##i;

void conv_cop0(mips_word_t* word, mips_instr_t* instr)
{
  switch (word->rs)
    {
      _INS(000, MFC0);
      _INS(004, MTC0);
    }
}

void conv_special(mips_word_t* word, mips_instr_t* instr)
{
  switch (word->funct)
    {
      _INS(000, SLL);
      _INS(002, SRL);
      _INS(003, SRA);
      _INS(004, SLLV);
      _INS(006, SRLV);
      _INS(007, SRAV);
      _INS(010, JR);
      _INS(011, JALR);
      _INS(012, MOVZ);
      _INS(013, MOVN);
      _INS(014, SYSCALL);
      _INS(015, BREAK);
      _INS(017, SYNC);
      _INS(020, MFHI);
      _INS(021, MTHI);
      _INS(022, MFLO);
      _INS(023, MTLO);
      _INS(030, MULT);
      _INS(031, MULTU);
      _INS(032, DIV);
      _INS(033, DIVU);
      _INS(040, ADD);
      _INS(041, ADDU);
      _INS(042, SUB);
      _INS(043, SUBU);
      _INS(044, AND);
      _INS(045, OR);
      _INS(046, XOR);
      _INS(047, NOR);
      _INS(052, SLT);
      _INS(053, SLTU);
      _INS(060, TGE);
      _INS(061, TGEU);
      _INS(062, TLT);
      _INS(063, TLTU);
      _INS(064, TEQ);
      _INS(066, TNE);
    }
}

void conv_special2(mips_word_t* word, mips_instr_t* instr)
{
  switch (word->funct)
    {
      _INS(000, MADD);
      _INS(001, MADDU);
      _INS(002, MUL);
      _INS(004, MSUB);
      _INS(005, MSUBU);
      _INS(040, CLZ);
      _INS(041, CLO);
      _INS(077, SDBBP);
    }
}

void conv_special3(mips_word_t* word, mips_instr_t* instr)
{
 switch (word->funct)
   {
     _INS(000, EXT);
     _INS(004, INS);
     _INS(040, BSHFL);
   }
}

void conv_regimm(mips_word_t* word, mips_instr_t* instr)
{
  switch (word->rt)
  {
    _INS(000, BLTZ);
    _INS(001, BGEZ);
    _INS(002, BLTZL);
    _INS(003, BGEZL);
    _INS(010, TGEI);
    _INS(011, TGEIU);
    _INS(012, TLTI);
    _INS(013, TLTIU);
    _INS(014, TEQI);
    _INS(016, TNEI);
    _INS(020, BLTZAL);
    _INS(021, BGEZAL);
    _INS(022, BLTZALL);
    _INS(023, BGEZALL);
    _INS(037, SYNCI);
  }
}

void conv_standard(mips_word_t* word, mips_instr_t* instr)
{
  switch (word->opcode)
    {
      _INS(002, J);
      _INS(003, JAL);
      _INS(004, BEQ);
      _INS(005, BNE);
      _INS(006, BLEZ);
      _INS(007, BGTZ);
      _INS(010, ADDI);
      _INS(011, ADDIU);
      _INS(012, SLTI);
      _INS(013, SLTIU);
      _INS(014, ANDI);
      _INS(015, ORI);
      _INS(016, XORI);
      _INS(017, LUI);
      _INS(024, BEQL);
      _INS(025, BNEL);
      _INS(026, BLEZL);
      _INS(027, BGTZL);
      _INS(040, LB);
      _INS(041, LH);
      _INS(042, LWL);
      _INS(043, LW);
      _INS(044, LBU);
      _INS(045, LHU);
      _INS(046, LWR);
      _INS(050, SB);
      _INS(051, SH);
      _INS(052, SWL);
      _INS(053, SW);
      _INS(056, SWR);
      _INS(057, CACHE);
      _INS(060, LL);
      _INS(062, LWC2);
      _INS(063, PREF);
      _INS(070, SC);
      _INS(072, SWC2);   
    }
}

void conv_word(mips_word_t* word, mips_instr_t* instr)
{
  memset(instr, 0, sizeof(mips_instr_t));

  // Special cases
  switch (word->w)
    {
    case 0x00000000: instr->instruction = MIPS_NOP; return;
    case 0x000000c0: instr->instruction = MIPS_EHB; return;
    }

  instr->rs        = word->rs        & 0x1f;
  instr->rt        = word->rt        & 0x1f;
  instr->rd        = word->rd        & 0x1f;
  instr->address   = word->address   & 0x03ffffff;
  instr->immediate = word->immediate & 0xffff;
  instr->shift     = word->shamt     & 0x1f;
  instr->opcode    = word->opcode    & 0x3f;
  instr->funct     = word->funct     & 0x3f;
  
  switch (word->opcode)
    {
    case 000: conv_special (word, instr); break;
    case 001: conv_regimm  (word, instr); break;
    case 020: conv_cop0    (word, instr); break;
    case 034: conv_special2(word, instr); break;
    case 037: conv_special3(word, instr); break;
    default:  conv_standard(word, instr); break;
    }
}

void decode_at(device_t* dev, mips_instr_t* instr, uint32_t address)
{
  mips_word_t   word;
 
  word.w = word_at(dev, address);
  
  conv_word(&word, instr);
}
