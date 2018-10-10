#ifndef _NAMES_H_
#define _NAMES_H_

extern const char* g_cp0[];
extern const char* g_regs[];
extern const char* g_instr[];

#define S_INSTR(a) g_instr[a]
#define S_REG(a)   g_regs[a]
#define S_CP0(a)   g_cp0[a]

#endif /* _NAMES_H_ */