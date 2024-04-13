#ifndef __TLFLOAT_H_INCLUDED__
#define __TLFLOAT_H_INCLUDED__

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct { uint64_t e[1 << 0]; } tlfloat_bigint6;
  typedef struct { uint64_t e[1 << 1]; } tlfloat_bigint7;
  typedef struct { uint64_t e[1 << 2]; } tlfloat_bigint8;
  typedef struct { uint64_t e[1 << 3]; } tlfloat_bigint9;
  typedef struct { uint64_t e[1 << 4]; } tlfloat_bigint10;

#if defined(__LDBL_MANT_DIG__)
#if __LDBL_MANT_DIG__ == 113
#define TLFLOAT_LONGDOUBLE_IS_FLOAT128
#endif
#endif

#if defined(__x86_64__) && defined(__GNUC__)
  typedef __float128 tlfloat_quad;
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)
  typedef long double tlfloat_quad;
#else
  /** This type is for handling quadruple-precision IEEE floating-point numbers in C.
   * This is an alias for __float128 if that type is defined by the compiler.
   * If long double is IEEE float 128, this type is an alias for long double. */
  typedef struct { uint64_t e[2]; } tlfloat_quad;
#endif

  /** This type is for handling octuple-precision IEEE floating-point numbers in C.
   * The data size and data structure of this type are the same as an octuple-precision floating-point number. */
  typedef struct { uint64_t e[4]; } tlfloat_octuple;

  //

  /** The tlfloat_printf family of functions has the functionality to produce output of numbers of the types defined in TLFloat library, 
   * in addition to the functionality of the standard printf family of functions.
   * To output a number of type defined in TLFloat library,
   * use _m as the length modifier where m is a decimal integer represents the data size of the corresponding argument in bits.
   * For example, use '\%_256g' to output an octuple-precision FP number, and '\%_128d' to output a 128-bit signed integer.
   * Q modifier can also be used to output a quadruple-precision FP number. */
  int tlfloat_printf(const char *fmt, ...);
  int tlfloat_fprintf(FILE *fp, const char *fmt, ...);
  int tlfloat_snprintf(char *str, size_t size, const char *fmt, ...);
  int tlfloat_vprintf(const char *fmt, va_list ap);
  int tlfloat_vfprintf(FILE *fp, const char *fmt, va_list ap);
  int tlfloat_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

  float tlfloat_strtof(const char *, const char **);
  double tlfloat_strtod(const char *, const char **);
  tlfloat_quad tlfloat_strtoq(const char *, const char **);
  tlfloat_octuple tlfloat_strtoo(const char *, const char **);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sinf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sin(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_sinq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_sino(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_cosf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cos(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_cosq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_coso(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tan(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_tanq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_tano(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_asinf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_asin(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_asinq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_asino(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_acosf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_acos(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_acosq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_acoso(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atan(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_atanq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_atano(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atan2f(const float y, const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atan2(const double y, const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_atan2q(const tlfloat_quad y, const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_atan2o(const tlfloat_octuple y, const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_expf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_expq(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_expo(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_expm1f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_expm1(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_expm1q(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_expm1o(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_exp2f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp2(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_exp2q(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_exp2o(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_exp10f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp10(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_exp10q(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_exp10o(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_logf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_logq(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_logo(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log1pf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log1p(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_log1pq(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_log1po(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log2f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log2(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_log2q(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_log2o(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log10f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log10(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_log10q(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_log10o(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_powf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_pow(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_powq(const tlfloat_quad x, const tlfloat_quad y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_powo(const tlfloat_octuple x, const tlfloat_octuple y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_cbrtf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cbrt(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_cbrtq(const tlfloat_quad a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_cbrto(const tlfloat_octuple a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sinhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sinh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_sinhq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_sinho(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_coshf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cosh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_coshq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_cosho(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tanhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tanh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_tanhq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_tanho(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_asinhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_asinh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_asinhq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_asinho(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_acoshf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_acosh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_acoshq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_acosho(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atanhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atanh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_atanhq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_atanho(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmodf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmod(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_fmodq(const tlfloat_quad x, const tlfloat_quad y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_fmodo(const tlfloat_octuple x, const tlfloat_octuple y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_remainderf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_remainder(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_remainderq(const tlfloat_quad x, const tlfloat_quad y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_remaindero(const tlfloat_octuple x, const tlfloat_octuple y);

#ifdef TLFLOAT_LIBQUADMATH_EMULATION
  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad strtoflt128(const char *s, const char **sp) {
    return tlfloat_strtoq(s, sp);
  }

  /** This function is designed to be used as a replacement for the function of the same name in quadmath.h,
   * but it is actually an alias of tlfloat_snprintf.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int quadmath_snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = tlfloat_vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
  }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad sinq(const tlfloat_quad x) { return tlfloat_sinq(x); }
#endif

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // #ifndef __TLFLOAT_H_INCLUDED__
