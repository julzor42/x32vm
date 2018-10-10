#ifndef _MISA_H_
#define _MISA_H_
// MIPS ISA implementation

#define USE_DISAS
#define USE_DEBUG

struct misaf_entry_s;

typedef void (*MISAF)(struct misaf_entry_s* ent, device_t* dev, mips_instr_t* p);

typedef struct misaf_entry_s
{
  uint32_t    id;
  const char* name;
  MISAF       func;
  char	      info[256];
} misaf_entry_t;

#define MISA(a)			{ MIPS_##a, #a, misa_##a }
#define MISAF_BEGIN()		misaf_entry_t misaf_list[] = {
#define MISAF_END()		{ (uint32_t)0, (const char*)NULL, (MISAF)NULL }};
#define MISA_FUNC(n)		void misa_##n(misaf_entry_t* ent, device_t* dev, mips_instr_t* p)

#ifdef USE_DISAS
#define MISA_DESC(a, ...)	do { if (dev->dump) sprintf(ent->info, a, ##__VA_ARGS__); if (!dev->run) return; } while (0)
#else
#define MISA_DESC(a, ...) if (!dev->run) return
#endif

#ifdef USE_DEBUG
#define MISA_DBG(a, ...) do { char szTmp[64]; sprintf(szTmp, a, ##__VA_ARGS__); device_event(dev, DEVICE_EVENT_DEBUG, szTmp); } while (0)
#else
#define MISA_DBG(...)
#endif
  
void misa_default(misaf_entry_t* ent, device_t* dev, mips_instr_t* p);

misaf_entry_t*	misa_find	(const char* name);
misaf_entry_t*	misa_get	(uint32_t instruction);
int		misa_call	(device_t* dev, mips_instr_t* p);
int		misa_dump	(device_t* dev, mips_instr_t* p);

int32_t b18int(uint32_t n, uint32_t w);

#define _GPR(n) dev->GPR[n]

// Register id
#define _RS     p->rs
#define _RT     p->rt
#define _RD     p->rd

// Register values
#define _GPRS   _GPR(_RS)
#define _GPRT   _GPR(_RT)
#define _GPRD   _GPR(_RD)

// Register names
#define S_RS    S_REG(p->rs)
#define S_RT    S_REG(p->rt)
#define S_RD    S_REG(p->rd)

#define _ADDR(offset)  ADDR_REL(p->address, dev->PC + offset)
#define _ADDRREG(r, o) ADDR_REL(r, dev->PC + o)
#define _ADDRRAW(r)    _GPR(r) + p->immediate     

#define MISA_ADDR(offset)   uint32_t _dest = _ADDR(offset)
#define MISA_OFS18(offset)  uint32_t _dest = dev->PC + b18int(p->immediate, offset)
#define MISA_REGADDR(r)     uint32_t _dest = r
#define MISA_ADDRRAW()      uint32_t _dest = _ADDRRAW(_RS)

#define MISA_REGADDR_S() MISA_REGADDR(_GPRS)
#define MISA_REGADDR_T() MISA_REGADDR(_GPRT)
#define MISA_REGADDR_D() MISA_REGADDR(_GPRD)

//
// Actions
//
#define MISA_JUMP() dev->Jump = _dest
#define MISA_JUMPIF(cond) if ((cond)) MISA_JUMP()
#define MISA_JUMPIFL(cond) if ((cond)) MISA_JUMP(); else dev->skip = 1
#define MISA_LINK() _GPR(R_RA) = dev->PC + 4
#define MISA_WRITE(data, size) device_write(dev, _dest, (uint8_t*)data, size)
#define MISA_WRITE_WORD(data)  MISA_WRITE(data, 4)
#define MISA_WRITE_HALF(data)  MISA_WRITE(data, 2)
#define MISA_WRITE_BYTE(data)  MISA_WRITE(data, 1)

#define MISA_READ(data, size)  device_read(dev, _dest, (uint8_t*)data, size)
#define MISA_READ_WORD(data)   MISA_READ(data, 4)
#define MISA_READ_HALF(data)   MISA_READ(data, 2)
#define MISA_READ_BYTE(data)   MISA_READ(data, 1)

//
// Debug
//
#define MISA_DBG_R1(a)        MISA_DBG("%s = %#.8x", S_R##a, _GPR##a)
#define MISA_DBG_R2(a, b)     MISA_DBG("%s = %#.8x, %s = %#.8x", S_R##a, _GPR##a, S_R##b, _GPR##b)
#define MISA_DBG_R3(a, b, c)  MISA_DBG("%s = %#.8x, %s = %#.8x, %s = %#.8x", S_R##a, _GPR##a, S_R##b, _GPR##b, S_R##c, _GPR##c)
#define MISA_DBG_R2PC(a, b)   MISA_DBG("%s = %#.8x, %s = %#.8x, PC = %#.8x", S_R##a, _GPR##a, S_R##b, _GPR##b, dev->Jump)
#define MISA_DBG_CP0(r, d)    MISA_DBG("CP0_%s = %#.8x", S_CP0(r), dev->CP0R[r])

#define MISA_DBG_RS()   MISA_DBG_R1(S)
#define MISA_DBG_RT()   MISA_DBG_R1(T)
#define MISA_DBG_RD()   MISA_DBG_R1(D)
#define MISA_DBG_DST()  MISA_DBG_R3(D, S, T)
#define MISA_DBG_PC()   MISA_DBG("PC = %#.8x", dev->Jump)
#define MISA_DBG_PCRA() MISA_DBG("PC = %#.8x, RA = %#.8x", dev->Jump, _GPR(R_RA))
#define MISA_DBG_RTA()  MISA_DBG("%s = %#.8x, addr = %#.8x", S_RT, _GPRT, _dest)
#define MISA_DBG_RSA()  MISA_DBG("%s = %#.8x, addr = %#.8x", S_RS, _GPRS, _dest)
#define MISA_DBG_DT()   MISA_DBG_R2(D, T)
#define MISA_DBG_TS()   MISA_DBG_R2(T, S)

//
// Description
//
#define MISA_DESC_DST() MISA_DESC("%s, %s, %s", S_RD, S_RS, S_RT)
#define MISA_DESC_D()   MISA_DESC("%s", S_RD)
#define MISA_DESC_S()   MISA_DESC("%s", S_RS)
#define MISA_DESC_T()   MISA_DESC("%s", S_RT)
#define MISA_DESC_STI() MISA_DESC("%s, %s, %#.4x (%d)", S_RS, S_RT, p->immediate, p->immediate)
#define MISA_DESC_STIS() MISA_DESC("%s, %s, %#.4x (%d)", S_RS, S_RT, p->immediate, (int16_t)p->immediate)
#define MISA_DESC_SI()  MISA_DESC("%s, %#x", S_RS, p->immediate)
#define MISA_DESC_TI()  MISA_DESC("%s, %#x", S_RT, p->immediate)
#define MISA_DESC_DTSHIFT() MISA_DESC("%s, %s, %d", S_RD, S_RT, p->shift)
#define MISA_DESC_BTO()     MISA_DESC("%s, (%d)%s", S_RT, p->immediate, S_RS);

//#define MISA_DESC_DSA()  MISA_DESC("%s, %s, %#.8x", S_RD, S_RS, _dest)
#define MISA_DESC_STA()  MISA_DESC("%s, %s, %#.8x", S_RS, S_RT, _dest)
#define MISA_DESC_ADDR() MISA_DESC("%#.8x", _dest)

#define MISA_DESC_PSZ()  MISA_DESC("%s, %s, %d, %d", S_RT, S_RS, _GPRD, p->shift)

#define MISA_DESC_NIMP() MISA_DESC("!!!!!!!!!!!!!!!!!!! Not yet implemented!"); usleep(1000000)

#endif /* _MISA_H_ */
