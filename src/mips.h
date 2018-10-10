#ifndef _MIPS_H_
#define _MIPS_H_

#pragma pack(1)
typedef struct mips_word_s
{
  union {
    // generic, J type
    struct {
      int address:26;
      uint8_t opcode:6;
    };

    // R type
    struct {
      uint8_t funct:6;
      uint8_t shamt:5;
      uint8_t rd:5;
      uint8_t rt:5;
      uint8_t rs:5;
      int :6;
    };

    // I type
    struct {
      uint16_t immediate:16;
      int :6; // rt
      int :6; // rs
      int :6; // opcode
    };

    struct {
      uint32_t w:32;
    };
  };
} mips_word_t;

enum
  {
    MIPS_MODE_STANDARD,
    MIPS_MODE_COP0,
    MIPS_MODE_SPECIAL2,
    MIPS_MODE_SPECIAL3,
  } mips_instr_mode;

enum 
{
  R_ZERO,
  R_AT,
  R_V0, R_V1,
  R_A0, R_A1, R_A2, R_A3,
  R_T0, R_T1, R_T2, R_T3, R_T4, R_T5, R_T6, R_T7,
  R_S0, R_S1, R_S2, R_S3, R_S4, R_S5, R_S6, R_S7,
  R_T8, R_T9,
  R_K0, R_K1,
  R_GP,
  R_SP,
  R_FP,
  R_RA
} mips_gpr;

enum
  {
    MIPS_UNHANDLED,
    MIPS_NOP,
    MIPS_EHB,
    
    // Standard
    MIPS_J,
    MIPS_JAL,
    MIPS_BEQ,
    MIPS_BNE,
    MIPS_BLEZ,
    MIPS_BGTZ,
    MIPS_ADDI,
    MIPS_ADDIU,
    MIPS_SLTI,
    MIPS_SLTIU,
    MIPS_ANDI,
    MIPS_ORI,
    MIPS_XORI,
    MIPS_LUI,
    MIPS_BEQL,
    MIPS_BNEL,
    MIPS_BLEZL,
    MIPS_BGTZL,
    MIPS_LB,
    MIPS_LH,
    MIPS_LWL,
    MIPS_LW,
    MIPS_LBU,
    MIPS_LHU,
    MIPS_LWR,
    MIPS_SB,
    MIPS_SH,
    MIPS_SWL,
    MIPS_SW,
    MIPS_SWR,
    MIPS_CACHE,
    MIPS_LL,
    MIPS_LWC2,
    MIPS_PREF,
    MIPS_SC,
    MIPS_SWC2,
    
    // Special
    MIPS_SLL,
    MIPS_SRL,
    MIPS_SRA,
    MIPS_SLLV,
    MIPS_SRLV,
    MIPS_SRAV,
    MIPS_JR,
    MIPS_JALR,
    MIPS_MOVZ,
    MIPS_MOVN,
    MIPS_SYSCALL,
    MIPS_BREAK,
    MIPS_SYNC,
    MIPS_MFHI,
    MIPS_MTHI,
    MIPS_MFLO,
    MIPS_MTLO,
    MIPS_MULT,
    MIPS_MULTU,
    MIPS_DIV,
    MIPS_DIVU,
    MIPS_ADD,
    MIPS_ADDU,
    MIPS_SUB,
    MIPS_SUBU,
    MIPS_AND,
    MIPS_OR,
    MIPS_XOR,
    MIPS_NOR,
    MIPS_SLT,
    MIPS_SLTU,
    MIPS_TGE,
    MIPS_TGEU,
    MIPS_TLT,
    MIPS_TLTU,
    MIPS_TEQ,
    MIPS_TNE,

    // Special2
    MIPS_MADD,
    MIPS_MADDU,
    MIPS_MUL,
    MIPS_MSUB,
    MIPS_MSUBU,
    MIPS_CLZ,
    MIPS_CLO,
    MIPS_SDBBP,

    // Special3
    MIPS_EXT,
    MIPS_INS,
    MIPS_BSHFL,
    
    // COP0
    MIPS_MFC0,
    MIPS_MTC0,

    // REGIMM
    MIPS_BLTZ,
    MIPS_BGEZ,
    MIPS_BLTZL,
    MIPS_BGEZL,
    MIPS_TGEI,
    MIPS_TGEIU,
    MIPS_TLTI,
    MIPS_TLTIU,
    MIPS_TEQI,
    MIPS_TNEI,
    MIPS_BLTZAL,
    MIPS_BGEZAL,
    MIPS_BLTZALL,
    MIPS_BGEZALL,
    MIPS_SYNCI,    

  } mips_instruction;

#endif /* _MIPS_H_ */
