#ifdef __DARWIN__
#ifdef __CINT__
#undef GNUC
#define _INT8_T
typedef char __signed;
typedef char int8_t;
typedef unsigned int uint32_t;
#endif
#endif
