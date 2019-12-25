#define SPC_DEFINE_DBG_SYM(name) asm(#name ": \n")
#define SPC_USE_DBG_SYM(name)    extern void name(void)

inline int spc_check_int3(void *address) {
  return (*(volatile unsigned char *)address == 0xCC);
}
