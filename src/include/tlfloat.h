#ifndef __TLFLOAT_H_INCLUDED__
#define __TLFLOAT_H_INCLUDED__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__x86_64__) && defined(__GNUC__)
  typedef __float128 tlfloat_quad;
#elif defined(__aarch64__) && defined(__GNUC__)
  typedef long double tlfloat_quad;
#else
  typedef struct { uint64_t e[2]; } tlfloat_quad;
#endif

  typedef struct { uint64_t e[4]; } tlfloat_octuple;

  //

  int tlfloat_vfprintf(FILE *fp, const char *fmt, va_list ap);
  int tlfloat_vprintf(const char *fmt, va_list ap);
  int tlfloat_fprintf(FILE *fp, const char *fmt, ...);
  int tlfloat_printf(const char *fmt, ...);
  int tlfloat_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
  int tlfloat_snprintf(char *str, size_t size, const char *fmt, ...);

  float tlfloat_strtof(const char *, const char **);
  double tlfloat_strtod(const char *, const char **);
  tlfloat_quad tlfloat_strtoq(const char *, const char **);
  tlfloat_octuple tlfloat_strtoo(const char *, const char **);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // #ifndef __TLFLOAT_H_INCLUDED__
