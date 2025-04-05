#ifndef __TLFLOAT_H_INCLUDED__
#define __TLFLOAT_H_INCLUDED__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include <tlfloat/tlfloatconfig.hpp>

#ifndef TLFLOAT_FP_NAN
#define TLFLOAT_FP_NAN 0
#define TLFLOAT_FP_INFINITE 1
#define TLFLOAT_FP_ZERO 2
#define TLFLOAT_FP_SUBNORMAL 3
#define TLFLOAT_FP_NORMAL 4
#define TLFLOAT_FP_ILOGB0 ((int)0x80000000)
#define TLFLOAT_FP_ILOGBNAN ((int)2147483647)

#define TLFLOAT_FLT128_MANT_DIG 113
#define TLFLOAT_FLT128_MIN_EXP (-16381)
#define TLFLOAT_FLT128_MAX_EXP 16384
#define TLFLOAT_FLT128_DIG 33
#define TLFLOAT_FLT128_MIN_10_EXP (-4931)
#define TLFLOAT_FLT128_MAX_10_EXP 4932

#define TLFLOAT_FLT256_MANT_DIG 237
#endif

#if defined(TLFLOAT_DOXYGEN)
/** Calls to libquadmath functions are replaced to the corresponding
    calls to tlfloat functions if this macro is defined */
#define TLFLOAT_LIBQUADMATH_EMULATION
#endif

#ifdef __cplusplus
#include <type_traits>

#ifndef TLFLOAT_NO_LIBSTDCXX
#include <iostream>
#include <vector>
#include <string>
#endif

extern "C" {
#endif

#if defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
  typedef __int128_t tlfloat_int128_t_;
  typedef __uint128_t tlfloat_uint128_t_;
#else
/** tlfloat_int128_t_ and tlfloat_uint128_t_ are types for handling
 * 128-bit integer numbers in C. The data size and data structure of
 * these types are the same as ordinary integer types. These are POD
 * types that can be safely passed with C ABI. If the compiler
 * supports __int128_t and __uint128_t, these type are aliases for
 * those types. */
  typedef struct { uint64_t e[1 << 1]; } tlfloat_int128_t_;
  typedef struct { uint64_t e[1 << 1]; } tlfloat_uint128_t_;
#endif

#if defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128)
  typedef __float128 tlfloat_quad_;
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)
  typedef long double tlfloat_quad_;
#elif defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128)
  typedef _Float128 tlfloat_quad_;
#else
/** This type is for handling quadruple-precision IEEE floating-point
 * numbers in C. The data size and data structure of this type are the
 * same as a quad-precision floating-point number. This is a POD type
 * that can be safely passed with C ABI. If the compiler supports
 * __float128, this type is an alias for __float128. When compling
 * with a compiler on which long double is IEEE float 128, this type
 * is an alias for long double. */
  typedef struct { uint64_t e[2]; } tlfloat_quad_;
#endif

/** This type is for handling octuple-precision IEEE floating-point
 * numbers in C. The data size and data structure of this type are the
 * same as an octuple-precision floating-point number. This is a POD
 * type that can be safely passed with C ABI. */
  typedef struct { uint64_t e[4]; } tlfloat_octuple_;

  //

  /** This function returns TLFLOAT_VERSION_MAJOR * 1000000ULL + TLFLOAT_VERSION_MINOR * 1000ULL + TLFLOAT_VERSION_PATCH */
  uint64_t tlfloat_version();

  /** The tlfloat_printf family of functions has the functionality to produce output of numbers of the types defined in TLFloat library, 
   * in addition to the functionality of the standard printf family of functions.
   * To output a number of type defined in TLFloat library,
   * use _m as the length modifier where m is a decimal integer represents the data size of the corresponding argument in bits.
   * For example, use '\%_256g' to output an octuple-precision FP number, and '\%_128d' to output a 128-bit signed integer.
   * Q and O modifier have to be used to output an FP number in tlfloat_quad and tlfloat_octuple types, respectively. */
  int tlfloat_printf(const char *fmt, ...);

  /** See explanation for tlfloat_printf() */
  int tlfloat_fprintf(FILE *fp, const char *fmt, ...);

  /** See explanation for tlfloat_printf() */
  int tlfloat_snprintf(char *str, size_t size, const char *fmt, ...);

  /** See explanation for tlfloat_printf() */
  int tlfloat_vprintf(const char *fmt, va_list ap);

  /** See explanation for tlfloat_printf() */
  int tlfloat_vfprintf(FILE *fp, const char *fmt, va_list ap);

  /** See explanation for tlfloat_printf() */
  int tlfloat_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

#if defined(TLFLOAT_DOXYGEN) || __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 14)
  /** This functionality is experimental. Please use tlfloat_printf
      functions instead. On systems with GLIBC, printf hooks can be
      registered by calling this function.  */
  int tlfloat_registerPrintfHook();

  /** Unregister printf hooks */
  void tlfloat_unregisterPrintfHook();
#endif

  /** The tlfloat_strtod family of functions provides functionality equivalent to the standard strtod family of functions,
   * but corresponds to the types defined in the TLFloat library. */
  double tlfloat_strtod(const char *, const char **);
  float tlfloat_strtof(const char *, const char **);

#if !defined(TLFLOAT_DOXYGEN)
  tlfloat_quad_ tlfloat_strtoq_(const char *, const char **);
  tlfloat_octuple_ tlfloat_strtoo_(const char *, const char **);

  tlfloat_int128_t_ tlfloat_strtoi128(const char *, const char **, const int);
  tlfloat_uint128_t_ tlfloat_strtou128(const char *, const char **, const int);

  //

  double tlfloat_cast_d_q(const tlfloat_quad_    x);
  double tlfloat_cast_d_o(const tlfloat_octuple_ x);

  tlfloat_quad_ tlfloat_cast_q_d_(const double           x);
  tlfloat_quad_ tlfloat_cast_q_o (const tlfloat_octuple_ x);

  tlfloat_octuple_ tlfloat_cast_o_d_(const double       x);
  tlfloat_octuple_ tlfloat_cast_o_q(const tlfloat_quad_ x);

  int64_t tlfloat_cast_i64_q(const tlfloat_quad_  x);
  tlfloat_quad_ tlfloat_cast_q_i64_(const int64_t x);

  int64_t tlfloat_cast_i64_o(const tlfloat_octuple_  x);
  tlfloat_octuple_ tlfloat_cast_o_i64_(const int64_t x);

  uint64_t tlfloat_cast_u64_q(const tlfloat_quad_  x);
  tlfloat_quad_ tlfloat_cast_q_u64_(const uint64_t x);

  uint64_t tlfloat_cast_u64_o(const tlfloat_octuple_  x);
  tlfloat_octuple_ tlfloat_cast_o_u64_(const uint64_t x);

  //

  tlfloat_int128_t_ tlfloat_cast_i128_i64_(const int64_t x);
  int64_t tlfloat_cast_i64_i128(const tlfloat_int128_t_ x);

  tlfloat_uint128_t_ tlfloat_cast_u128_u64_(const uint64_t x);
  uint64_t tlfloat_cast_u64_u128(const tlfloat_uint128_t_ x);

  tlfloat_int128_t_ tlfloat_cast_i128_d_(const double x);
  double tlfloat_cast_d_i128(const tlfloat_int128_t_ x);

  tlfloat_uint128_t_ tlfloat_cast_u128_d_(const double x);
  double tlfloat_cast_d_u128(const tlfloat_uint128_t_ x);

  tlfloat_int128_t_ tlfloat_cast_i128_q(const tlfloat_quad_ x);
  tlfloat_quad_ tlfloat_cast_q_i128(const tlfloat_int128_t_ x);

  tlfloat_uint128_t_ tlfloat_cast_u128_q(const tlfloat_quad_ x);
  tlfloat_quad_ tlfloat_cast_q_u128(const tlfloat_uint128_t_ x);

  tlfloat_int128_t_ tlfloat_cast_i128_o(const tlfloat_octuple_ x);
  tlfloat_octuple_ tlfloat_cast_o_i128(const tlfloat_int128_t_ x);

  tlfloat_uint128_t_ tlfloat_cast_u128_o(const tlfloat_octuple_ x);
  tlfloat_octuple_ tlfloat_cast_o_u128(const tlfloat_uint128_t_ x);

  int tlfloat_eq_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  int tlfloat_ne_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  int tlfloat_lt_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  int tlfloat_le_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  int tlfloat_gt_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  int tlfloat_ge_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);

  tlfloat_int128_t_ tlfloat_add_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_sub_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_mul_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_div_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_mod_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);

  tlfloat_int128_t_ tlfloat_and_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_or_i128_i128 (const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_xor_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y);
  tlfloat_int128_t_ tlfloat_not_i128(const tlfloat_int128_t_ x);

  tlfloat_int128_t_ tlfloat_shl_i128_i(const tlfloat_int128_t_ x, const int y);
  tlfloat_int128_t_ tlfloat_shr_i128_i(const tlfloat_int128_t_ x, const int y);

  tlfloat_int128_t_ tlfloat_neg_i128(const tlfloat_int128_t_ x);

  int tlfloat_eq_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  int tlfloat_ne_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  int tlfloat_lt_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  int tlfloat_le_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  int tlfloat_gt_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  int tlfloat_ge_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);

  tlfloat_uint128_t_ tlfloat_add_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_sub_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_mul_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_div_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_mod_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);

  tlfloat_uint128_t_ tlfloat_and_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_or_u128_u128 (const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_xor_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y);
  tlfloat_uint128_t_ tlfloat_not_u128(const tlfloat_uint128_t_ x);

  tlfloat_uint128_t_ tlfloat_shl_u128_i(const tlfloat_uint128_t_ x, const int y);
  tlfloat_uint128_t_ tlfloat_shr_u128_i(const tlfloat_uint128_t_ x, const int y);

  //

  int tlfloat_eq_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);
  int tlfloat_ne_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);
  int tlfloat_lt_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);
  int tlfloat_le_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);
  int tlfloat_gt_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);
  int tlfloat_ge_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);

  int tlfloat_eq_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  int tlfloat_ne_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  int tlfloat_lt_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  int tlfloat_le_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  int tlfloat_gt_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  int tlfloat_ge_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
#endif // #if !defined(TLFLOAT_DOXYGEN)

  //

  float tlfloat_addf(const float x, const float y);
  double tlfloat_add(const double x, const double y);

  float tlfloat_subf(const float x, const float y);
  double tlfloat_sub(const double x, const double y);

  float tlfloat_mulf(const float x, const float y);
  double tlfloat_mul(const double x, const double y);

  float tlfloat_divf(const float x, const float y);
  double tlfloat_div(const double x, const double y);

  float tlfloat_negf(const float x);
  double tlfloat_neg(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmaf(const float x, const float y, const float z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fma(const double x, const double y, const double z);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sqrtf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sqrt(const double x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fabsf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fabs(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_copysignf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_copysign(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmaxf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmax(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fminf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmin(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fdimf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fdim(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_ldexpf(const float x, const int y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_ldexp(const double x, const int y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_frexpf(const float x, int *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_frexp(const double x, int *y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_modff(const float x, float *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_modf(const double x, double *y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_nextafterf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_nextafter(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_ilogbf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_ilogb(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isnanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isnan(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isinff(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isinf(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_finitef(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_finite(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_fpclassifyf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_fpclassify(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_signbitf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_signbit(const double x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_hypotf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_hypot(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_truncf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_trunc(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_floorf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_floor(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_ceilf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_ceil(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_roundf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_round(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_rintf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_rint(const double x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sinf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sin(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_cosf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cos(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tan(const double x);

  float tlfloat_sinpif(const float x);
  double tlfloat_sinpi(const double x);

  float tlfloat_cospif(const float x);
  double tlfloat_cospi(const double x);

  float tlfloat_tanpif(const float x);
  double tlfloat_tanpi(const double x);

  void tlfloat_sincosf(const float x, float *s, float *c);
  void tlfloat_sincos(const double x, double *s, double *c);

  void tlfloat_sincospif(const float x, float *s, float *c);
  void tlfloat_sincospi(const double x, double *s, double *c);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_asinf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_asin(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_acosf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_acos(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atan(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atan2f(const float y, const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atan2(const double y, const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_expf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_expm1f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_expm1(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_exp2f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp2(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_exp10f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp10(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_logf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log1pf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log1p(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log2f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log2(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log10f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log10(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_powf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_pow(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_cbrtf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cbrt(const double a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sinhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sinh(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_coshf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cosh(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tanhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tanh(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_asinhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_asinh(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_acoshf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_acosh(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atanhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atanh(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmodf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmod(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_remainderf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_remainder(const double x, const double y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_remquof(const float x, const float y, int *quo);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_remquo(const double x, const double y, int *quo);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_erff(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_erf(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_erfcf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_erfc(const double x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tgammaf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tgamma(const double x);

  /** This is experimental implementation of log gamma function. Link with -ltlfloat. */
  float tlfloat_lgammaf(const float x);
  /** This is experimental implementation of log gamma function. Link with -ltlfloat. */
  double tlfloat_lgamma(const double x);

#if !defined(TLFLOAT_DOXYGEN)
  tlfloat_quad_ tlfloat_addq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_addo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_subq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_subo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_mulq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_mulo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_divq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_divo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_negq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_nego(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_fmaq(const tlfloat_quad_ x, const tlfloat_quad_ y, const tlfloat_quad_ z);
  tlfloat_octuple_ tlfloat_fmao(const tlfloat_octuple_ x, const tlfloat_octuple_ y, const tlfloat_octuple_ z);
  tlfloat_quad_ tlfloat_sqrtq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_sqrto(const tlfloat_octuple_ x);

  tlfloat_quad_ tlfloat_fabsq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_fabso(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_copysignq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_copysigno(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_fmaxq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_fmaxo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_fminq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_fmino(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_fdimq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_fdimo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_ldexpq(const tlfloat_quad_ x, const int y);
  tlfloat_octuple_ tlfloat_ldexpo(const tlfloat_octuple_ x, const int y);
  tlfloat_quad_ tlfloat_frexpq(const tlfloat_quad_ x, int *y);
  tlfloat_octuple_ tlfloat_frexpo(const tlfloat_octuple_ x, int *y);
  tlfloat_quad_ tlfloat_modfq(const tlfloat_quad_ x, tlfloat_quad_ *y);
  tlfloat_octuple_ tlfloat_modfo(const tlfloat_octuple_ x, tlfloat_octuple_ *y);
  tlfloat_quad_ tlfloat_nextafterq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_nextaftero(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  int tlfloat_ilogbq(const tlfloat_quad_ x);
  int tlfloat_ilogbo(const tlfloat_octuple_ x);
  int tlfloat_isnanq(const tlfloat_quad_ x);
  int tlfloat_isnano(const tlfloat_octuple_ x);
  int tlfloat_isinfq(const tlfloat_quad_ x);
  int tlfloat_isinfo(const tlfloat_octuple_ x);
  int tlfloat_finiteq(const tlfloat_quad_ x);
  int tlfloat_finiteo(const tlfloat_octuple_ x);
  int tlfloat_fpclassifyq(const tlfloat_quad_ x);
  int tlfloat_fpclassifyo(const tlfloat_octuple_ x);
  int tlfloat_signbitq(const tlfloat_quad_ x);
  int tlfloat_signbito(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_hypotq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_hypoto(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_truncq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_trunco(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_floorq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_flooro(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_ceilq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_ceilo(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_roundq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_roundo(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_rintq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_rinto(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_sinq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_sino(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_cosq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_coso(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_tanq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_tano(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_sinpiq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_sinpio(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_cospiq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_cospio(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_tanpiq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_tanpio(const tlfloat_octuple_ x);
  void tlfloat_sincosq(const tlfloat_quad_ x, tlfloat_quad_ *s, tlfloat_quad_ *c);
  void tlfloat_sincoso(const tlfloat_octuple_ x, tlfloat_octuple_ *s, tlfloat_octuple_ *c);
  void tlfloat_sincospiq(const tlfloat_quad_ x, tlfloat_quad_ *s, tlfloat_quad_ *c);
  void tlfloat_sincospio(const tlfloat_octuple_ x, tlfloat_octuple_ *s, tlfloat_octuple_ *c);
  tlfloat_quad_ tlfloat_asinq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_asino(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_acosq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_acoso(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_atanq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_atano(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_atan2q(const tlfloat_quad_ y, const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_atan2o(const tlfloat_octuple_ y, const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_expq(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_expo(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_expm1q(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_expm1o(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_exp2q(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_exp2o(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_exp10q(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_exp10o(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_logq(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_logo(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_log1pq(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_log1po(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_log2q(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_log2o(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_log10q(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_log10o(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_powq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_powo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_cbrtq(const tlfloat_quad_ a);
  tlfloat_octuple_ tlfloat_cbrto(const tlfloat_octuple_ a);
  tlfloat_quad_ tlfloat_sinhq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_sinho(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_coshq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_cosho(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_tanhq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_tanho(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_asinhq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_asinho(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_acoshq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_acosho(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_atanhq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_atanho(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_erfq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_erfo(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_erfcq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_erfco(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_tgammaq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_tgammao(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_lgammaq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_lgammao(const tlfloat_octuple_ x);
  tlfloat_quad_ tlfloat_fmodq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_fmodo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_remainderq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  tlfloat_octuple_ tlfloat_remaindero(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  tlfloat_quad_ tlfloat_remquoq(const tlfloat_quad_ x, const tlfloat_quad_ y, int *quo);
  tlfloat_octuple_ tlfloat_remquoo(const tlfloat_octuple_ x, const tlfloat_octuple_ y, int *quo);
#endif // #if !defined(TLFLOAT_DOXYGEN)

#ifdef __cplusplus
} // extern "C" {
#endif

#if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128) && !defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128)) || defined(TLFLOAT_DOXYGEN)
/** tlfloat_quad is a trivially copyable type for handling
 * quadruple-precision IEEE floating-point numbers in C and C++11. The
 * data size and data structure of this type are the same as a
 * quad-precision floating-point number. When compling C code, this
 * type is an alias for tlfloat_quad_. When compling C++ code without
 * IEEE float 128 support, this is a struct encapsulating a
 * tlfloat_quad_ variable with operators overloaded. */
struct tlfloat_quad {
  tlfloat_quad_ value;

  tlfloat_quad() { value.e[0] = value.e[1] = 0; }

  // Conversion to/from the corresponding C type
  constexpr tlfloat_quad(const tlfloat_quad_& v) : value(v) {}
  constexpr operator tlfloat_quad_() const { return value; }

  // Upcast / downcast
  explicit tlfloat_quad(const struct tlfloat_octuple& v);

  // Conversion to/from double
  tlfloat_quad(const double& d) : value(tlfloat_cast_q_d_(d)) {}
  explicit operator double() const { return tlfloat_cast_d_q(value); }

  // Conversion to/from integral types
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && (sizeof(T) < 8 || !std::is_unsigned<T>::value)), int>::type = 0>
  tlfloat_quad(const T& i) : value(tlfloat_cast_q_i64_(i)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_unsigned<T>::value && sizeof(T) == 8), int>::type = 0>
  tlfloat_quad(const T& u) : value(tlfloat_cast_q_u64_(u)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && (sizeof(T) < 8 || !std::is_unsigned<T>::value)), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_i64_q(value); }
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_unsigned<T>::value && sizeof(T) == 8), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_u64_q(value); }

  explicit tlfloat_quad(const tlfloat_int128_t_& i);
  explicit tlfloat_quad(const tlfloat_uint128_t_& u);
  explicit operator tlfloat_int128_t_() const;
  explicit operator tlfloat_uint128_t_() const;

#if !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
  explicit tlfloat_quad(const struct tlfloat_int128_t& i);
  explicit tlfloat_quad(const struct tlfloat_uint128_t& u);
#endif

  /** Any non-integer object is memcpy-ed */
  template<typename fptype,
	   typename std::enable_if<(sizeof(fptype)==sizeof(tlfloat_quad_) && std::is_trivially_copyable<fptype>::value &&
				    !std::is_same<fptype, tlfloat_int128_t_>::value && !std::is_same<fptype, tlfloat_uint128_t_>::value &&
#if !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
				    !std::is_same<fptype, tlfloat_int128_t>::value && !std::is_same<fptype, tlfloat_uint128_t>::value &&
#endif
				    !std::is_floating_point<fptype>::value && !std::is_pointer<fptype>::value && !std::is_integral<fptype>::value), int>::type = 0>
  tlfloat_quad(const fptype& s) { memcpy(&value, &s, sizeof(tlfloat_quad_)); }

  template<typename fptype,
	   typename std::enable_if<(sizeof(fptype)==sizeof(tlfloat_quad_) && std::is_trivially_copyable<fptype>::value &&
				    !std::is_same<fptype, tlfloat_int128_t_>::value && !std::is_same<fptype, tlfloat_uint128_t_>::value &&
#if !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
				    !std::is_same<fptype, tlfloat_int128_t>::value && !std::is_same<fptype, tlfloat_uint128_t>::value &&
#endif
				    !std::is_floating_point<fptype>::value && !std::is_pointer<fptype>::value && !std::is_integral<fptype>::value), int>::type = 0>
  explicit operator fptype() const {
    fptype ret;
    memcpy(&ret, &value, sizeof(tlfloat_quad_));
    return ret;
  }

  //

  tlfloat_quad operator+(const tlfloat_quad& rhs) const { return tlfloat_addq(value, rhs.value); }
  tlfloat_quad operator-(const tlfloat_quad& rhs) const { return tlfloat_subq(value, rhs.value); }
  tlfloat_quad operator*(const tlfloat_quad& rhs) const { return tlfloat_mulq(value, rhs.value); }
  tlfloat_quad operator/(const tlfloat_quad& rhs) const { return tlfloat_divq(value, rhs.value); }

  tlfloat_quad operator-() const { return tlfloat_negq(value); }
  tlfloat_quad operator+() const { return *this; }

  bool operator==(const tlfloat_quad& rhs) const { return tlfloat_eq_q_q(value, rhs.value); }
  bool operator!=(const tlfloat_quad& rhs) const { return tlfloat_ne_q_q(value, rhs.value); }
  bool operator< (const tlfloat_quad& rhs) const { return tlfloat_lt_q_q(value, rhs.value); }
  bool operator<=(const tlfloat_quad& rhs) const { return tlfloat_le_q_q(value, rhs.value); }
  bool operator> (const tlfloat_quad& rhs) const { return tlfloat_gt_q_q(value, rhs.value); }
  bool operator>=(const tlfloat_quad& rhs) const { return tlfloat_ge_q_q(value, rhs.value); }

  tlfloat_quad& operator++()    { *this = tlfloat_addq(value, tlfloat_cast_q_i64_(1)); return *this; }
  tlfloat_quad& operator--()    { *this = tlfloat_subq(value, tlfloat_cast_q_i64_(1)); return *this; }
  tlfloat_quad  operator++(int) { tlfloat_quad t = *this; *this = tlfloat_addq(value, tlfloat_cast_q_i64_(1)); return t; }
  tlfloat_quad  operator--(int) { tlfloat_quad t = *this; *this = tlfloat_subq(value, tlfloat_cast_q_i64_(1)); return t; }
};

/** This macro is defined iff tlfloat_quad is not an alias of
    __float128, but a struct defined in tlfloat.h. */
#define TLFLOAT_QUAD_IS_STRUCT
#else // #if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)) && !defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128)) || defined(TLFLOAT_DOXYGEN)
typedef tlfloat_quad_ tlfloat_quad;
#endif

#if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
/** tlfloat_octuple is a trivially copyable type for handling
 * octuple-precision IEEE floating-point numbers in C and older
 * C++11. The data size and data structure of this type are the same
 * as an octuple-precision floating-point number. When compling C
 * code, this type is an alias for tlfloat_octuple_. When compling C++
 * code, this is a struct encapsulating a tlfloat_octuple_ variable
 * with operators overloaded. */
struct tlfloat_octuple {
  tlfloat_octuple_ value;

  tlfloat_octuple() { value.e[0] = value.e[1] = value.e[2] = value.e[3] = 0; }

  // Conversion to/from the corresponding C type
  constexpr tlfloat_octuple(const tlfloat_octuple_& v) : value(v) {}
  constexpr operator tlfloat_octuple_() const { return value; }

  // Upcast / downcast
  tlfloat_octuple(const tlfloat_quad& v) : value(tlfloat_cast_o_q(v)) {}
  explicit operator tlfloat_quad() const { return tlfloat_cast_q_o(value); }

  // Conversion to/from double
  tlfloat_octuple(const double& d) : value(tlfloat_cast_o_d_(d)) {}
  explicit operator double() const { return tlfloat_cast_d_o(value); }

  // Conversion to/from integral types
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && (sizeof(T) < 8 || !std::is_unsigned<T>::value)), int>::type = 0>
  tlfloat_octuple(const T& i) : value(tlfloat_cast_o_i64_(i)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_unsigned<T>::value && sizeof(T) == 8), int>::type = 0>
  tlfloat_octuple(const T& u) : value(tlfloat_cast_o_u64_(u)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && (sizeof(T) < 8 || !std::is_unsigned<T>::value)), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_i64_o(value); }
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && std::is_unsigned<T>::value && sizeof(T) == 8), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_u64_o(value); }

  tlfloat_octuple(const tlfloat_int128_t_& i);
  tlfloat_octuple(const tlfloat_uint128_t_& u);
  explicit operator tlfloat_int128_t_() const;
  explicit operator tlfloat_uint128_t_() const;

#if !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
  tlfloat_octuple(const struct tlfloat_int128_t& i);
  tlfloat_octuple(const struct tlfloat_uint128_t& u);
#endif

  /** Any non-integer object is memcpy-ed */
  template<typename fptype,
	   typename std::enable_if<(sizeof(fptype)==sizeof(tlfloat_octuple_) && std::is_trivially_copyable<fptype>::value &&
				    !std::is_floating_point<fptype>::value && !std::is_pointer<fptype>::value && !std::is_integral<fptype>::value), int>::type = 0>
  tlfloat_octuple(const fptype& s) { memcpy(&value, &s, sizeof(tlfloat_octuple_)); }

  template<typename fptype,
	   typename std::enable_if<(sizeof(fptype)==sizeof(tlfloat_octuple_) && std::is_trivially_copyable<fptype>::value &&
				    !std::is_floating_point<fptype>::value && !std::is_pointer<fptype>::value && !std::is_integral<fptype>::value), int>::type = 0>
  explicit operator fptype() const {
    fptype ret;
    memcpy(&ret, &value, sizeof(tlfloat_octuple_));
    return ret;
  }

  //

  tlfloat_octuple operator+(const tlfloat_octuple& rhs) const { return tlfloat_addo(value, rhs.value); }
  tlfloat_octuple operator-(const tlfloat_octuple& rhs) const { return tlfloat_subo(value, rhs.value); }
  tlfloat_octuple operator*(const tlfloat_octuple& rhs) const { return tlfloat_mulo(value, rhs.value); }
  tlfloat_octuple operator/(const tlfloat_octuple& rhs) const { return tlfloat_divo(value, rhs.value); }

  tlfloat_octuple operator-() const { return tlfloat_nego(value); }
  tlfloat_octuple operator+() const { return *this; }

  bool operator==(const tlfloat_octuple& rhs) const { return tlfloat_eq_o_o(value, rhs.value); }
  bool operator!=(const tlfloat_octuple& rhs) const { return tlfloat_ne_o_o(value, rhs.value); }
  bool operator< (const tlfloat_octuple& rhs) const { return tlfloat_lt_o_o(value, rhs.value); }
  bool operator<=(const tlfloat_octuple& rhs) const { return tlfloat_le_o_o(value, rhs.value); }
  bool operator> (const tlfloat_octuple& rhs) const { return tlfloat_gt_o_o(value, rhs.value); }
  bool operator>=(const tlfloat_octuple& rhs) const { return tlfloat_ge_o_o(value, rhs.value); }

  tlfloat_octuple& operator++()    { *this = tlfloat_addo(value, tlfloat_cast_o_i64_(1)); return *this; }
  tlfloat_octuple& operator--()    { *this = tlfloat_subo(value, tlfloat_cast_o_i64_(1)); return *this; }
  tlfloat_octuple  operator++(int) { tlfloat_octuple t = *this; *this = tlfloat_addo(value, tlfloat_cast_o_i64_(1)); return t; }
  tlfloat_octuple  operator--(int) { tlfloat_octuple t = *this; *this = tlfloat_subo(value, tlfloat_cast_o_i64_(1)); return t; }
};

#define TLFLOAT_OCTUPLE_IS_STRUCT
#else // #if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
typedef tlfloat_octuple_ tlfloat_octuple;
#endif

#if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)) || defined(TLFLOAT_DOXYGEN)
/** tlfloat_int128_t is a trivially copyable type for handling 128-bit
 * signed integer in C and C++11. The data size and data structure of
 * this type are the same as ordinary integer types. When compling C
 * code, this type is an alias for tlfloat_int128_t_. When compling
 * C++ code without __int128_t support, this is a struct encapsulating
 * a tlfloat_int128_t variable with operators overloaded. */
struct tlfloat_int128_t {
  tlfloat_int128_t_ value;

  tlfloat_int128_t() { value.e[0] = value.e[1] = 0; }

  // Conversion to/from the corresponding C type
  tlfloat_int128_t(const tlfloat_int128_t_& v) : value(v) {}
  operator tlfloat_int128_t_() const { return value; }

  // Conversion to/from double
  explicit tlfloat_int128_t(const double& d) : value(tlfloat_cast_i128_d_(d)) {}
  explicit operator double() const { return tlfloat_cast_d_i128(value); }

  // Conversion to/from tlfloat_uint128_
  explicit tlfloat_int128_t(const tlfloat_uint128_t_& d);
  explicit operator tlfloat_uint128_t_() const;

  // Conversion to/from integral types
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && !std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  tlfloat_int128_t(const T& i) : value(tlfloat_cast_i128_i64_(i)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value &&  std::is_unsigned<T>::value && sizeof(T) <  8), int>::type = 0>
  explicit tlfloat_int128_t(const T& u) : value(tlfloat_cast_i128_i64_((int64_t)u)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value &&  std::is_unsigned<T>::value && sizeof(T) == 8), int>::type = 0>
  explicit tlfloat_int128_t(const T& u) : value((tlfloat_int128_t)tlfloat_cast_u128_u64_(u)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && !std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_i64_i128(value); }
  template<typename T, typename std::enable_if<(std::is_integral<T>::value &&  std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  explicit operator T() const { tlfloat_uint128_t_ v; memcpy(&v, &value, sizeof(v)); return tlfloat_cast_u64_u128(v); }

#ifdef __BIGINT_HPP_INCLUDED__
  tlfloat_int128_t(const tlfloat::BigInt<7>& d) { memcpy((void *)this, (void *)&d, sizeof(*this)); }
  operator tlfloat::BigInt<7>() const { tlfloat::BigInt<7> v; memcpy((void *)&v, (void *)this, sizeof(v)); return v; }
#endif

  explicit tlfloat_int128_t(const tlfloat_quad_& f) : value(tlfloat_cast_i128_q(f)) {}
  explicit operator tlfloat_quad_() const { return tlfloat_cast_q_i128(value); }
  explicit tlfloat_int128_t(const tlfloat_octuple_& f) : value(tlfloat_cast_i128_o(f)) {}
  explicit operator tlfloat_octuple_() const { return tlfloat_cast_o_i128(value); }

  //

  tlfloat_int128_t operator+(const tlfloat_int128_t& rhs) const { return tlfloat_add_i128_i128(value, rhs.value); }
  tlfloat_int128_t operator-(const tlfloat_int128_t& rhs) const { return tlfloat_sub_i128_i128(value, rhs.value); }
  tlfloat_int128_t operator*(const tlfloat_int128_t& rhs) const { return tlfloat_mul_i128_i128(value, rhs.value); }
  tlfloat_int128_t operator/(const tlfloat_int128_t& rhs) const { return tlfloat_div_i128_i128(value, rhs.value); }
  tlfloat_int128_t operator%(const tlfloat_int128_t& rhs) const { return tlfloat_mod_i128_i128(value, rhs.value); }

  tlfloat_int128_t operator-() const { return tlfloat_neg_i128(value); }
  tlfloat_int128_t operator+() const { return *this; }

  tlfloat_int128_t operator&(const tlfloat_int128_t& rhs) const { return tlfloat_and_i128_i128(value, rhs.value); }
  tlfloat_int128_t operator|(const tlfloat_int128_t& rhs) const { return tlfloat_or_i128_i128 (value, rhs.value); }
  tlfloat_int128_t operator^(const tlfloat_int128_t& rhs) const { return tlfloat_xor_i128_i128(value, rhs.value); }
  tlfloat_int128_t operator~() const { return tlfloat_not_i128(value); }

  tlfloat_int128_t operator<<(const int& rhs) const { return tlfloat_shl_i128_i(value, rhs); }
  tlfloat_int128_t operator>>(const int& rhs) const { return tlfloat_shr_i128_i(value, rhs); }
  tlfloat_int128_t& operator<<=(int n) { *this = *this << n; return *this; }
  tlfloat_int128_t& operator>>=(int n) { *this = *this >> n; return *this; }

  bool operator==(const tlfloat_int128_t& rhs) const { return tlfloat_eq_i128_i128(value, rhs.value); }
  bool operator!=(const tlfloat_int128_t& rhs) const { return tlfloat_ne_i128_i128(value, rhs.value); }
  bool operator< (const tlfloat_int128_t& rhs) const { return tlfloat_lt_i128_i128(value, rhs.value); }
  bool operator<=(const tlfloat_int128_t& rhs) const { return tlfloat_le_i128_i128(value, rhs.value); }
  bool operator> (const tlfloat_int128_t& rhs) const { return tlfloat_gt_i128_i128(value, rhs.value); }
  bool operator>=(const tlfloat_int128_t& rhs) const { return tlfloat_ge_i128_i128(value, rhs.value); }

  tlfloat_int128_t& operator++()    { *this = tlfloat_add_i128_i128(value, tlfloat_cast_i128_i64_(1)); return *this; }
  tlfloat_int128_t& operator--()    { *this = tlfloat_sub_i128_i128(value, tlfloat_cast_i128_i64_(1)); return *this; }
  tlfloat_int128_t  operator++(int) { tlfloat_int128_t t = *this; *this = tlfloat_add_i128_i128(value, tlfloat_cast_i128_i64_(1)); return t; }
  tlfloat_int128_t  operator--(int) { tlfloat_int128_t t = *this; *this = tlfloat_sub_i128_i128(value, tlfloat_cast_i128_i64_(1)); return t; }
};

/** tlfloat_uint128_t is a trivially copyable type for handling
 * 128-bit unsigned integer in C and C++11. The data size and data
 * structure of this type are the same as ordinary integer types. When
 * compling C code, this type is an alias for tlfloat_uint128_t_. When
 * compling C++ code without __uint128_t support, this is a struct
 * encapsulating a tlfloat_int128_t variable with operators
 * overloaded. */
struct tlfloat_uint128_t {
  tlfloat_uint128_t_ value;

  tlfloat_uint128_t() { value.e[0] = value.e[1] = 0; }

  // Conversion to/from the corresponding C type
  tlfloat_uint128_t(const tlfloat_uint128_t_& v) : value(v) {}
  operator tlfloat_uint128_t_() const { return value; }

  // Conversion to/from double
  explicit tlfloat_uint128_t(const double& d) : value(tlfloat_cast_u128_d_(d)) {}
  explicit operator double() const { return tlfloat_cast_d_u128(value); }

  // Conversion to/from tlfloat_int128_t
  explicit tlfloat_uint128_t(const tlfloat_int128_t& d) { memcpy((void *)this, (void *)&d, sizeof(*this)); }
  explicit operator tlfloat_int128_t() const { tlfloat_int128_t v; memcpy((void *)&v, (void *)this, sizeof(v)); return v; }

  // Conversion to/from integral types
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && !std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  explicit tlfloat_uint128_t(const T& i) : value(tlfloat_cast_u128_u64_(i)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value &&  std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  tlfloat_uint128_t(const T& u) : value(tlfloat_cast_u128_u64_(u)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && !std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  explicit operator T() const { tlfloat_int128_t_ v; memcpy(&v, &value, sizeof(v)); return (T)tlfloat_cast_i64_i128(v); }
  template<typename T, typename std::enable_if<(std::is_integral<T>::value &&  std::is_unsigned<T>::value && sizeof(T) <= 8), int>::type = 0>
  explicit operator T() const { return (T)tlfloat_cast_u64_u128(value); }

#ifdef __BIGINT_HPP_INCLUDED__
  tlfloat_uint128_t(const tlfloat::BigUInt<7>& d) { memcpy((void *)this, (void *)&d, sizeof(*this)); }
  operator tlfloat::BigUInt<7>() const { tlfloat::BigUInt<7> v; memcpy((void *)&v, (void *)this, sizeof(v)); return v; }
#endif

  explicit tlfloat_uint128_t(const tlfloat_quad_& f) : value(tlfloat_cast_u128_q(f)) {}
  explicit operator tlfloat_quad_() const { return tlfloat_cast_q_u128(value); }
  explicit tlfloat_uint128_t(const tlfloat_octuple_& f) : value(tlfloat_cast_u128_o(f)) {}
  explicit operator tlfloat_octuple_() const { return tlfloat_cast_o_u128(value); }

  //

  tlfloat_uint128_t operator+(const tlfloat_uint128_t& rhs) const { return tlfloat_add_u128_u128(value, rhs.value); }
  tlfloat_uint128_t operator-(const tlfloat_uint128_t& rhs) const { return tlfloat_sub_u128_u128(value, rhs.value); }
  tlfloat_uint128_t operator*(const tlfloat_uint128_t& rhs) const { return tlfloat_mul_u128_u128(value, rhs.value); }
  tlfloat_uint128_t operator/(const tlfloat_uint128_t& rhs) const { return tlfloat_div_u128_u128(value, rhs.value); }
  tlfloat_uint128_t operator%(const tlfloat_uint128_t& rhs) const { return tlfloat_mod_u128_u128(value, rhs.value); }

  tlfloat_uint128_t operator-() const { return (tlfloat_uint128_t)tlfloat_neg_i128((tlfloat_int128_t)value); }
  tlfloat_uint128_t operator+() const { return *this; }

  tlfloat_uint128_t operator&(const tlfloat_uint128_t& rhs) const { return tlfloat_and_u128_u128(value, rhs.value); }
  tlfloat_uint128_t operator|(const tlfloat_uint128_t& rhs) const { return tlfloat_or_u128_u128 (value, rhs.value); }
  tlfloat_uint128_t operator^(const tlfloat_uint128_t& rhs) const { return tlfloat_xor_u128_u128(value, rhs.value); }
  tlfloat_uint128_t operator~() const { return tlfloat_not_u128(value); }

  tlfloat_uint128_t operator<<(const int& rhs) const { return tlfloat_shl_u128_i(value, rhs); }
  tlfloat_uint128_t operator>>(const int& rhs) const { return tlfloat_shr_u128_i(value, rhs); }
  tlfloat_uint128_t& operator<<=(int n) { *this = *this << n; return *this; }
  tlfloat_uint128_t& operator>>=(int n) { *this = *this >> n; return *this; }

  bool operator==(const tlfloat_uint128_t& rhs) const { return tlfloat_eq_u128_u128(value, rhs.value); }
  bool operator!=(const tlfloat_uint128_t& rhs) const { return tlfloat_ne_u128_u128(value, rhs.value); }
  bool operator< (const tlfloat_uint128_t& rhs) const { return tlfloat_lt_u128_u128(value, rhs.value); }
  bool operator<=(const tlfloat_uint128_t& rhs) const { return tlfloat_le_u128_u128(value, rhs.value); }
  bool operator> (const tlfloat_uint128_t& rhs) const { return tlfloat_gt_u128_u128(value, rhs.value); }
  bool operator>=(const tlfloat_uint128_t& rhs) const { return tlfloat_ge_u128_u128(value, rhs.value); }

  tlfloat_uint128_t& operator++()    { *this = tlfloat_add_u128_u128(value, tlfloat_cast_u128_u64_(1)); return *this; }
  tlfloat_uint128_t& operator--()    { *this = tlfloat_sub_u128_u128(value, tlfloat_cast_u128_u64_(1)); return *this; }
  tlfloat_uint128_t  operator++(int) { tlfloat_uint128_t t = *this; *this = tlfloat_add_u128_u128(value, tlfloat_cast_u128_u64_(1)); return t; }
  tlfloat_uint128_t  operator--(int) { tlfloat_uint128_t t = *this; *this = tlfloat_sub_u128_u128(value, tlfloat_cast_u128_u64_(1)); return t; }
};

inline tlfloat_int128_t::tlfloat_int128_t(const tlfloat_uint128_t_& d) { memcpy((void *)this, (void *)&d, sizeof(*this)); }
inline tlfloat_int128_t::operator tlfloat_uint128_t_() const { tlfloat_uint128_t v; memcpy((void *)&v, (void *)this, sizeof(v)); return v; }

/** This macro is defined iff tlfloat_int128_t and tlfloat_uint128_t
    are not aliases of __int128_t and __uint128_t, but structs defined
    in tlfloat.h. */
#define TLFLOAT_INT128_IS_STRUCT
#else
typedef tlfloat_int128_t_ tlfloat_int128_t;
typedef tlfloat_uint128_t_ tlfloat_uint128_t;
#endif // #if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)) || defined(TLFLOAT_DOXYGEN)

#if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
inline tlfloat_octuple::tlfloat_octuple(const tlfloat_int128_t_& i) : value(tlfloat_cast_o_i128(i)) {}
inline tlfloat_octuple::tlfloat_octuple(const tlfloat_uint128_t_& u) : value(tlfloat_cast_o_u128(u)) {}
inline tlfloat_octuple::operator tlfloat_int128_t_() const { return tlfloat_cast_i128_o(value); }
inline tlfloat_octuple::operator tlfloat_uint128_t_() const { return tlfloat_cast_u128_o(value); }
#if !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
inline tlfloat_octuple::tlfloat_octuple(const struct tlfloat_int128_t& i) : value(tlfloat_cast_o_i128(i)) {}
inline tlfloat_octuple::tlfloat_octuple(const struct tlfloat_uint128_t& u) : value(tlfloat_cast_o_u128(u)) {}
#endif
#endif

#if defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) || defined(TLFLOAT_DOXYGEN)
#define TLFLOAT_M_Eq 0x1.5bf0a8b1457695355fb8ac404e7ap+1Q
#define TLFLOAT_M_LOG2Eq 0x1.71547652b82fe1777d0ffda0d23ap+0Q
#define TLFLOAT_M_LOG10Eq 0x1.bcb7b1526e50e32a6ab7555f5a68p-2Q
#define TLFLOAT_M_LN2q 0x1.62e42fefa39ef35793c7673007e6p-1Q
#define TLFLOAT_M_LN10q 0x1.26bb1bbb5551582dd4adac5705a6p+1Q
#define TLFLOAT_M_PIq 0x1.921fb54442d18469898cc51701b8p+1Q
#define TLFLOAT_M_PI_2q 0x1.921fb54442d18469898cc51701b8p+0Q
#define TLFLOAT_M_PI_4q 0x1.921fb54442d18469898cc51701b8p-1Q
#define TLFLOAT_M_1_PIq 0x1.45f306dc9c882a53f84eafa3ea6ap-2Q
#define TLFLOAT_M_2_PIq 0x1.45f306dc9c882a53f84eafa3ea6ap-1Q
#define TLFLOAT_M_2_SQRTPIq 0x1.20dd750429b6d11ae3a914fed7fep+0Q
#define TLFLOAT_M_SQRT2q 0x1.6a09e667f3bcc908b2fb1366ea95p+0Q
#define TLFLOAT_M_SQRT1_2q 0x1.6a09e667f3bcc908b2fb1366ea95p-1Q
#define TLFLOAT_FLT128_MAX 0x1.ffffffffffffffffffffffffffffp+16383Q
#define TLFLOAT_FLT128_MIN 0x1p-16382Q
#define TLFLOAT_FLT128_DENORM_MIN 0x0.0000000000000000000000000001p-16382Q
#define TLFLOAT_FLT128_TRUE_MIN 0x0.0000000000000000000000000001p-16382Q
#define TLFLOAT_FLT128_EPSILON 0x1p-112Q
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)
#define TLFLOAT_M_Eq 0x1.5bf0a8b1457695355fb8ac404e7ap+1L
#define TLFLOAT_M_LOG2Eq 0x1.71547652b82fe1777d0ffda0d23ap+0L
#define TLFLOAT_M_LOG10Eq 0x1.bcb7b1526e50e32a6ab7555f5a68p-2L
#define TLFLOAT_M_LN2q 0x1.62e42fefa39ef35793c7673007e6p-1L
#define TLFLOAT_M_LN10q 0x1.26bb1bbb5551582dd4adac5705a6p+1L
#define TLFLOAT_M_PIq 0x1.921fb54442d18469898cc51701b8p+1L
#define TLFLOAT_M_PI_2q 0x1.921fb54442d18469898cc51701b8p+0L
#define TLFLOAT_M_PI_4q 0x1.921fb54442d18469898cc51701b8p-1L
#define TLFLOAT_M_1_PIq 0x1.45f306dc9c882a53f84eafa3ea6ap-2L
#define TLFLOAT_M_2_PIq 0x1.45f306dc9c882a53f84eafa3ea6ap-1L
#define TLFLOAT_M_2_SQRTPIq 0x1.20dd750429b6d11ae3a914fed7fep+0L
#define TLFLOAT_M_SQRT2q 0x1.6a09e667f3bcc908b2fb1366ea95p+0L
#define TLFLOAT_M_SQRT1_2q 0x1.6a09e667f3bcc908b2fb1366ea95p-1L
#define TLFLOAT_FLT128_MAX 0x1.ffffffffffffffffffffffffffffp+16383L
#define TLFLOAT_FLT128_MIN 0x1p-16382L
#define TLFLOAT_FLT128_DENORM_MIN 0x0.0000000000000000000000000001p-16382L
#define TLFLOAT_FLT128_TRUE_MIN 0x0.0000000000000000000000000001p-16382L
#define TLFLOAT_FLT128_EPSILON 0x1p-112L
#elif defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128)
#define TLFLOAT_M_Eq 0x1.5bf0a8b1457695355fb8ac404e7ap+1F128
#define TLFLOAT_M_LOG2Eq 0x1.71547652b82fe1777d0ffda0d23ap+0F128
#define TLFLOAT_M_LOG10Eq 0x1.bcb7b1526e50e32a6ab7555f5a68p-2F128
#define TLFLOAT_M_LN2q 0x1.62e42fefa39ef35793c7673007e6p-1F128
#define TLFLOAT_M_LN10q 0x1.26bb1bbb5551582dd4adac5705a6p+1F128
#define TLFLOAT_M_PIq 0x1.921fb54442d18469898cc51701b8p+1F128
#define TLFLOAT_M_PI_2q 0x1.921fb54442d18469898cc51701b8p+0F128
#define TLFLOAT_M_PI_4q 0x1.921fb54442d18469898cc51701b8p-1F128
#define TLFLOAT_M_1_PIq 0x1.45f306dc9c882a53f84eafa3ea6ap-2F128
#define TLFLOAT_M_2_PIq 0x1.45f306dc9c882a53f84eafa3ea6ap-1F128
#define TLFLOAT_M_2_SQRTPIq 0x1.20dd750429b6d11ae3a914fed7fep+0F128
#define TLFLOAT_M_SQRT2q 0x1.6a09e667f3bcc908b2fb1366ea95p+0F128
#define TLFLOAT_M_SQRT1_2q 0x1.6a09e667f3bcc908b2fb1366ea95p-1F128
#define TLFLOAT_FLT128_MAX 0x1.ffffffffffffffffffffffffffffp+16383F128
#define TLFLOAT_FLT128_MIN 0x1p-16382F128
#define TLFLOAT_FLT128_DENORM_MIN 0x0.0000000000000000000000000001p-16382F128
#define TLFLOAT_FLT128_TRUE_MIN 0x0.0000000000000000000000000001p-16382F128
#define TLFLOAT_FLT128_EPSILON 0x1p-112F128
#elif defined(__cplusplus)
#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
static inline constexpr tlfloat_quad tlfloat_constq(uint64_t h, uint64_t l) { return tlfloat_quad_ { l, h }; }
#else
static inline constexpr tlfloat_quad tlfloat_constq(uint64_t h, uint64_t l) { return tlfloat_quad_ { h, l }; }
#endif

#define TLFLOAT_M_Eq tlfloat_constq( 0x40005bf0a8b14576, 0x95355fb8ac404e7a )
#define TLFLOAT_M_LOG2Eq tlfloat_constq( 0x3fff71547652b82f, 0xe1777d0ffda0d23a )
#define TLFLOAT_M_LOG10Eq tlfloat_constq( 0x3ffdbcb7b1526e50, 0xe32a6ab7555f5a68 )
#define TLFLOAT_M_LN2q tlfloat_constq( 0x3ffe62e42fefa39e, 0xf35793c7673007e6 )
#define TLFLOAT_M_LN10q tlfloat_constq( 0x400026bb1bbb5551, 0x582dd4adac5705a6 )
#define TLFLOAT_M_PIq tlfloat_constq( 0x4000921fb54442d1, 0x8469898cc51701b8 )
#define TLFLOAT_M_PI_2q tlfloat_constq( 0x3fff921fb54442d1, 0x8469898cc51701b8 )
#define TLFLOAT_M_PI_4q tlfloat_constq( 0x3ffe921fb54442d1, 0x8469898cc51701b8 )
#define TLFLOAT_M_1_PIq tlfloat_constq( 0x3ffd45f306dc9c88, 0x2a53f84eafa3ea6a )
#define TLFLOAT_M_2_PIq tlfloat_constq( 0x3ffe45f306dc9c88, 0x2a53f84eafa3ea6a )
#define TLFLOAT_M_2_SQRTPIq tlfloat_constq( 0x3fff20dd750429b6, 0xd11ae3a914fed7fe )
#define TLFLOAT_M_SQRT2q tlfloat_constq( 0x3fff6a09e667f3bc, 0xc908b2fb1366ea95 )
#define TLFLOAT_M_SQRT1_2q tlfloat_constq( 0x3ffe6a09e667f3bc, 0xc908b2fb1366ea95 )
#define TLFLOAT_FLT128_MAX tlfloat_constq( 0x7ffeffffffffffff, 0xffffffffffffffff )
#define TLFLOAT_FLT128_MIN tlfloat_constq( 0x0001000000000000, 0x0000000000000000 )
#define TLFLOAT_FLT128_DENORM_MIN tlfloat_constq( 0x0000000000000000, 0x0000000000000001 )
#define TLFLOAT_FLT128_TRUE_MIN tlfloat_constq( 0x0000000000000000, 0x0000000000000001 )
#define TLFLOAT_FLT128_EPSILON tlfloat_constq( 0x3f8f000000000000, 0x0000000000000000 )
#elif defined(__STDC_VERSION__)

#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
#define TLFLOAT_M_Eq ((tlfloat_quad_) { 0x95355fb8ac404e7a, 0x40005bf0a8b14576 })
#define TLFLOAT_M_LOG2Eq ((tlfloat_quad_) { 0xe1777d0ffda0d23a, 0x3fff71547652b82f })
#define TLFLOAT_M_LOG10Eq ((tlfloat_quad_) { 0xe32a6ab7555f5a68, 0x3ffdbcb7b1526e50 })
#define TLFLOAT_M_LN2q ((tlfloat_quad_) { 0xf35793c7673007e6, 0x3ffe62e42fefa39e })
#define TLFLOAT_M_LN10q ((tlfloat_quad_) { 0x582dd4adac5705a6, 0x400026bb1bbb5551 })
#define TLFLOAT_M_PIq ((const tlfloat_quad_) { 0x8469898cc51701b8, 0x4000921fb54442d1 })
#define TLFLOAT_M_PI_2q ((tlfloat_quad_) { 0x8469898cc51701b8, 0x3fff921fb54442d1 })
#define TLFLOAT_M_PI_4q ((tlfloat_quad_) { 0x8469898cc51701b8, 0x3ffe921fb54442d1 })
#define TLFLOAT_M_1_PIq ((tlfloat_quad_) { 0x2a53f84eafa3ea6a, 0x3ffd45f306dc9c88 })
#define TLFLOAT_M_2_PIq ((tlfloat_quad_) { 0x2a53f84eafa3ea6a, 0x3ffe45f306dc9c88 })
#define TLFLOAT_M_2_SQRTPIq ((tlfloat_quad_) { 0xd11ae3a914fed7fe, 0x3fff20dd750429b6 })
#define TLFLOAT_M_SQRT2q ((tlfloat_quad_) { 0xc908b2fb1366ea95, 0x3fff6a09e667f3bc })
#define TLFLOAT_M_SQRT1_2q ((tlfloat_quad_) { 0xc908b2fb1366ea95, 0x3ffe6a09e667f3bc })
#define TLFLOAT_FLT128_MAX ((tlfloat_quad_) { 0xffffffffffffffff, 0x7ffeffffffffffff })
#define TLFLOAT_FLT128_MIN ((tlfloat_quad_) { 0x0000000000000000, 0x0001000000000000 })
#define TLFLOAT_FLT128_DENORM_MIN ((tlfloat_quad_) { 0x0000000000000001, 0x0000000000000000 })
#define TLFLOAT_FLT128_TRUE_MIN ((tlfloat_quad_) { 0x0000000000000001, 0x0000000000000000 })
#define TLFLOAT_FLT128_EPSILON ((tlfloat_quad_) { 0x0000000000000000, 0x3f8f000000000000 })
#else
#define TLFLOAT_M_Eq ((tlfloat_quad_) { 0x40005bf0a8b14576, 0x95355fb8ac404e7a })
#define TLFLOAT_M_LOG2Eq ((tlfloat_quad_) { 0x3fff71547652b82f, 0xe1777d0ffda0d23a })
#define TLFLOAT_M_LOG10Eq ((tlfloat_quad_) { 0x3ffdbcb7b1526e50, 0xe32a6ab7555f5a68 })
#define TLFLOAT_M_LN2q ((tlfloat_quad_) { 0x3ffe62e42fefa39e, 0xf35793c7673007e6 })
#define TLFLOAT_M_LN10q ((tlfloat_quad_) { 0x400026bb1bbb5551, 0x582dd4adac5705a6 })
#define TLFLOAT_M_PIq ((const tlfloat_quad_) { 0x4000921fb54442d1, 0x8469898cc51701b8 })
#define TLFLOAT_M_PI_2q ((tlfloat_quad_) { 0x3fff921fb54442d1, 0x8469898cc51701b8 })
#define TLFLOAT_M_PI_4q ((tlfloat_quad_) { 0x3ffe921fb54442d1, 0x8469898cc51701b8 })
#define TLFLOAT_M_1_PIq ((tlfloat_quad_) { 0x3ffd45f306dc9c88, 0x2a53f84eafa3ea6a })
#define TLFLOAT_M_2_PIq ((tlfloat_quad_) { 0x3ffe45f306dc9c88, 0x2a53f84eafa3ea6a })
#define TLFLOAT_M_2_SQRTPIq ((tlfloat_quad_) { 0x3fff20dd750429b6, 0xd11ae3a914fed7fe })
#define TLFLOAT_M_SQRT2q ((tlfloat_quad_) { 0x3fff6a09e667f3bc, 0xc908b2fb1366ea95 })
#define TLFLOAT_M_SQRT1_2q ((tlfloat_quad_) { 0x3ffe6a09e667f3bc, 0xc908b2fb1366ea95 })
#define TLFLOAT_FLT128_MAX ((tlfloat_quad_) { 0x7ffeffffffffffff, 0xffffffffffffffff })
#define TLFLOAT_FLT128_MIN ((tlfloat_quad_) { 0x0001000000000000, 0x0000000000000000 })
#define TLFLOAT_FLT128_DENORM_MIN ((tlfloat_quad_) { 0x0000000000000000, 0x0000000000000001 })
#define TLFLOAT_FLT128_TRUE_MIN ((tlfloat_quad_) { 0x0000000000000000, 0x0000000000000001 })
#define TLFLOAT_FLT128_EPSILON ((tlfloat_quad_) { 0x3f8f000000000000, 0x0000000000000000 })
#endif

#endif

#if defined(__cplusplus)
#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
static inline constexpr tlfloat_octuple tlfloat_consto(uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3) { return tlfloat_octuple_ { w3, w2, w1, w0 }; }
#else
static inline constexpr tlfloat_octuple tlfloat_consto(uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3) { return tlfloat_octuple_ { w0, w1, w2, w3 }; }
#endif

#define TLFLOAT_M_Eo tlfloat_consto(0x400005bf0a8b1457LL, 0x695355fb8ac404e7LL, 0xa79e3b1738b079c5LL, 0xa6d2b53c26c8228dLL)
#define TLFLOAT_M_LOG2Eo tlfloat_consto(0x3ffff71547652b82LL, 0xfe1777d0ffda0d23LL, 0xa7d11d6aef551badLL, 0x2b4b1164a2cd9a34LL)
#define TLFLOAT_M_LOG10Eo tlfloat_consto(0x3fffdbcb7b1526e5LL, 0x0e32a6ab7555f5a6LL, 0x7b8647dc68c048b9LL, 0x34404747e5a89ef2LL)
#define TLFLOAT_M_LN2o tlfloat_consto(0x3fffe62e42fefa39LL, 0xef35793c7673007eLL, 0x5ed5e81e6864ce53LL, 0x16c5b141a2eb7175LL)
#define TLFLOAT_M_LN10o tlfloat_consto(0x4000026bb1bbb555LL, 0x1582dd4adac5705aLL, 0x61451c51fd9f3b4bLL, 0xbf21d078c3d0403eLL)
#define TLFLOAT_M_PIo tlfloat_consto(0x40000921fb54442dLL, 0x18469898cc51701bLL, 0x839a252049c1114cLL, 0xf98e804177d4c762LL)
#define TLFLOAT_M_PI_2o tlfloat_consto(0x3ffff921fb54442dLL, 0x18469898cc51701bLL, 0x839a252049c1114cLL, 0xf98e804177d4c762LL)
#define TLFLOAT_M_PI_4o tlfloat_consto(0x3fffe921fb54442dLL, 0x18469898cc51701bLL, 0x839a252049c1114cLL, 0xf98e804177d4c762LL)
#define TLFLOAT_M_1_PIo tlfloat_consto(0x3fffd45f306dc9c8LL, 0x82a53f84eafa3ea6LL, 0x9bb81b6c52b32788LL, 0x72083fca2c757bd7LL)
#define TLFLOAT_M_2_PIo tlfloat_consto(0x3fffe45f306dc9c8LL, 0x82a53f84eafa3ea6LL, 0x9bb81b6c52b32788LL, 0x72083fca2c757bd7LL)
#define TLFLOAT_M_2_SQRTPIo tlfloat_consto(0x3ffff20dd750429bLL, 0x6d11ae3a914fed7fLL, 0xd8688281341d7587LL, 0xcea2e7342b06199dLL)
#define TLFLOAT_M_SQRT2o tlfloat_consto(0x3ffff6a09e667f3bLL, 0xcc908b2fb1366ea9LL, 0x57d3e3adec175127LL, 0x75099da2f590b066LL)
#define TLFLOAT_M_SQRT1_2o tlfloat_consto(0x3fffe6a09e667f3bLL, 0xcc908b2fb1366ea9LL, 0x57d3e3adec175127LL, 0x75099da2f590b066LL)
#define TLFLOAT_FLT256_MAX tlfloat_consto(0x7fffefffffffffffLL, 0xffffffffffffffffLL, 0xffffffffffffffffLL, 0xffffffffffffffffLL)
#define TLFLOAT_FLT256_MIN tlfloat_consto(0x0000100000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL)
#define TLFLOAT_FLT256_DENORM_MIN tlfloat_consto(0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000001LL)
#define TLFLOAT_FLT256_TRUE_MIN tlfloat_consto(0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000001LL)
#define TLFLOAT_FLT256_EPSILON tlfloat_consto(0x3ff1300000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL)

#elif defined(__STDC_VERSION__)

#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
#define TLFLOAT_M_Eo ((tlfloat_octuple_) { 0xa6d2b53c26c8228dLL, 0xa79e3b1738b079c5LL, 0x695355fb8ac404e7LL, 0x400005bf0a8b1457LL })
#define TLFLOAT_M_LOG2Eo ((tlfloat_octuple_) { 0x2b4b1164a2cd9a34LL, 0xa7d11d6aef551badLL, 0xfe1777d0ffda0d23LL, 0x3ffff71547652b82LL })
#define TLFLOAT_M_LOG10Eo ((tlfloat_octuple_) { 0x34404747e5a89ef2LL, 0x7b8647dc68c048b9LL, 0x0e32a6ab7555f5a6LL, 0x3fffdbcb7b1526e5LL })
#define TLFLOAT_M_LN2o ((tlfloat_octuple_) { 0x16c5b141a2eb7175LL, 0x5ed5e81e6864ce53LL, 0xef35793c7673007eLL, 0x3fffe62e42fefa39LL })
#define TLFLOAT_M_LN10o ((tlfloat_octuple_) { 0xbf21d078c3d0403eLL, 0x61451c51fd9f3b4bLL, 0x1582dd4adac5705aLL, 0x4000026bb1bbb555LL })
#define TLFLOAT_M_PIo ((tlfloat_octuple_) { 0xf98e804177d4c762LL, 0x839a252049c1114cLL, 0x18469898cc51701bLL, 0x40000921fb54442dLL })
#define TLFLOAT_M_PI_2o ((tlfloat_octuple_) { 0xf98e804177d4c762LL, 0x839a252049c1114cLL, 0x18469898cc51701bLL, 0x3ffff921fb54442dLL })
#define TLFLOAT_M_PI_4o ((tlfloat_octuple_) { 0xf98e804177d4c762LL, 0x839a252049c1114cLL, 0x18469898cc51701bLL, 0x3fffe921fb54442dLL })
#define TLFLOAT_M_1_PIo ((tlfloat_octuple_) { 0x72083fca2c757bd7LL, 0x9bb81b6c52b32788LL, 0x82a53f84eafa3ea6LL, 0x3fffd45f306dc9c8LL })
#define TLFLOAT_M_2_PIo ((tlfloat_octuple_) { 0x72083fca2c757bd7LL, 0x9bb81b6c52b32788LL, 0x82a53f84eafa3ea6LL, 0x3fffe45f306dc9c8LL })
#define TLFLOAT_M_2_SQRTPIo ((tlfloat_octuple_) { 0xcea2e7342b06199dLL, 0xd8688281341d7587LL, 0x6d11ae3a914fed7fLL, 0x3ffff20dd750429bLL })
#define TLFLOAT_M_SQRT2o ((tlfloat_octuple_) { 0x75099da2f590b066LL, 0x57d3e3adec175127LL, 0xcc908b2fb1366ea9LL, 0x3ffff6a09e667f3bLL })
#define TLFLOAT_M_SQRT1_2o ((tlfloat_octuple_) { 0x75099da2f590b066LL, 0x57d3e3adec175127LL, 0xcc908b2fb1366ea9LL, 0x3fffe6a09e667f3bLL })
#define TLFLOAT_FLT256_MAX ((tlfloat_octuple_) { 0xffffffffffffffffLL, 0xffffffffffffffffLL, 0xffffffffffffffffLL, 0x7fffefffffffffffLL })
#define TLFLOAT_FLT256_MIN ((tlfloat_octuple_) { 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000100000000000LL })
#define TLFLOAT_FLT256_DENORM_MIN ((tlfloat_octuple_) { 0x0000000000000001LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL })
#define TLFLOAT_FLT256_TRUE_MIN ((tlfloat_octuple_) { 0x0000000000000001LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL })
#define TLFLOAT_FLT256_EPSILON ((tlfloat_octuple_) { 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x3ff1300000000000LL })
#else
#define TLFLOAT_M_Eo ((tlfloat_octuple_) { 0x400005bf0a8b1457LL, 0x695355fb8ac404e7LL, 0xa79e3b1738b079c5LL, 0xa6d2b53c26c8228dLL })
#define TLFLOAT_M_LOG2Eo ((tlfloat_octuple_) { 0x3ffff71547652b82LL, 0xfe1777d0ffda0d23LL, 0xa7d11d6aef551badLL, 0x2b4b1164a2cd9a34LL })
#define TLFLOAT_M_LOG10Eo ((tlfloat_octuple_) { 0x3fffdbcb7b1526e5LL, 0x0e32a6ab7555f5a6LL, 0x7b8647dc68c048b9LL, 0x34404747e5a89ef2LL })
#define TLFLOAT_M_LN2o ((tlfloat_octuple_) { 0x3fffe62e42fefa39LL, 0xef35793c7673007eLL, 0x5ed5e81e6864ce53LL, 0x16c5b141a2eb7175LL })
#define TLFLOAT_M_LN10o ((tlfloat_octuple_) { 0x4000026bb1bbb555LL, 0x1582dd4adac5705aLL, 0x61451c51fd9f3b4bLL, 0xbf21d078c3d0403eLL })
#define TLFLOAT_M_PIo ((tlfloat_octuple_) { 0x40000921fb54442dLL, 0x18469898cc51701bLL, 0x839a252049c1114cLL, 0xf98e804177d4c762LL })
#define TLFLOAT_M_PI_2o ((tlfloat_octuple_) { 0x3ffff921fb54442dLL, 0x18469898cc51701bLL, 0x839a252049c1114cLL, 0xf98e804177d4c762LL })
#define TLFLOAT_M_PI_4o ((tlfloat_octuple_) { 0x3fffe921fb54442dLL, 0x18469898cc51701bLL, 0x839a252049c1114cLL, 0xf98e804177d4c762LL })
#define TLFLOAT_M_1_PIo ((tlfloat_octuple_) { 0x3fffd45f306dc9c8LL, 0x82a53f84eafa3ea6LL, 0x9bb81b6c52b32788LL, 0x72083fca2c757bd7LL })
#define TLFLOAT_M_2_PIo ((tlfloat_octuple_) { 0x3fffe45f306dc9c8LL, 0x82a53f84eafa3ea6LL, 0x9bb81b6c52b32788LL, 0x72083fca2c757bd7LL })
#define TLFLOAT_M_2_SQRTPIo ((tlfloat_octuple_) { 0x3ffff20dd750429bLL, 0x6d11ae3a914fed7fLL, 0xd8688281341d7587LL, 0xcea2e7342b06199dLL })
#define TLFLOAT_M_SQRT2o ((tlfloat_octuple_) { 0x3ffff6a09e667f3bLL, 0xcc908b2fb1366ea9LL, 0x57d3e3adec175127LL, 0x75099da2f590b066LL })
#define TLFLOAT_M_SQRT1_2o ((tlfloat_octuple_) { 0x3fffe6a09e667f3bLL, 0xcc908b2fb1366ea9LL, 0x57d3e3adec175127LL, 0x75099da2f590b066LL })
#define TLFLOAT_FLT256_MAX ((tlfloat_octuple_) { 0x7fffefffffffffffLL, 0xffffffffffffffffLL, 0xffffffffffffffffLL, 0xffffffffffffffffLL })
#define TLFLOAT_FLT256_MIN ((tlfloat_octuple_) { 0x0000100000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL })
#define TLFLOAT_FLT256_DENORM_MIN ((tlfloat_octuple_) { 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000001LL })
#define TLFLOAT_FLT256_TRUE_MIN ((tlfloat_octuple_) { 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000001LL })
#define TLFLOAT_FLT256_EPSILON ((tlfloat_octuple_) { 0x3ff1300000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL })
#endif

#endif

#if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128) && !defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128)) || defined(TLFLOAT_DOXYGEN)

inline tlfloat_quad::tlfloat_quad(const struct tlfloat_octuple& v) : value(tlfloat_cast_q_o(v)) {}
inline tlfloat_quad::tlfloat_quad(const tlfloat_int128_t_& i) : value(tlfloat_cast_q_i128(i)) {}
inline tlfloat_quad::tlfloat_quad(const tlfloat_uint128_t_& u) : value(tlfloat_cast_q_u128(u)) {}
inline tlfloat_quad::operator tlfloat_int128_t_() const { return tlfloat_cast_i128_q(value); }
inline tlfloat_quad::operator tlfloat_uint128_t_() const { return tlfloat_cast_u128_q(value); }

#if !defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
inline tlfloat_quad::tlfloat_quad(const struct tlfloat_int128_t& i) : value(tlfloat_cast_q_i128(i)) {}
inline tlfloat_quad::tlfloat_quad(const struct tlfloat_uint128_t& u) : value(tlfloat_cast_q_u128(u)) {}
#endif

/** This function casts a octuple-precision FP number to a double-precision FP number. Link with -ltlfloat. */
static inline double tlfloat_cast_d_q(const tlfloat_quad x) { return tlfloat_cast_d_q(tlfloat_quad_(x)); }
/** This function casts a quadruple-precision FP number to a octuple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_cast_o_q(const tlfloat_quad x) { return tlfloat_cast_o_q(tlfloat_quad_(x)); }
  /** This function casts a quadruple-precision FP number to a 64-bit signed integer. Link with -ltlfloat. */
static inline int64_t tlfloat_cast_i64_q(const tlfloat_quad x) { return tlfloat_cast_i64_q(tlfloat_quad_(x)); }
  /** This function casts a quadruple-precision FP number to a 64-bit unsigned integer. Link with -ltlfloat. */
static inline uint64_t tlfloat_cast_u64_q(const tlfloat_quad x){ return tlfloat_cast_u64_q(tlfloat_quad_(x)); }

/** This function performs addition of two quadruple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_addq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_addq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs subtraction of two quadruple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_subq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_subq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs multiplication of two quadruple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_mulq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_mulq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs division of two quadruple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_divq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_divq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_negq(const tlfloat_quad x) { return tlfloat_negq(tlfloat_quad_(x)); }

/** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_eq_q_q(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_eq_q_q(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_ne_q_q(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_ne_q_q(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_lt_q_q(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_lt_q_q(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_le_q_q(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_le_q_q(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_gt_q_q(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_gt_q_q(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_ge_q_q(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_ge_q_q(tlfloat_quad_(x), tlfloat_quad_(y)); }

/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_fmaq(const tlfloat_quad x, const tlfloat_quad y, const tlfloat_quad z) { return tlfloat_fmaq(tlfloat_quad_(x), tlfloat_quad_(y), tlfloat_quad_(z)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_sqrtq(const tlfloat_quad x) { return tlfloat_sqrtq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_fabsq(const tlfloat_quad x) { return tlfloat_fabsq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_copysignq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_copysignq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_fmaxq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmaxq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_fminq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fminq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_fdimq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fdimq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_ldexpq(const tlfloat_quad x, const int y) { return tlfloat_ldexpq(tlfloat_quad_(x), y); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_frexpq(const tlfloat_quad x, int *y) { return tlfloat_frexpq(tlfloat_quad_(x), y); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_modfq(const tlfloat_quad x, tlfloat_quad *y) { return tlfloat_modfq(tlfloat_quad_(x), (tlfloat_quad_ *)y); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_nextafterq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_nextafterq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_ilogbq(const tlfloat_quad x) { return tlfloat_ilogbq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_isnanq(const tlfloat_quad x) { return tlfloat_isnanq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_isinfq(const tlfloat_quad x) { return tlfloat_isinfq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_finiteq(const tlfloat_quad x) { return tlfloat_finiteq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_fpclassifyq(const tlfloat_quad x) { return tlfloat_fpclassifyq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_signbitq(const tlfloat_quad x) { return tlfloat_signbitq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_hypotq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_hypotq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_truncq(const tlfloat_quad x) { return tlfloat_truncq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_floorq(const tlfloat_quad x) { return tlfloat_floorq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_ceilq(const tlfloat_quad x) { return tlfloat_ceilq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_roundq(const tlfloat_quad x) { return tlfloat_roundq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_rintq(const tlfloat_quad x) { return tlfloat_rintq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_sinq(const tlfloat_quad x) { return tlfloat_sinq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_cosq(const tlfloat_quad x) { return tlfloat_cosq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_tanq(const tlfloat_quad x) { return tlfloat_tanq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_sinpiq(const tlfloat_quad x) { return tlfloat_sinpiq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_cospiq(const tlfloat_quad x) { return tlfloat_cospiq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_tanpiq(const tlfloat_quad x) { return tlfloat_tanpiq(tlfloat_quad_(x)); }
static inline void tlfloat_sincosq(const tlfloat_quad x, tlfloat_quad *s, tlfloat_quad *c) {
  tlfloat_quad_ s_, c_;
  tlfloat_sincosq(tlfloat_quad_(x), &s_, &c_);
  *s = tlfloat_quad(s_); *c = tlfloat_quad(c_);
}
static inline void tlfloat_sincospiq(const tlfloat_quad x, tlfloat_quad *s, tlfloat_quad *c) {
  tlfloat_quad_ s_, c_;
  tlfloat_sincospiq(tlfloat_quad_(x), &s_, &c_);
  *s = tlfloat_quad(s_); *c = tlfloat_quad(c_);
}
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_asinq(const tlfloat_quad x) { return tlfloat_asinq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_acosq(const tlfloat_quad x) { return tlfloat_acosq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_atanq(const tlfloat_quad x) { return tlfloat_atanq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_atan2q(const tlfloat_quad y, const tlfloat_quad x) { return tlfloat_atan2q(tlfloat_quad_(y), tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_expq(const tlfloat_quad x) { return tlfloat_expq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_expm1q(const tlfloat_quad x) { return tlfloat_expm1q(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_exp2q(const tlfloat_quad x) { return tlfloat_exp2q(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_exp10q(const tlfloat_quad x) { return tlfloat_exp10q(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_logq(const tlfloat_quad x) { return tlfloat_logq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_log1pq(const tlfloat_quad x) { return tlfloat_log1pq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_log2q(const tlfloat_quad x) { return tlfloat_log2q(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_log10q(const tlfloat_quad x) { return tlfloat_log10q(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_powq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_powq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_cbrtq(const tlfloat_quad x) { return tlfloat_cbrtq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_sinhq(const tlfloat_quad x) { return tlfloat_sinhq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_coshq(const tlfloat_quad x) { return tlfloat_coshq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_tanhq(const tlfloat_quad x) { return tlfloat_tanhq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_asinhq(const tlfloat_quad x) { return tlfloat_asinhq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_acoshq(const tlfloat_quad x) { return tlfloat_acoshq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_atanhq(const tlfloat_quad x) { return tlfloat_atanhq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_erfq(const tlfloat_quad x) { return tlfloat_erfq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_erfcq(const tlfloat_quad x) { return tlfloat_erfcq(tlfloat_quad_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_tgammaq(const tlfloat_quad x) { return tlfloat_tgammaq(tlfloat_quad_(x)); }
/** This is experimental implementation of log gamma function. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_lgammaq(const tlfloat_quad x) { return tlfloat_cast_q_o(tlfloat_lgammao(tlfloat_cast_o_q(tlfloat_quad_(x)))); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_fmodq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmodq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_remainderq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_remainderq(tlfloat_quad_(x), tlfloat_quad_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_remquoq(const tlfloat_quad x, const tlfloat_quad y, int *quo) { return tlfloat_remquoq(tlfloat_quad_(x), tlfloat_quad_(y), quo); }

#endif // #if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128) && !defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128)) || defined(TLFLOAT_DOXYGEN)

/** This function casts a double-precision FP number to a quadruple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_cast_q_d(const double x) { return tlfloat_cast_q_d_(x); }
/** This function casts a 64-bit signed integer to a quadruple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_cast_q_i64(const int64_t x) { return tlfloat_cast_q_i64_(x); }
/** This function casts a 64-bit unsigned integer to a quadruple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_cast_q_u64(const uint64_t x) { return tlfloat_cast_q_u64_(x); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_strtoq(const char *nptr, const char **endptr) { return tlfloat_strtoq_(nptr, endptr); }

#ifdef TLFLOAT_LIBQUADMATH_EMULATION

#if defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) || defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128) || defined(TLFLOAT_COMPILER_SUPPORTS_ISOFLOAT128) || defined(__STDC_VERSION__) || defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_Eq TLFLOAT_M_Eq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_LOG2Eq TLFLOAT_M_LOG2Eq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_LOG10Eq TLFLOAT_M_LOG10Eq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_LN2q TLFLOAT_M_LN2q
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_LN10q TLFLOAT_M_LN10q
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_PIq TLFLOAT_M_PIq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_PI_2q TLFLOAT_M_PI_2q
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_PI_4q TLFLOAT_M_PI_4q
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_1_PIq TLFLOAT_M_1_PIq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_2_PIq TLFLOAT_M_2_PIq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_2_SQRTPIq TLFLOAT_M_2_SQRTPIq
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_SQRT2q TLFLOAT_M_SQRT2q
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define M_SQRT1_2q TLFLOAT_M_SQRT1_2q
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MAX TLFLOAT_FLT128_MAX
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MIN TLFLOAT_FLT128_MIN
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_DENORM_MIN TLFLOAT_FLT128_DENORM_MIN
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_EPSILON TLFLOAT_FLT128_EPSILON
#endif

/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MANT_DIG TLFLOAT_FLT128_MANT_DIG
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MIN_EXP TLFLOAT_FLT128_MIN_EXP
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MAX_EXP TLFLOAT_FLT128_MAX_EXP
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_DIG TLFLOAT_FLT128_DIG
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MIN_10_EXP TLFLOAT_FLT128_MIN_10_EXP
/** This macro is defined only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined. */
#define FLT128_MAX_10_EXP TLFLOAT_FLT128_MAX_10_EXP

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad strtoflt128(const char *s, const char **sp) {
    return tlfloat_strtoq_(s, sp);
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
  static inline tlfloat_quad fmaq(const tlfloat_quad x, const tlfloat_quad y, const tlfloat_quad z) { return tlfloat_fmaq(x, y, z); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad sqrtq(const tlfloat_quad x) { return tlfloat_sqrtq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad fabsq(const tlfloat_quad x) { return tlfloat_fabsq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad copysignq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_copysignq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad fmaxq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmaxq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad fminq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fminq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad fdimq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fdimq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad ldexpq(const tlfloat_quad x, const int y) { return tlfloat_ldexpq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad frexpq(const tlfloat_quad x, int *y) { return tlfloat_frexpq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad modfq(const tlfloat_quad x, tlfloat_quad *y) { return tlfloat_modfq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad nextafterq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_nextafterq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int ilogbq(const tlfloat_quad x) { return tlfloat_ilogbq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int isnanq(const tlfloat_quad x) { return tlfloat_isnanq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int isinfq(const tlfloat_quad x) { return tlfloat_isinfq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int finiteq(const tlfloat_quad x) { return tlfloat_finiteq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int fpclassifyq(const tlfloat_quad x) { return tlfloat_fpclassifyq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline int signbitq(const tlfloat_quad x) { return tlfloat_signbitq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad hypotq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_hypotq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad truncq(const tlfloat_quad x) { return tlfloat_truncq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad floorq(const tlfloat_quad x) { return tlfloat_floorq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad ceilq(const tlfloat_quad x) { return tlfloat_ceilq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad roundq(const tlfloat_quad x) { return tlfloat_roundq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad rintq(const tlfloat_quad x) { return tlfloat_rintq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad sinq(const tlfloat_quad x) { return tlfloat_sinq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad cosq(const tlfloat_quad x) { return tlfloat_cosq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline void sincosq(const tlfloat_quad x, tlfloat_quad *s, tlfloat_quad *c) { tlfloat_sincosq(x, s, c); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad tanq(const tlfloat_quad x) { return tlfloat_tanq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad asinq(const tlfloat_quad x) { return tlfloat_asinq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad acosq(const tlfloat_quad x) { return tlfloat_acosq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad atanq(const tlfloat_quad x) { return tlfloat_atanq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad atan2q(const tlfloat_quad y, const tlfloat_quad x) { return tlfloat_atan2q(y, x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad expq(const tlfloat_quad x) { return tlfloat_expq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad expm1q(const tlfloat_quad x) { return tlfloat_expm1q(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad exp2q(const tlfloat_quad x) { return tlfloat_exp2q(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad exp10q(const tlfloat_quad x) { return tlfloat_exp10q(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad logq(const tlfloat_quad x) { return tlfloat_logq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad log1pq(const tlfloat_quad x) { return tlfloat_log1pq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad log2q(const tlfloat_quad x) { return tlfloat_log2q(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad log10q(const tlfloat_quad x) { return tlfloat_log10q(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad powq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_powq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad cbrtq(const tlfloat_quad x) { return tlfloat_cbrtq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad sinhq(const tlfloat_quad x) { return tlfloat_sinhq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad coshq(const tlfloat_quad x) { return tlfloat_coshq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad tanhq(const tlfloat_quad x) { return tlfloat_tanhq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad asinhq(const tlfloat_quad x) { return tlfloat_asinhq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad acoshq(const tlfloat_quad x) { return tlfloat_acoshq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad atanhq(const tlfloat_quad x) { return tlfloat_atanhq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad erfq(const tlfloat_quad x) { return tlfloat_erfq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad erfcq(const tlfloat_quad x) { return tlfloat_erfcq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad tgammaq(const tlfloat_quad x) { return tlfloat_tgammaq(x); }

  /** This is experimental implementation of log gamma function. 
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad lgammaq(const tlfloat_quad x) { return tlfloat_lgammaq(x); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad fmodq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmodq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad remainderq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_remainderq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad remquoq(const tlfloat_quad x, const tlfloat_quad y, int *quo) { return tlfloat_remquoq(x, y, quo); }
#endif // #ifdef TLFLOAT_LIBQUADMATH_EMULATION

//

#if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
#if !defined(TLFLOAT_DOXYGEN)
#define TLFLOAT_OVERLOAD_OP2(OP, OPA)					\
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline tlfloat_octuple operator OP(const T& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  static inline tlfloat_octuple operator OP(const tlfloat_int128_t& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(tlfloat_cast_o_i128(lhs)) OP rhs; } \
  static inline tlfloat_octuple operator OP(const tlfloat_uint128_t& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(tlfloat_cast_o_u128(lhs)) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline tlfloat_octuple operator OP(const tlfloat_octuple& lhs, const T& rhs) { return lhs OP tlfloat_octuple(rhs); } \
  static inline tlfloat_octuple operator OP(const tlfloat_octuple& lhs, const tlfloat_int128_t& rhs) { return lhs OP tlfloat_cast_o_i128(rhs); } \
  static inline tlfloat_octuple operator OP(const tlfloat_octuple& lhs, const tlfloat_uint128_t& rhs) { return lhs OP tlfloat_cast_o_u128(rhs); } \
  static inline tlfloat_octuple operator OP(const tlfloat_quad& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  static inline tlfloat_octuple operator OP(const tlfloat_octuple& lhs, const tlfloat_quad& rhs) { return lhs OP tlfloat_octuple(rhs); } \
  template<typename rhstype>						\
  static inline tlfloat_octuple& operator OPA(tlfloat_octuple& lhs, const rhstype& rhs) { return (lhs = lhs OP tlfloat_octuple(rhs)); } \
  static_assert(true, "")

#define TLFLOAT_OVERLOAD_OP2Q(OP, OPA)					\
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline tlfloat_quad operator OP(const T& lhs, const tlfloat_quad& rhs) { return tlfloat_quad(lhs) OP rhs; } \
  static inline tlfloat_quad operator OP(const tlfloat_int128_t& lhs, const tlfloat_quad& rhs) { return tlfloat_quad(tlfloat_cast_q_i128(lhs)) OP rhs; } \
  static inline tlfloat_quad operator OP(const tlfloat_uint128_t& lhs, const tlfloat_quad& rhs) { return tlfloat_quad(tlfloat_cast_q_u128(lhs)) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline tlfloat_quad operator OP(const tlfloat_quad& lhs, const T& rhs) { return lhs OP tlfloat_quad(rhs); } \
  static inline tlfloat_quad operator OP(const tlfloat_quad& lhs, const tlfloat_int128_t& rhs) { return lhs OP tlfloat_cast_q_i128(rhs); } \
  static inline tlfloat_quad operator OP(const tlfloat_quad& lhs, const tlfloat_uint128_t& rhs) { return lhs OP tlfloat_cast_q_u128(rhs); } \
  template<typename rhstype>						\
  static inline tlfloat_quad& operator OPA(tlfloat_quad& lhs, const rhstype& rhs) { return (lhs = lhs OP tlfloat_quad(rhs)); } \
  static_assert(true, "")

#define TLFLOAT_OVERLOAD_OP2I(OP, OPA)					\
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline tlfloat_int128_t operator OP(const T& lhs, const tlfloat_int128_t& rhs) { return tlfloat_int128_t(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline tlfloat_uint128_t operator OP(const T& lhs, const tlfloat_uint128_t& rhs) { return tlfloat_uint128_t(lhs) OP rhs; } \
  static inline tlfloat_int128_t operator OP(const tlfloat_int128_t& lhs, const tlfloat_int128_t_& rhs) { return lhs OP (tlfloat_int128_t)rhs; } \
  static inline tlfloat_int128_t operator OP(const tlfloat_int128_t_& lhs, const tlfloat_int128_t& rhs) { return (tlfloat_int128_t)lhs OP rhs; } \
  static inline tlfloat_uint128_t operator OP(const tlfloat_uint128_t& lhs, const tlfloat_uint128_t_& rhs) { return lhs OP (tlfloat_uint128_t)rhs; } \
  static inline tlfloat_uint128_t operator OP(const tlfloat_uint128_t_& lhs, const tlfloat_uint128_t& rhs) { return (tlfloat_uint128_t)lhs OP rhs; } \
  template<typename rhstype>						\
  static inline tlfloat_int128_t& operator OPA(tlfloat_int128_t& lhs, const rhstype& rhs) { return (lhs = lhs OP tlfloat_int128_t(rhs)); } \
  template<typename rhstype>						\
  static inline tlfloat_uint128_t& operator OPA(tlfloat_uint128_t& lhs, const rhstype& rhs) { return (lhs = lhs OP tlfloat_uint128_t(rhs)); } \
  static_assert(true, "")

#define TLFLOAT_OVERLOAD_CMP(OP)					\
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline bool operator OP(const T& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  static inline bool operator OP(const tlfloat_int128_t& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(tlfloat_cast_o_i128(lhs)) OP rhs; } \
  static inline bool operator OP(const tlfloat_uint128_t& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(tlfloat_cast_o_u128(lhs)) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline bool operator OP(const tlfloat_octuple& lhs, const T& rhs) { return lhs OP tlfloat_octuple(rhs); } \
  static inline bool operator OP(const tlfloat_octuple& lhs, const tlfloat_int128_t& rhs) { return lhs OP tlfloat_octuple(tlfloat_cast_o_i128(rhs)); } \
  static inline bool operator OP(const tlfloat_octuple& lhs, const tlfloat_uint128_t& rhs) { return lhs OP tlfloat_octuple(tlfloat_cast_o_u128(rhs)); } \
  static inline bool operator OP(const tlfloat_octuple& lhs, const tlfloat_quad& rhs) { return lhs OP tlfloat_octuple(rhs); } \
  static inline bool operator OP(const tlfloat_quad& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  static_assert(true, "")

#define TLFLOAT_OVERLOAD_CMPQ(OP)					\
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline bool operator OP(const T& lhs, const tlfloat_quad& rhs) { return tlfloat_quad(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 8) || (std::is_integral<T>::value && sizeof(T) <= 8)), int>::type = 0> \
  static inline bool operator OP(const tlfloat_quad& lhs, const T& rhs) { return lhs OP tlfloat_quad(rhs); } \
  static_assert(true, "")

#define TLFLOAT_OVERLOAD_CMPI(OP)					\
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) <= 8), int>::type = 0> \
  static inline bool operator OP(const T& lhs, const tlfloat_int128_t& rhs) { return tlfloat_int128_t(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) <= 8), int>::type = 0> \
  static inline bool operator OP(const tlfloat_int128_t& lhs, const T& rhs) { return lhs OP tlfloat_int128_t(rhs); } \
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) <= 8), int>::type = 0> \
  static inline bool operator OP(const T& lhs, const tlfloat_uint128_t& rhs) { return tlfloat_uint128_t(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) <= 8), int>::type = 0> \
  static inline bool operator OP(const tlfloat_uint128_t& lhs, const T& rhs) { return lhs OP tlfloat_uint128_t(rhs); } \
  static inline bool operator OP(const tlfloat_int128_t& lhs, const tlfloat_int128_t_& rhs) { return lhs OP (tlfloat_int128_t)rhs; } \
  static inline bool operator OP(const tlfloat_int128_t_& lhs, const tlfloat_int128_t& rhs) { return (tlfloat_int128_t)lhs OP rhs; } \
  static inline bool operator OP(const tlfloat_uint128_t& lhs, const tlfloat_uint128_t_& rhs) { return lhs OP (tlfloat_uint128_t)rhs; } \
  static inline bool operator OP(const tlfloat_uint128_t_& lhs, const tlfloat_uint128_t& rhs) { return (tlfloat_uint128_t)lhs OP rhs; } \
  static_assert(true, "")

TLFLOAT_OVERLOAD_OP2(+, +=);
TLFLOAT_OVERLOAD_OP2(-, -=);
TLFLOAT_OVERLOAD_OP2(*, *=);
TLFLOAT_OVERLOAD_OP2(/, /=);
TLFLOAT_OVERLOAD_CMP(==);
TLFLOAT_OVERLOAD_CMP(!=);
TLFLOAT_OVERLOAD_CMP(<);
TLFLOAT_OVERLOAD_CMP(<=);
TLFLOAT_OVERLOAD_CMP(>);
TLFLOAT_OVERLOAD_CMP(>=);

#ifdef TLFLOAT_QUAD_IS_STRUCT
TLFLOAT_OVERLOAD_OP2Q(+, +=);
TLFLOAT_OVERLOAD_OP2Q(-, -=);
TLFLOAT_OVERLOAD_OP2Q(*, *=);
TLFLOAT_OVERLOAD_OP2Q(/, /=);
TLFLOAT_OVERLOAD_CMPQ(==);
TLFLOAT_OVERLOAD_CMPQ(!=);
TLFLOAT_OVERLOAD_CMPQ(<);
TLFLOAT_OVERLOAD_CMPQ(<=);
TLFLOAT_OVERLOAD_CMPQ(>);
TLFLOAT_OVERLOAD_CMPQ(>=);
#endif

#ifdef TLFLOAT_INT128_IS_STRUCT
TLFLOAT_OVERLOAD_OP2I(+, +=);
TLFLOAT_OVERLOAD_OP2I(-, -=);
TLFLOAT_OVERLOAD_OP2I(*, *=);
TLFLOAT_OVERLOAD_OP2I(/, /=);
TLFLOAT_OVERLOAD_CMPI(==);
TLFLOAT_OVERLOAD_CMPI(!=);
TLFLOAT_OVERLOAD_CMPI(<);
TLFLOAT_OVERLOAD_CMPI(<=);
TLFLOAT_OVERLOAD_CMPI(>);
TLFLOAT_OVERLOAD_CMPI(>=);
#endif

#undef TLFLOAT_OVERLOAD_OP2
#undef TLFLOAT_OVERLOAD_OP2Q
#undef TLFLOAT_OVERLOAD_OP2I
#undef TLFLOAT_OVERLOAD_CMP
#undef TLFLOAT_OVERLOAD_CMPQ
#undef TLFLOAT_OVERLOAD_CMPI
#endif // #if !defined(TLFLOAT_DOXYGEN)

/** This function casts a octuple-precision FP number to a double-precision FP number. Link with -ltlfloat. */
static inline double tlfloat_cast_d_o(const tlfloat_octuple x) { return tlfloat_cast_d_o(tlfloat_octuple_(x)); }
/** This function casts a octuple-precision FP number to a quadruple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_quad tlfloat_cast_q_o(const tlfloat_octuple x) { return tlfloat_cast_q_o(tlfloat_octuple_(x)); }
 /** This function casts a octuple-precision FP number to a 64-bit signed integer. Link with -ltlfloat. */
static inline int64_t tlfloat_cast_i64_o(const tlfloat_octuple x) { return tlfloat_cast_i64_o(tlfloat_octuple_(x)); }
 /** This function casts a octuple-precision FP number to a 64-bit unsigned integer. Link with -ltlfloat. */
static inline uint64_t tlfloat_cast_u64_o(const tlfloat_octuple x) { return tlfloat_cast_u64_o(tlfloat_octuple_(x)); }

/** This function performs addition of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_addo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_addo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs subtraction of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_subo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_subo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs multiplication of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_mulo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_mulo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs division of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_divo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_divo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_nego(const tlfloat_octuple x) { return tlfloat_nego(tlfloat_octuple_(x)); }

/** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_eq_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_eq_o_o(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_ne_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_ne_o_o(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_lt_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_lt_o_o(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_le_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_le_o_o(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_gt_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_gt_o_o(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
static inline int tlfloat_ge_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_ge_o_o(tlfloat_octuple_(x), tlfloat_octuple_(y)); }

/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_fmao(const tlfloat_octuple x, const tlfloat_octuple y, const tlfloat_octuple z) { return tlfloat_fmao(tlfloat_octuple_(x), tlfloat_octuple_(y), tlfloat_octuple_(z)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_sqrto(const tlfloat_octuple x) { return tlfloat_sqrto(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_fabso(const tlfloat_octuple x) { return tlfloat_fabso(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_copysigno(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_copysigno(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_fmaxo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fmaxo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_fmino(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fmino(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_fdimo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fdimo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_ldexpo(const tlfloat_octuple x, const int y) { return tlfloat_ldexpo(tlfloat_octuple_(x), y); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_frexpo(const tlfloat_octuple x, int *y) { return tlfloat_frexpo(tlfloat_octuple_(x), y); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_modfo(const tlfloat_octuple x, tlfloat_octuple *y) { return tlfloat_modfo(tlfloat_octuple_(x), (tlfloat_octuple_ *)y); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_nextaftero(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_nextaftero(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_ilogbo(const tlfloat_octuple x) { return tlfloat_ilogbo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_isnano(const tlfloat_octuple x) { return tlfloat_isnano(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_isinfo(const tlfloat_octuple x) { return tlfloat_isinfo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_finiteo(const tlfloat_octuple x) { return tlfloat_finiteo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline int tlfloat_signbito(const tlfloat_octuple x) { return tlfloat_signbito(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_hypoto(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_hypoto(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_trunco(const tlfloat_octuple x) { return tlfloat_trunco(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_flooro(const tlfloat_octuple x) { return tlfloat_flooro(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_ceilo(const tlfloat_octuple x) { return tlfloat_ceilo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_roundo(const tlfloat_octuple x) { return tlfloat_roundo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_rinto(const tlfloat_octuple x) { return tlfloat_rinto(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_sino(const tlfloat_octuple x) { return tlfloat_sino(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_coso(const tlfloat_octuple x) { return tlfloat_coso(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_tano(const tlfloat_octuple x) { return tlfloat_tano(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_sinpio(const tlfloat_octuple x) { return tlfloat_sinpio(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_cospio(const tlfloat_octuple x) { return tlfloat_cospio(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_tanpio(const tlfloat_octuple x) { return tlfloat_tanpio(tlfloat_octuple_(x)); }
static inline void tlfloat_sincoso(const tlfloat_octuple x, tlfloat_octuple *s, tlfloat_octuple *c) {
  tlfloat_octuple_ s_, c_;
  tlfloat_sincoso(tlfloat_octuple_(x), &s_, &c_);
  *s = tlfloat_octuple(s_); *c = tlfloat_octuple(c_);
}
static inline void tlfloat_sincospio(const tlfloat_octuple x, tlfloat_octuple *s, tlfloat_octuple *c) {
  tlfloat_octuple_ s_, c_;
  tlfloat_sincospio(tlfloat_octuple_(x), &s_, &c_);
  *s = tlfloat_octuple(s_); *c = tlfloat_octuple(c_);
}
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_asino(const tlfloat_octuple x) { return tlfloat_asino(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_acoso(const tlfloat_octuple x) { return tlfloat_acoso(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_atano(const tlfloat_octuple x) { return tlfloat_atano(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_atan2o(const tlfloat_octuple y, const tlfloat_octuple x) { return tlfloat_atan2o(tlfloat_octuple_(y), tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_expo(const tlfloat_octuple x) { return tlfloat_expo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_expm1o(const tlfloat_octuple x) { return tlfloat_expm1o(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_exp2o(const tlfloat_octuple x) { return tlfloat_exp2o(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_exp10o(const tlfloat_octuple x) { return tlfloat_exp10o(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_logo(const tlfloat_octuple x) { return tlfloat_logo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_log1po(const tlfloat_octuple x) { return tlfloat_log1po(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_log2o(const tlfloat_octuple x) { return tlfloat_log2o(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_log10o(const tlfloat_octuple x) { return tlfloat_log10o(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_powo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_powo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_cbrto(const tlfloat_octuple x) { return tlfloat_cbrto(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_sinho(const tlfloat_octuple x) { return tlfloat_sinho(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_cosho(const tlfloat_octuple x) { return tlfloat_cosho(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_tanho(const tlfloat_octuple x) { return tlfloat_tanho(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_asinho(const tlfloat_octuple x) { return tlfloat_asinho(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_acosho(const tlfloat_octuple x) { return tlfloat_acosho(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_atanho(const tlfloat_octuple x) { return tlfloat_atanho(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_erfo(const tlfloat_octuple x) { return tlfloat_erfo(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_erfco(const tlfloat_octuple x) { return tlfloat_erfco(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_tgammao(const tlfloat_octuple x) { return tlfloat_tgammao(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_lgammao(const tlfloat_octuple x) { return tlfloat_lgammao(tlfloat_octuple_(x)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_fmodo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fmodo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_remaindero(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_remaindero(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_remquoo(const tlfloat_octuple x, const tlfloat_octuple y, int *quo) { return tlfloat_remquoo(tlfloat_octuple_(x), tlfloat_octuple_(y), quo); }

namespace tlfloat {
#ifndef TLFLOAT_NO_LIBSTDCXX
  static inline std::string to_string(const float a, int d=6) {
    std::vector<char> buf(1000);
    tlfloat_snprintf(buf.data(), buf.size(), "%.*g", d, (double)a);
    return std::string(buf.data());
  }

  static inline std::string to_string(const double a, int d=6) {
    std::vector<char> buf(1000);
    tlfloat_snprintf(buf.data(), buf.size(), "%.*g", d, a);
    return std::string(buf.data());
  }

  static inline std::string to_string(const tlfloat_quad& a, int d=6) {
    std::vector<char> buf(1000);
    tlfloat_snprintf(buf.data(), buf.size(), "%.*Qg", d, a);
    return std::string(buf.data());
  }

  static inline std::string to_string(const tlfloat_octuple& a, int d=6) {
    std::vector<char> buf(1000);
    tlfloat_snprintf(buf.data(), buf.size(), "%.*Og", d, a);
    return std::string(buf.data());
  }

  static inline std::string to_string(const tlfloat_int128_t& a) {
    std::vector<char> buf(1000);
    tlfloat_snprintf(buf.data(), buf.size(), "%Qd", a);
    return std::string(buf.data());
  }

  static inline std::string to_string(const tlfloat_uint128_t& a) {
    std::vector<char> buf(1000);
    tlfloat_snprintf(buf.data(), buf.size(), "%Qu", a);
    return std::string(buf.data());
  }
#endif

  //

#if !defined(TLFLOAT_DOXYGEN)
#define TLFLOAT_POLYFUNC_f_f(OP)					\
  static inline float OP ## _(const float &x) { return tlfloat_ ## OP ## f(x); } \
  static inline double OP ## _(const double &x) { return tlfloat_ ## OP(x); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x) { return tlfloat_ ## OP ## q(x); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x) { return tlfloat_ ## OP ## o(x); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_i_f(OP)					\
  static inline int OP ## _(const float &x) { return tlfloat_ ## OP ## f(x); } \
  static inline int OP ## _(const double &x) { return tlfloat_ ## OP(x); } \
  static inline int OP ## _(const tlfloat_quad &x) { return tlfloat_ ## OP ## q(x); } \
  static inline int OP ## _(const tlfloat_octuple &x) { return tlfloat_ ## OP ## o(x); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_f_f_f(OP)					\
  static inline float OP ## _(const float &x, const float &y) { return tlfloat_ ## OP ## f(x, y); } \
  static inline double OP ## _(const double &x, const double &y) { return tlfloat_ ## OP(x, y); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x, const tlfloat_quad &y) { return tlfloat_ ## OP ## q(x, y); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x, const tlfloat_octuple &y) { return tlfloat_ ## OP ## o(x, y); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_f_f_i(OP)					\
  static inline float OP ## _(const float &x, const int &y) { return tlfloat_ ## OP ## f(x, y); } \
  static inline double OP ## _(const double &x, const int &y) { return tlfloat_ ## OP(x, y); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x, const int &y) { return tlfloat_ ## OP ## q(x, y); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x, const int &y) { return tlfloat_ ## OP ## o(x, y); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_f_f_pf(OP)					\
  static inline float OP ## _(const float &x, float *y) { return tlfloat_ ## OP ## f(x, y); } \
  static inline double OP ## _(const double &x, double *y) { return tlfloat_ ## OP(x, y); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x, tlfloat_quad *y) { return tlfloat_ ## OP ## q(x, y); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x, tlfloat_octuple *y) { return tlfloat_ ## OP ## o(x, y); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_f_f_pi(OP)					\
  static inline float OP ## _(const float &x, int *y) { return tlfloat_ ## OP ## f(x, y); } \
  static inline double OP ## _(const double &x, int *y) { return tlfloat_ ## OP(x, y); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x, int *y) { return tlfloat_ ## OP ## q(x, y); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x, int *y) { return tlfloat_ ## OP ## o(x, y); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_f_f_f_pi(OP)					\
  static inline float OP ## _(const float &x, const float &y, int *z) { return tlfloat_ ## OP ## f(x, y, z); } \
  static inline double OP ## _(const double &x, const double &y, int *z) { return tlfloat_ ## OP(x, y, z); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x, const tlfloat_quad &y, int *z) { return tlfloat_ ## OP ## q(x, y, z); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x, const tlfloat_octuple &y, int *z) { return tlfloat_ ## OP ## o(x, y, z); } \
  static_assert(true, "")

#define TLFLOAT_POLYFUNC_f_f_f_f(OP)					\
  static inline float OP ## _(const float &x, const float &y, const float &z) { return tlfloat_ ## OP ## f(x, y, z); } \
  static inline double OP ## _(const double &x, const double &y, const double &z) { return tlfloat_ ## OP(x, y, z); } \
  static inline tlfloat_quad OP ## _(const tlfloat_quad &x, const tlfloat_quad &y, const tlfloat_quad &z) { return tlfloat_ ## OP ## q(x, y, z); } \
  static inline tlfloat_octuple OP ## _(const tlfloat_octuple &x, const tlfloat_octuple &y, const tlfloat_octuple &z) { return tlfloat_ ## OP ## o(x, y, z); } \
  static_assert(true, "")

  TLFLOAT_POLYFUNC_f_f_f(add);
  TLFLOAT_POLYFUNC_f_f_f(sub);
  TLFLOAT_POLYFUNC_f_f_f(mul);
  TLFLOAT_POLYFUNC_f_f_f(div);
  TLFLOAT_POLYFUNC_f_f(neg);
  TLFLOAT_POLYFUNC_f_f_f_f(fma);
  TLFLOAT_POLYFUNC_f_f(sqrt);
  TLFLOAT_POLYFUNC_f_f(cbrt);
  TLFLOAT_POLYFUNC_f_f(fabs);
  TLFLOAT_POLYFUNC_f_f_f(copysign);
  TLFLOAT_POLYFUNC_f_f_f(fmax);
  TLFLOAT_POLYFUNC_f_f_f(fmin);
  TLFLOAT_POLYFUNC_f_f_f(fdim);
  TLFLOAT_POLYFUNC_f_f_i(ldexp);
  TLFLOAT_POLYFUNC_f_f_pi(frexp);
  TLFLOAT_POLYFUNC_f_f_pf(modf);
  TLFLOAT_POLYFUNC_f_f_f(nextafter);
  TLFLOAT_POLYFUNC_i_f(ilogb);
  TLFLOAT_POLYFUNC_i_f(isnan);
  TLFLOAT_POLYFUNC_i_f(isinf);
  TLFLOAT_POLYFUNC_i_f(finite);
  TLFLOAT_POLYFUNC_i_f(fpclassify);
  TLFLOAT_POLYFUNC_i_f(signbit);
  TLFLOAT_POLYFUNC_f_f_f(hypot);
  TLFLOAT_POLYFUNC_f_f(trunc);
  TLFLOAT_POLYFUNC_f_f(floor);
  TLFLOAT_POLYFUNC_f_f(ceil);
  TLFLOAT_POLYFUNC_f_f(round);
  TLFLOAT_POLYFUNC_f_f(rint);
  TLFLOAT_POLYFUNC_f_f(sin);
  TLFLOAT_POLYFUNC_f_f(cos);
  TLFLOAT_POLYFUNC_f_f(tan);
  TLFLOAT_POLYFUNC_f_f(sinpi);
  TLFLOAT_POLYFUNC_f_f(cospi);
  TLFLOAT_POLYFUNC_f_f(tanpi);
  TLFLOAT_POLYFUNC_f_f(asin);
  TLFLOAT_POLYFUNC_f_f(acos);
  TLFLOAT_POLYFUNC_f_f(atan);
  TLFLOAT_POLYFUNC_f_f_f(atan2);
  TLFLOAT_POLYFUNC_f_f(exp);
  TLFLOAT_POLYFUNC_f_f(expm1);
  TLFLOAT_POLYFUNC_f_f(exp2);
  TLFLOAT_POLYFUNC_f_f(exp10);
  TLFLOAT_POLYFUNC_f_f(log);
  TLFLOAT_POLYFUNC_f_f(log1p);
  TLFLOAT_POLYFUNC_f_f(log2);
  TLFLOAT_POLYFUNC_f_f(log10);
  TLFLOAT_POLYFUNC_f_f_f(pow);
  TLFLOAT_POLYFUNC_f_f(sinh);
  TLFLOAT_POLYFUNC_f_f(cosh);
  TLFLOAT_POLYFUNC_f_f(tanh);
  TLFLOAT_POLYFUNC_f_f(asinh);
  TLFLOAT_POLYFUNC_f_f(acosh);
  TLFLOAT_POLYFUNC_f_f(atanh);
  TLFLOAT_POLYFUNC_f_f_f(fmod);
  TLFLOAT_POLYFUNC_f_f_f(remainder);
  TLFLOAT_POLYFUNC_f_f_f_pi(remquo);
  TLFLOAT_POLYFUNC_f_f(erf);
  TLFLOAT_POLYFUNC_f_f(erfc);
  TLFLOAT_POLYFUNC_f_f(tgamma);
  TLFLOAT_POLYFUNC_f_f(lgamma);

#undef TLFLOAT_POLYFUNC_f_f_f_f
#undef TLFLOAT_POLYFUNC_f_f_f_pi
#undef TLFLOAT_POLYFUNC_f_f_pi
#undef TLFLOAT_POLYFUNC_f_f_pf
#undef TLFLOAT_POLYFUNC_f_f_f
#undef TLFLOAT_POLYFUNC_i_f
#undef TLFLOAT_POLYFUNC_f_f
#endif // #if !defined(TLFLOAT_DOXYGEN)
} // namespace tlfloat {

static_assert(sizeof(tlfloat_quad) == 16, "sizeof(tlfloat_quad)");
static_assert(sizeof(tlfloat_octuple) == 32, "sizeof(tlfloat_octuple)");
#endif // #if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)

/** This function casts a double-precision FP number to a octuple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_cast_o_d(const double x) { return tlfloat_cast_o_d_(x); }
  /** This function casts a 64-bit signed integer to a octuple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_cast_o_i64(const int64_t x) { return tlfloat_cast_o_i64_(x); }
  /** This function casts a 64-bit unsigned integer to a octuple-precision FP number. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_cast_o_u64(const uint64_t x) { return tlfloat_cast_o_u64_(x); }
/** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
static inline tlfloat_octuple tlfloat_strtoo(const char *nptr, const char **endptr) { return tlfloat_strtoo_(nptr, endptr); }

static inline tlfloat_int128_t tlfloat_cast_i128_i64(const int64_t x) { return tlfloat_cast_i128_i64_(x); }
static inline tlfloat_uint128_t tlfloat_cast_u128_u64(const uint64_t x) { return tlfloat_cast_u128_u64_(x); }
static inline tlfloat_int128_t tlfloat_cast_i128_d(const double x) { return tlfloat_cast_i128_d_(x); }
static inline tlfloat_uint128_t tlfloat_cast_u128_d(const double x) { return tlfloat_cast_u128_d_(x); }

//

#endif // #ifndef __TLFLOAT_H_INCLUDED__
