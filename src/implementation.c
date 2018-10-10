#include <stdio.h>
#include <stdint.h>

#include "names.h"
#include "device.h"
#include "decoder.h"
#include "mips.h"
#include "misa.h"

/*
  Register Ids

  _RS: Source
  _RT: Target
  _RD: 

  Register names
  S_RS
  S_RT
  S_RD

  Register vales
  _GPRS
  _GPRT
  _GPRD

  Address, relative to PC + offset
  _ADDR(offset)

  Update PC at the end of the cycle
  _JUMP(address)
 */

MISA_FUNC(default)
{
  printf("ERROR!\n");
  usleep(1000000);
}

//
// Standard instructions
//

// No Operation
MISA_FUNC(NOP)
{
}

// Execution Hazard Barrier
MISA_FUNC(EHB)
{
  //MISA_DESC_NIMP();
  MISA_DESC("Not implemented");
  MISA_DBG("Wait");
}

// Jump
MISA_FUNC(J)
{
  MISA_ADDR(0);
  MISA_DESC_ADDR();
  MISA_JUMP();
  MISA_DBG_PC();
}

// Jump And Link
MISA_FUNC(JAL)
{
  MISA_ADDR(0);
  MISA_DESC_ADDR();
  MISA_JUMP();
  MISA_LINK();
  MISA_DBG_PCRA();
}

// Jump Register
MISA_FUNC(JR)
{
  MISA_DESC_S();
  MISA_REGADDR_S();
  MISA_JUMP();
  MISA_DBG_PC();
}

// Jump And Link Register
MISA_FUNC(JALR)
{
  MISA_DESC_S();
  MISA_REGADDR_S();
  MISA_JUMP();
  MISA_LINK();
  MISA_DBG_PCRA();
}

// Branch on EQual
MISA_FUNC(BEQ)
{
  MISA_OFS18(4);
  MISA_DESC_STA();
  MISA_JUMPIF(_GPRS == _GPRT);
  MISA_DBG_R2PC(S, T);
}

// Branch on Not eQual
MISA_FUNC(BNE)
{
  MISA_OFS18(4);
  MISA_DESC_STA();
  MISA_JUMPIF(_GPRS != _GPRT);
  MISA_DBG_R2PC(S, T);
}

// Branch on Equal Likely
MISA_FUNC(BEQL)
{
  MISA_OFS18(4);
  MISA_DESC_STA();
  MISA_JUMPIFL(_GPRS == _GPRT);
  MISA_DBG_R2PC(S, T);
}

// Branch on Not Equal Likely
MISA_FUNC(BNEL)
{
  MISA_OFS18(4);
  MISA_DESC_STA();
  MISA_JUMPIFL(_GPRS != _GPRT);
  MISA_DBG_R2PC(S, T);
}

// Branch on Less Than or Equal to Zero Likely
MISA_FUNC(BLEZL)
{
  MISA_DESC_NIMP();
}

// Branch on Greater Than Zero Likely
MISA_FUNC(BGTZL)
{
  MISA_DESC_NIMP();
}

// Branch on Less than or Equal to Zero
MISA_FUNC(BLEZ)
{
  MISA_OFS18(4);
  MISA_DESC_SI();
  MISA_JUMPIF(_GPRS <= 0);
  MISA_DBG_PC();
}

// Branch on Greater Than Zero
MISA_FUNC(BGTZ)
{
  MISA_OFS18(4);
  MISA_DESC_SI();
  MISA_JUMPIF(_GPRS > 0);
  MISA_DBG_PC();
}

// Add Immediate?
MISA_FUNC(ADDI)
{
  // TODO: signal overflow?
  MISA_DESC_STI();
  _GPRT = _GPRS + p->immediate;
  MISA_DBG_RT();
}

// Add Immediate Unsigned
MISA_FUNC(ADDIU)
{
  MISA_DESC_STIS();
  MISA_DBG_RT();
  _GPRT = _GPRS + (int16_t)p->immediate;
  MISA_DBG_RT();
}

// Set on Less Than Immediate
MISA_FUNC(SLTI)
{
  MISA_DESC_STI();
  _GPRT = _GPRS < p->immediate ? 1 : 0;
  MISA_DBG_TS();
}

// Set on Less Than Immediate Unsigned
MISA_FUNC(SLTIU)
{
  MISA_DESC_STI();
  _GPRT = _GPRS < p->immediate ? 1 : 0;
  MISA_DBG_TS();
}

// Set on Less Than
MISA_FUNC(SLT)
{
  MISA_DESC_DST();
  _GPRD = _GPRS < _GPRT;
  MISA_DBG_DST();
}

// Set on Less Than Unsigned
MISA_FUNC(SLTU)
{
  MISA_DESC_DST();
  _GPRD = _GPRS < _GPRT;
  MISA_DBG_DST();
}

// Logical And Immediate
MISA_FUNC(ANDI)
{
  MISA_DESC_STI();
  _GPRT = _GPRS & p->immediate;
  MISA_DBG_RT();
}

// Logical Or Immediate
MISA_FUNC(ORI)
{
  MISA_DESC_STI();
  _GPRT = _GPRS | p->immediate;
  MISA_DBG_RT();
}

// Logical eXclusive Or Immediate
MISA_FUNC(XORI)
{
  MISA_DESC_STI();
  _GPRT = _GPRS ^ p->immediate;
  MISA_DBG_RT();
}

// Load Upper Immediate
MISA_FUNC(LUI)
{
  MISA_DESC_TI();
  _GPRT = p->immediate << 16;
  MISA_DBG_RT();
}

// Load Word
MISA_FUNC(LW)
{
  if (p->immediate == 42)
    exit(0);

  MISA_ADDRRAW();
  MISA_DESC_BTO();
  MISA_READ_WORD(&_GPRT);
  MISA_DBG_RTA();
}

// Load Halfword
MISA_FUNC(LH)
{
  MISA_DESC_NIMP();
}

// Load Byte
MISA_FUNC(LB)
{
  MISA_ADDRRAW();
  MISA_DESC_BTO();
  _GPRT = 0;
  MISA_READ_BYTE(&_GPRT);
  MISA_DBG_RTA();
}

// Unsigned Load Byte
MISA_FUNC(LBU)
{
  MISA_ADDRRAW();
  MISA_DESC_BTO();
  _GPRT = 0;
  MISA_READ_BYTE(&_GPRT);

  _GPRT = (int8_t)_GPRT & 0xff;

  MISA_DBG_RTA();
}

// Load Word Left
MISA_FUNC(LWL)
{
  MISA_DESC_NIMP();
}

// Unsigned Load Halfword
MISA_FUNC(LHU)
{
  MISA_ADDRRAW();
  MISA_DESC_BTO();
  _GPRT = 0;
  MISA_READ_HALF(&_GPRT);
  MISA_DBG_RTA();
}

// Load Word Right
MISA_FUNC(LWR)
{
  MISA_DESC_NIMP();
}

// Store Word
MISA_FUNC(SW)
{
  MISA_ADDRRAW();
  MISA_DESC_BTO();
  MISA_WRITE_WORD(&_GPRT);
  MISA_DBG_RTA();
}

// Store Halfword
MISA_FUNC(SH)
{
  MISA_ADDRRAW();
  MISA_DESC_BTO();
  MISA_WRITE_HALF(&_GPRT);
  MISA_DBG_RTA();
}

// Store Byte
MISA_FUNC(SB)
{
  MISA_ADDRRAW();
  MISA_DESC_BTO();
  _GPRT = 0;
  MISA_WRITE_BYTE(&_GPRT);
  MISA_DBG_RTA();
}

// Store Word Left
MISA_FUNC(SWL)
{
  MISA_DESC_NIMP();
}

// Store Word Right
MISA_FUNC(SWR)
{
  MISA_DESC_NIMP();
}

// Cache Operation
MISA_FUNC(CACHE)
{
  MISA_DESC_NIMP();
}

// Load Linked Word
MISA_FUNC(LL)
{
  MISA_DESC_NIMP();
}

// Load Word To Coprocessor 2
MISA_FUNC(LWC2)
{
  MISA_DESC_NIMP();
}

// Prefetch
MISA_FUNC(PREF)
{
  MISA_DESC_NIMP();
}

// Store Conditional Word
MISA_FUNC(SC)
{
  MISA_DESC_NIMP();
}

// Store Word from Coprocessor 2
MISA_FUNC(SWC2)
{
  MISA_DESC_NIMP();
}

//
// Special
//

// Shift Left Logical
MISA_FUNC(SLL)
{
  MISA_DESC_DTSHIFT();

  _GPRD = _GPRT << p->shift;

  MISA_DBG_RD();
}

// Shift Right Logical
MISA_FUNC(SRL)
{
  MISA_DESC_DTSHIFT();

  _GPRD = _GPRT >> p->shift;

  MISA_DBG_RD();
}

// Shift Right Arithmetic
MISA_FUNC(SRA)
{
  MISA_DESC_NIMP();
}

// Shift Left Logical Variable
MISA_FUNC(SLLV)
{
  MISA_DESC_NIMP();
}

// Shift Right Logical Variable
MISA_FUNC(SRLV)
{
  MISA_DESC_NIMP();
}

// Shift Right Arithmetic Variable
MISA_FUNC(SRAV)
{
  MISA_DESC_NIMP();
}

// Move on Zero
MISA_FUNC(MOVZ)
{
  MISA_DESC_NIMP();
}

// Move on Not zero
MISA_FUNC(MOVN)
{
  MISA_DESC_NIMP();
}

// System call
MISA_FUNC(SYSCALL)
{
  MISA_DESC_NIMP();
}

// Breakpoint
MISA_FUNC(BREAK)
{
  MISA_DESC_NIMP();
}

// Synchronize
MISA_FUNC(SYNC)
{
  MISA_DESC_NIMP();
}

// Move From HI
MISA_FUNC(MFHI)
{
  MISA_DESC_NIMP();
}

// Move To HI
MISA_FUNC(MTHI)
{
  MISA_DESC_NIMP();
}

// Move From LO
MISA_FUNC(MFLO)
{
  MISA_DESC_NIMP();
}

// Move To LO
MISA_FUNC(MTLO)
{
  MISA_DESC_NIMP();
}

// Multiply
MISA_FUNC(MULT)
{
  MISA_DESC_NIMP();
}

// Multiply Unsigned
MISA_FUNC(MULTU)
{
  MISA_DESC_NIMP();
}

// Divide
MISA_FUNC(DIV)
{
  MISA_DESC_NIMP();
}

// Divide Unsigned
MISA_FUNC(DIVU)
{
  MISA_DESC_NIMP();
}

// Add
MISA_FUNC(ADD)
{
  MISA_DESC_DST();

  // TODO: signed?
  _GPRD = _GPRS + _GPRT;

  MISA_DBG_DST();
}

// Add Unsigned
MISA_FUNC(ADDU)
{
  MISA_DESC_DST();

  // TODO: unsigned?
  _GPRD = _GPRS + _GPRT;

  MISA_DBG_DST();
}

// Substract
MISA_FUNC(SUB)
{
  MISA_DESC_NIMP();
}

// Substract Unsigned
MISA_FUNC(SUBU)
{
  MISA_DESC_DST();

  _GPRD = _GPRS - _GPRT;

  MISA_DBG_DST();
}

// Logical And
MISA_FUNC(AND)
{
  MISA_DESC_DST();

  _GPRD = _GPRS & _GPRT;

  MISA_DBG_DST();
}

// Logical Or
MISA_FUNC(OR)
{
  MISA_DESC_DST();

  _GPRD = _GPRS | _GPRT;

  MISA_DBG_DST();
}

// Logical eXclusive Or
MISA_FUNC(XOR)
{
  MISA_DESC_DST();

  _GPRD = _GPRS ^ _GPRT;

  MISA_DBG_DST();
}

// Logical Not Or
MISA_FUNC(NOR)
{
  MISA_DESC_DST();

  _GPRD = ~(_GPRS | _GPRT);

  MISA_DBG_DST();
}

// Trap if Greater Than or Equal
MISA_FUNC(TGE)
{
  MISA_DESC_NIMP();
}

// Trap if Greater Than or Equal Unsigned
MISA_FUNC(TGEU)
{
  MISA_DESC_NIMP();
}

// Trap if Less Than
MISA_FUNC(TLT)
{
  MISA_DESC_NIMP();
}

// Trap if Less Than Unsigned
MISA_FUNC(TLTU)
{
  MISA_DESC_NIMP();
}

// Trap if Equal
MISA_FUNC(TEQ)
{
  MISA_DESC_NIMP();
}

// Trap if Not Equal
MISA_FUNC(TNE)
{
  MISA_DESC_NIMP();
}

//
// Special2
//

// Multiply-Add
MISA_FUNC(MADD)
{
  MISA_DESC_NIMP();
}

// Multiply-Add Unsigned
MISA_FUNC(MADDU)
{
  MISA_DESC_NIMP();
}

// Multiply
MISA_FUNC(MUL)
{
  MISA_DESC_DST();
  _GPRD = _GPRS * _GPRT;
  MISA_DBG_DST();
}

// Multiply-Substract
MISA_FUNC(MSUB)
{
  MISA_DESC_NIMP();
}

// Multiply-Substract Unsigned
MISA_FUNC(MSUBU)
{
  MISA_DESC_NIMP();
}

// Count Leading Zeroes
MISA_FUNC(CLZ)
{
  MISA_DESC_NIMP();
}

// Count Leading Ones
MISA_FUNC(CLO)
{
  MISA_DESC_NIMP();
}

// Software Debug Breakpoint
MISA_FUNC(SDBBP)
{
  MISA_DESC_NIMP();
}

//
// Special3
//

// Extract bit field
MISA_FUNC(EXT)
{
  MISA_DESC_PSZ();

  _GPRT = _GPRS >> p->shift;

  // Remove anything before size (RD)
  _GPRT <<= 32 - _RD;
  _GPRT >>= 32 - _RD;

  MISA_DBG_R2(T, S);
}

// Insert bit field
MISA_FUNC(INS)
{
  uint32_t t1, t2;

  MISA_DESC_PSZ();

  t1 = 0xffffffff;
  t2 = 0xffffffff;

  // Create mask
  t1 >>= p->shift;
  t1 <<= p->shift;

  t1 <<= 32 - (p->shift + _RD);
  t1 >>= 32 - (p->shift + _RD);

  // Clean area
  _GPRT &= ~t1;

  // Insert bits
  _GPRT |= _GPRS << _RD;

  MISA_DBG_R2(T, S);
}

// ???
MISA_FUNC(BSHFL)
{
  MISA_DESC_NIMP();
}

//
// Coprocessor 0
//

// Move From Coprocessor 0
MISA_FUNC(MFC0)
{
  MISA_DESC("%s, CP0_%s (%d)", S_RT, S_CP0(_GPRD), p->funct);

  _GPRT = dev->CP0R[_GPRD];

  MISA_DBG_RT();
}

// Move To Coprocessor 0
MISA_FUNC(MTC0)
{
  MISA_DESC("%s, CP0_%s (%d)", S_RT, S_CP0(_GPRD), p->funct);

  dev->CP0R[_GPRD] = _GPRT;

  MISA_DBG_CP0(_GPRD, p->funct);
}

//
// RegImm
//

MISA_FUNC(BLTZ)
{
  MISA_OFS18(4);
  MISA_DESC_SI();
  MISA_JUMPIF(_GPRS < 0);
  MISA_DBG_RSA();
}

MISA_FUNC(BGEZ)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(BLTZL)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(BGEZL)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(TGEI)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(TGEIU)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(TLTI)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(TLTIU)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(TEQI)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(TNEI)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(BLTZAL)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(BGEZAL)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(BLTZALL)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(BGEZALL)
{
  MISA_DESC_NIMP();
}

MISA_FUNC(SYNCI)
{
  MISA_DESC_NIMP();
}

//
// Instruction table definition
//

MISAF_BEGIN()
// Standard
MISA(NOP)
,
    MISA(EHB),
    MISA(J),
    MISA(JAL),
    MISA(BEQ),
    MISA(BNE),
    MISA(BLEZ),
    MISA(BGTZ),
    MISA(ADDI),
    MISA(ADDIU),
    MISA(SLTI),
    MISA(SLTIU),
    MISA(ANDI),
    MISA(ORI),
    MISA(XORI),
    MISA(LUI),
    MISA(BEQL),
    MISA(BNEL),
    MISA(BLEZL),
    MISA(BGTZL),
    MISA(LB),
    MISA(LH),
    MISA(LWL),
    MISA(LW),
    MISA(LBU),
    MISA(LHU),
    MISA(LWR),
    MISA(SB),
    MISA(SH),
    MISA(SWL),
    MISA(SW),
    MISA(SWR),
    MISA(CACHE),
    MISA(LL),
    MISA(LWC2),
    MISA(PREF),
    MISA(SC),
    MISA(SWC2),

    // Special
    MISA(SLL),
    MISA(SRL),
    MISA(SRA),
    MISA(SLLV),
    MISA(SRLV),
    MISA(SRAV),
    MISA(JR),
    MISA(JALR),
    MISA(MOVZ),
    MISA(MOVN),
    MISA(SYSCALL),
    MISA(BREAK),
    MISA(SYNC),
    MISA(MFHI),
    MISA(MTHI),
    MISA(MFLO),
    MISA(MTLO),
    MISA(MULT),
    MISA(MULTU),
    MISA(DIV),
    MISA(DIVU),
    MISA(ADD),
    MISA(ADDU),
    MISA(SUB),
    MISA(SUBU),
    MISA(AND),
    MISA(OR),
    MISA(XOR),
    MISA(NOR),
    MISA(SLT),
    MISA(SLTU),
    MISA(TGE),
    MISA(TGEU),
    MISA(TLT),
    MISA(TLTU),
    MISA(TEQ),
    MISA(TNE),

    // Special2
    MISA(MADD),
    MISA(MADDU),
    MISA(MUL),
    MISA(MSUB),
    MISA(MSUBU),
    MISA(CLZ),
    MISA(CLO),
    MISA(SDBBP),

    // Special3
    MISA(EXT),
    MISA(INS),
    MISA(BSHFL),

    // COP0
    MISA(MFC0),
    MISA(MTC0),

    // REGIMM
    MISA(BLTZ),
    MISA(BGEZ),
    MISA(BLTZL),
    MISA(BGEZL),
    MISA(TGEI),
    MISA(TGEIU),
    MISA(TLTI),
    MISA(TLTIU),
    MISA(TEQI),
    MISA(TNEI),
    MISA(BLTZAL),
    MISA(BGEZAL),
    MISA(BLTZALL),
    MISA(BGEZALL),
    MISA(SYNCI),

    MISAF_END()
