/*
 * ctype_b.c
 *
 * This file has been added to compensate for a bug in
 * version 2.3.2 of the glibc library for RH8.
 */

 
#define attribute_hidden
#define CTYPE_EXTERN_INLINE /* Define real functions for accessors.  */
#include <ctype.h>






/*
#include <locale/localeinfo.h>
 
__libc_tsd_define (, CTYPE_B)
__libc_tsd_define (, CTYPE_TOLOWER)
__libc_tsd_define (, CTYPE_TOUPPER)
 
 
#include <shlib-compat.h>
*/
 
#define b(t,x,o) (((const t *) _nl_C_LC_CTYPE_##x) + o)
extern const char _nl_C_LC_CTYPE_class[] attribute_hidden;
extern const char _nl_C_LC_CTYPE_toupper[] attribute_hidden;
extern const char _nl_C_LC_CTYPE_tolower[] attribute_hidden;
const unsigned short int *__ctype_b = b (unsigned short int, class, 128);
const __int32_t *__ctype_tolower = b (__int32_t, tolower, 128);
const __int32_t *__ctype_toupper = b (__int32_t, toupper, 128);

#include <fenv.h>

void fegetexcept_(int * val){
*val=fegetexcept();
}

void fesetexcept_(int *val){
feclearexcept(*val);
feenableexcept(*val);
}

void feunsetexcept_(){
fedisableexcept(FE_ALL_EXCEPT);
}
#ifdef __LXPLUS6__
int __nss_next2 (void **ni, const char *fct_name,
                        const char *fct2_name, void **fctp, int status,
                        int all_values) {
return 1;
}
#endif
