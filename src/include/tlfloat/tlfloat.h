#ifndef __TLFLOAT_H_INCLUDED__
#define __TLFLOAT_H_INCLUDED__

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#if defined(TLFLOAT_DOXYGEN)
/** Calls to libquadmath functions are replaced to the corresponding
    calls to tlfloat functions if this macro is defined */
#define TLFLOAT_LIBQUADMATH_EMULATION
#endif

#if defined(__LDBL_MANT_DIG__)
#if __LDBL_MANT_DIG__ == 113
#define TLFLOAT_LONGDOUBLE_IS_FLOAT128
#endif
#endif

#if defined(__SIZEOF_FLOAT128__)
#if __SIZEOF_FLOAT128__ == 16
#define TLFLOAT_COMPILER_SUPPORTS_FLOAT128
#endif
#endif

#ifdef __cplusplus
#include <type_traits>

extern "C" {
#endif

  typedef struct { uint64_t e[1 << 0]; } tlfloat_bigint6;
  typedef struct { uint64_t e[1 << 1]; } tlfloat_bigint7;
  typedef struct { uint64_t e[1 << 2]; } tlfloat_bigint8;
  typedef struct { uint64_t e[1 << 3]; } tlfloat_bigint9;
  typedef struct { uint64_t e[1 << 4]; } tlfloat_bigint10;

#if defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_DEBUG_QUAD)
  typedef __float128 tlfloat_quad_;
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128) && !defined(TLFLOAT_DEBUG_QUAD)
  typedef long double tlfloat_quad_;
#else
  typedef struct { uint64_t e[2]; } tlfloat_quad_;
#endif

  typedef struct { uint64_t e[4]; } tlfloat_octuple_;

  //

  /** This function returns TLFLOAT_VERSION_MAJOR * 1000000ULL + TLFLOAT_VERSION_MINOR * 1000ULL + TLFLOAT_VERSION_PATCHLEVEL */
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

  /** The tlfloat_strtod family of functions provides functionality equivalent to the standard strtod family of functions,
   * but corresponds to the types defined in the TLFloat library. */
  double tlfloat_strtod(const char *, const char **);
  float tlfloat_strtof(const char *, const char **);
  tlfloat_quad_ tlfloat_strtoq(const char *, const char **);
  tlfloat_octuple_ tlfloat_strtoo(const char *, const char **);

  //

  /** This function casts a quadruple-precision FP number to a double-precision FP number. Link with -ltlfloat. */
  double tlfloat_cast_d_q(const tlfloat_quad_ x         );
  /** This function casts a octuple-precision FP number to a double-precision FP number. Link with -ltlfloat. */
  double tlfloat_cast_d_o(const tlfloat_octuple_ x      );

  /** This function casts a double-precision FP number to a quadruple-precision FP number. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_cast_q_d(const double x         );
  /** This function casts a octuple-precision FP number to a quadruple-precision FP number. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_cast_q_o(const tlfloat_octuple_ x);

  /** This function casts a double-precision FP number to a octuple-precision FP number. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_cast_o_d(const double x      );
  /** This function casts a quadruple-precision FP number to a octuple-precision FP number. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_cast_o_q(const tlfloat_quad_ x);

  /** This function casts a quadruple-precision FP number to a 64-bit signed integer. Link with -ltlfloat. */
  int64_t tlfloat_cast_i64_q(const tlfloat_quad_ x);
  /** This function casts a 64-bit signed integer to a quadruple-precision FP number. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_cast_q_i64(const int64_t x);

  /** This function casts a octuple-precision FP number to a 64-bit signed integer. Link with -ltlfloat. */
  int64_t tlfloat_cast_i64_o(const tlfloat_octuple_ x);
  /** This function casts a 64-bit signed integer to a octuple-precision FP number. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_cast_o_i64(const int64_t x);

  /** This function casts a quadruple-precision FP number to a 64-bit unsigned integer. Link with -ltlfloat. */
  uint64_t tlfloat_cast_u64_q(const tlfloat_quad_ x);
  /** This function casts a 64-bit unsigned integer to a quadruple-precision FP number. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_cast_q_u64(const uint64_t x);

  /** This function casts a octuple-precision FP number to a 64-bit unsigned integer. Link with -ltlfloat. */
  uint64_t tlfloat_cast_u64_o(const tlfloat_octuple_ x);
  /** This function casts a 64-bit unsigned integer to a octuple-precision FP number. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_cast_o_u64(const uint64_t x);

  //

  /** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_eq_q_q (const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_neq_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_lt_q_q (const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_le_q_q (const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_gt_q_q (const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs ordered comparison of two quad-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_ge_q_q (const tlfloat_quad_ x, const tlfloat_quad_ y);

  /** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_eq_o_o (const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_neq_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_lt_o_o (const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_le_o_o (const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_gt_o_o (const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs ordered comparison of two octuple-precision floating point numbers. Link with -ltlfloat. */
  int tlfloat_ge_o_o (const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  //

  /** This function performs addition of two quadruple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_addq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs addition of two octuple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_addo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs subtraction of two quadruple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_subq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs subtraction of two octuple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_subo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs multiplication of two quadruple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_mulq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs multiplication of two octuple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_mulo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);
  /** This function performs division of two quadruple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_divq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function performs division of two octuple-precision floating point numbers. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_divo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  tlfloat_quad_ tlfloat_negq(const tlfloat_quad_ x);
  tlfloat_octuple_ tlfloat_nego(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmaf(const float x, const float y, const float z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fma(const double x, const double y, const double z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_fmaq(const tlfloat_quad_ x, const tlfloat_quad_ y, const tlfloat_quad_ z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_fmao(const tlfloat_octuple_ x, const tlfloat_octuple_ y, const tlfloat_octuple_ z);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sqrtf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sqrt(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_sqrtq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_sqrto(const tlfloat_octuple_ x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fabsf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fabs(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_fabsq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_fabso(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_copysignf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_copysign(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_copysignq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_copysigno(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmaxf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmax(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_fmaxq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_fmaxo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fminf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmin(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_fminq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_fmino(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fdimf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fdim(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_fdimq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_fdimo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_ldexpf(const float x, const int y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_ldexp(const double x, const int y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_ldexpq(const tlfloat_quad_ x, const int y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_ldexpo(const tlfloat_octuple_ x, const int y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_frexpf(const float x, int *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_frexp(const double x, int *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_frexpq(const tlfloat_quad_ x, int *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_frexpo(const tlfloat_octuple_ x, int *y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_modff(const float x, float *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_modf(const double x, double *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_modfq(const tlfloat_quad_ x, tlfloat_quad_ *y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_modfo(const tlfloat_octuple_ x, tlfloat_octuple_ *y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_nextafterf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_nextafter(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_nextafterq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_nextaftero(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_ilogbf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_ilogb(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_ilogbq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_ilogbo(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isnanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isnan(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isnanq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isnano(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isinff(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isinf(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isinfq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_isinfo(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_finitef(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_finite(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_finiteq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_finiteo(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_signbitf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_signbit(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_signbitq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  int tlfloat_signbito(const tlfloat_octuple_ x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_hypotf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_hypot(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_hypotq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_hypoto(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_truncf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_trunc(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_truncq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_trunco(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_floorf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_floor(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_floorq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_flooro(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_ceilf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_ceil(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_ceilq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_ceilo(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_roundf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_round(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_roundq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_roundo(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_rintf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_rint(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_rintq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_rinto(const tlfloat_octuple_ x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sinf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sin(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_sinq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_sino(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_cosf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cos(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_cosq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_coso(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tan(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_tanq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_tano(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_asinf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_asin(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_asinq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_asino(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_acosf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_acos(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_acosq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_acoso(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atanf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atan(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_atanq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_atano(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atan2f(const float y, const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atan2(const double y, const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_atan2q(const tlfloat_quad_ y, const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_atan2o(const tlfloat_octuple_ y, const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_expf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_expq(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_expo(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_expm1f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_expm1(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_expm1q(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_expm1o(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_exp2f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp2(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_exp2q(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_exp2o(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_exp10f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_exp10(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_exp10q(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_exp10o(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_logf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_logq(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_logo(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log1pf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log1p(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_log1pq(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_log1po(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log2f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log2(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_log2q(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_log2o(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_log10f(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_log10(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_log10q(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_log10o(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_powf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_pow(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_powq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_powo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_cbrtf(const float a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cbrt(const double a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_cbrtq(const tlfloat_quad_ a);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_cbrto(const tlfloat_octuple_ a);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sinhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sinh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_sinhq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_sinho(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_coshf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_cosh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_coshq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_cosho(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_tanhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_tanh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_tanhq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_tanho(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_asinhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_asinh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_asinhq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_asinho(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_acoshf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_acosh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_acoshq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_acosho(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_atanhf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_atanh(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_atanhq(const tlfloat_quad_ x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_atanho(const tlfloat_octuple_ x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmodf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fmod(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_fmodq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_fmodo(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_remainderf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_remainder(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad_ tlfloat_remainderq(const tlfloat_quad_ x, const tlfloat_quad_ y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple_ tlfloat_remaindero(const tlfloat_octuple_ x, const tlfloat_octuple_ y);

#ifdef __cplusplus
} // extern "C" {
#endif

#if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)) || defined(TLFLOAT_DOXYGEN) || defined(TLFLOAT_DEBUG_QUAD)
/** This class is for handling quadruple-precision IEEE floating-point numbers in older C++.
 * This is an alias for __float128 if that type is defined by the compiler.
 * If long double is IEEE float 128, this type is an alias for long double. */
struct tlfloat_quad {
  tlfloat_quad_ value;

  tlfloat_quad() { value.e[0] = value.e[1] = 0; }

  // Conversion to/from the corresponding C type
  tlfloat_quad(const tlfloat_quad_& v) : value(v) {}
  operator tlfloat_quad_() const { return value; }

  // Upcast / downcast
  explicit tlfloat_quad(const struct tlfloat_octuple& v);

  // Conversion to/from double
  tlfloat_quad(const double& d) : value(tlfloat_cast_q_d(d)) {}
  explicit operator double() const { return tlfloat_cast_d_q(value); }

  // Conversion to/from integral types
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) < 64), int>::type = 0>
  tlfloat_quad(const T& i) : value(tlfloat_cast_q_d((double)i)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) < 64), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_d_q(value); }

  //

  tlfloat_quad operator+(const tlfloat_quad& rhs) const { return tlfloat_addq(value, rhs.value); }
  tlfloat_quad operator-(const tlfloat_quad& rhs) const { return tlfloat_subq(value, rhs.value); }
  tlfloat_quad operator*(const tlfloat_quad& rhs) const { return tlfloat_mulq(value, rhs.value); }
  tlfloat_quad operator/(const tlfloat_quad& rhs) const { return tlfloat_divq(value, rhs.value); }

  tlfloat_quad operator-() const { return tlfloat_negq(value); }
  tlfloat_quad operator+() const { return *this; }

  bool operator==(const tlfloat_quad& rhs) const { return tlfloat_eq_q_q(value, rhs.value); }
  bool operator!=(const tlfloat_quad& rhs) const { return tlfloat_neq_q_q(value, rhs.value); }
  bool operator< (const tlfloat_quad& rhs) const { return tlfloat_lt_q_q(value, rhs.value); }
  bool operator<=(const tlfloat_quad& rhs) const { return tlfloat_le_q_q(value, rhs.value); }
  bool operator> (const tlfloat_quad& rhs) const { return tlfloat_gt_q_q(value, rhs.value); }
  bool operator>=(const tlfloat_quad& rhs) const { return tlfloat_ge_q_q(value, rhs.value); }
};
#else // #if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)) || defined(TLFLOAT_DOXYGEN)
typedef tlfloat_quad_ tlfloat_quad;
#endif

#if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
/** This type is for handling octuple-precision IEEE floating-point numbers in older C++.
 * The data size and data structure of this type are the same as an octuple-precision floating-point number. */
struct tlfloat_octuple {
  tlfloat_octuple_ value;

  tlfloat_octuple() { value.e[0] = value.e[1] = value.e[2] = value.e[3] = 0; }

  // Conversion to/from the corresponding C type
  tlfloat_octuple(const tlfloat_octuple_& v) : value(v) {}
  operator tlfloat_octuple_() const { return value; }

  // Upcast / downcast
  tlfloat_octuple(const tlfloat_quad& v) : value(tlfloat_cast_o_q(v)) {}
  explicit operator tlfloat_quad() const { return tlfloat_cast_q_o(value); }

  // Conversion to/from double
  tlfloat_octuple(const double& d) : value(tlfloat_cast_o_d(d)) {}
  explicit operator double() const { return tlfloat_cast_d_o(value); }

  // Conversion to/from integral types
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) < 64), int>::type = 0>
  tlfloat_octuple(const T& i) : value(tlfloat_cast_o_d((double)i)) {}
  template<typename T, typename std::enable_if<(std::is_integral<T>::value && sizeof(T) < 64), int>::type = 0>
  explicit operator T() const { return tlfloat_cast_d_o(value); }

  //

  tlfloat_octuple operator+(const tlfloat_octuple& rhs) const { return tlfloat_addo(value, rhs.value); }
  tlfloat_octuple operator-(const tlfloat_octuple& rhs) const { return tlfloat_subo(value, rhs.value); }
  tlfloat_octuple operator*(const tlfloat_octuple& rhs) const { return tlfloat_mulo(value, rhs.value); }
  tlfloat_octuple operator/(const tlfloat_octuple& rhs) const { return tlfloat_divo(value, rhs.value); }

  tlfloat_octuple operator-() const { return tlfloat_nego(value); }
  tlfloat_octuple operator+() const { return *this; }

  bool operator==(const tlfloat_octuple& rhs) const { return tlfloat_eq_o_o(value, rhs.value); }
  bool operator!=(const tlfloat_octuple& rhs) const { return tlfloat_neq_o_o(value, rhs.value); }
  bool operator< (const tlfloat_octuple& rhs) const { return tlfloat_lt_o_o(value, rhs.value); }
  bool operator<=(const tlfloat_octuple& rhs) const { return tlfloat_le_o_o(value, rhs.value); }
  bool operator> (const tlfloat_octuple& rhs) const { return tlfloat_gt_o_o(value, rhs.value); }
  bool operator>=(const tlfloat_octuple& rhs) const { return tlfloat_ge_o_o(value, rhs.value); }
};
#else // #if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
typedef tlfloat_octuple_ tlfloat_octuple;
#endif

#if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)) || defined(TLFLOAT_DOXYGEN) || defined(TLFLOAT_DEBUG_QUAD)

inline tlfloat_quad::tlfloat_quad(const struct tlfloat_octuple& v) : value(tlfloat_cast_q_o(v)) {}

static inline tlfloat_quad tlfloat_fmaq(const tlfloat_quad x, const tlfloat_quad y, const tlfloat_quad z) { return tlfloat_fmaq(tlfloat_quad_(x), tlfloat_quad_(y), tlfloat_quad_(z)); }
static inline tlfloat_quad tlfloat_sqrtq(const tlfloat_quad x) { return tlfloat_sqrtq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_fabsq(const tlfloat_quad x) { return tlfloat_fabsq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_copysignq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_copysignq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_fmaxq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmaxq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_fminq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fminq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_fdimq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fdimq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_ldexpq(const tlfloat_quad x, const int y) { return tlfloat_ldexpq(tlfloat_quad_(x), y); }
static inline tlfloat_quad tlfloat_frexpq(const tlfloat_quad x, int *y) { return tlfloat_frexpq(tlfloat_quad_(x), y); }
static inline tlfloat_quad tlfloat_modfq(const tlfloat_quad x, tlfloat_quad *y) { return tlfloat_modfq(tlfloat_quad_(x), (tlfloat_quad_ *)y); }
static inline tlfloat_quad tlfloat_nextafterq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_nextafterq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline int tlfloat_ilogbq(const tlfloat_quad x) { return tlfloat_ilogbq(tlfloat_quad_(x)); }
static inline int tlfloat_isnanq(const tlfloat_quad x) { return tlfloat_isnanq(tlfloat_quad_(x)); }
static inline int tlfloat_isinfq(const tlfloat_quad x) { return tlfloat_isinfq(tlfloat_quad_(x)); }
static inline int tlfloat_finiteq(const tlfloat_quad x) { return tlfloat_finiteq(tlfloat_quad_(x)); }
static inline int tlfloat_signbitq(const tlfloat_quad x) { return tlfloat_signbitq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_hypotq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_hypotq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_truncq(const tlfloat_quad x) { return tlfloat_truncq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_floorq(const tlfloat_quad x) { return tlfloat_floorq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_ceilq(const tlfloat_quad x) { return tlfloat_ceilq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_roundq(const tlfloat_quad x) { return tlfloat_roundq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_rintq(const tlfloat_quad x) { return tlfloat_rintq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_sinq(const tlfloat_quad x) { return tlfloat_sinq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_cosq(const tlfloat_quad x) { return tlfloat_cosq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_tanq(const tlfloat_quad x) { return tlfloat_tanq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_asinq(const tlfloat_quad x) { return tlfloat_asinq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_acosq(const tlfloat_quad x) { return tlfloat_acosq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_atanq(const tlfloat_quad x) { return tlfloat_atanq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_atan2q(const tlfloat_quad y, const tlfloat_quad x) { return tlfloat_atan2q(tlfloat_quad_(y), tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_expq(const tlfloat_quad x) { return tlfloat_expq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_expm1q(const tlfloat_quad x) { return tlfloat_expm1q(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_exp2q(const tlfloat_quad x) { return tlfloat_exp2q(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_exp10q(const tlfloat_quad x) { return tlfloat_exp10q(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_logq(const tlfloat_quad x) { return tlfloat_logq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_log1pq(const tlfloat_quad x) { return tlfloat_log1pq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_log2q(const tlfloat_quad x) { return tlfloat_log2q(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_log10q(const tlfloat_quad x) { return tlfloat_log10q(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_powq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_powq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_cbrtq(const tlfloat_quad x) { return tlfloat_cbrtq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_sinhq(const tlfloat_quad x) { return tlfloat_sinhq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_coshq(const tlfloat_quad x) { return tlfloat_coshq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_tanhq(const tlfloat_quad x) { return tlfloat_tanhq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_asinhq(const tlfloat_quad x) { return tlfloat_asinhq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_acoshq(const tlfloat_quad x) { return tlfloat_acoshq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_atanhq(const tlfloat_quad x) { return tlfloat_atanhq(tlfloat_quad_(x)); }
static inline tlfloat_quad tlfloat_fmodq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmodq(tlfloat_quad_(x), tlfloat_quad_(y)); }
static inline tlfloat_quad tlfloat_remainderq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_remainderq(tlfloat_quad_(x), tlfloat_quad_(y)); }
#endif // #if (defined(__cplusplus) && !defined(TLFLOAT_COMPILER_SUPPORTS_FLOAT128) && !defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)) || defined(TLFLOAT_DOXYGEN) || defined(TLFLOAT_DEBUG_QUAD)

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
  static inline tlfloat_quad fmodq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_fmodq(x, y); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad remainderq(const tlfloat_quad x, const tlfloat_quad y) { return tlfloat_remainderq(x, y); }
#endif // #ifdef TLFLOAT_LIBQUADMATH_EMULATION

//

#if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)
#define TLFLOAT_OVERLOAD_OP2(OP, OPA) \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline tlfloat_octuple operator OP(const T& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline tlfloat_octuple operator OP(const tlfloat_octuple& lhs, const T& rhs) { return lhs OP tlfloat_octuple(rhs); } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline tlfloat_quad operator OP(const T& lhs, const tlfloat_quad& rhs) { return tlfloat_quad(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline tlfloat_quad operator OP(const tlfloat_quad& lhs, const T& rhs) { return lhs OP tlfloat_quad(rhs); } \
  static inline tlfloat_octuple operator OP(const tlfloat_quad& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  template<typename rhstype>						\
  static inline tlfloat_octuple& operator OPA(tlfloat_octuple& lhs, const rhstype& rhs) { return (lhs = lhs OP tlfloat_octuple(rhs)); } \
  static_assert(true, "")

TLFLOAT_OVERLOAD_OP2(+, +=);
TLFLOAT_OVERLOAD_OP2(-, -=);
TLFLOAT_OVERLOAD_OP2(*, *=);
TLFLOAT_OVERLOAD_OP2(/, /=);

#undef TLFLOAT_OVERLOAD_OP2

#define TLFLOAT_OVERLOAD_CMP(OP) \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline bool operator OP(const T& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline bool operator OP(const tlfloat_octuple& lhs, const T& rhs) { return lhs OP tlfloat_octuple(rhs); } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline bool operator OP(const T& lhs, const tlfloat_quad& rhs) { return tlfloat_quad(lhs) OP rhs; } \
  template<typename T, typename std::enable_if<((std::is_floating_point<T>::value && sizeof(T) <= 64) || (std::is_integral<T>::value && sizeof(T) < 64)), int>::type = 0> \
  static inline bool operator OP(const tlfloat_quad& lhs, const T& rhs) { return lhs OP tlfloat_quad(rhs); } \
  static inline bool operator OP(const tlfloat_quad& lhs, const tlfloat_octuple& rhs) { return tlfloat_octuple(lhs) OP rhs; } \
  static_assert(true, "")

TLFLOAT_OVERLOAD_CMP(==);
TLFLOAT_OVERLOAD_CMP(!=);
TLFLOAT_OVERLOAD_CMP(<);
TLFLOAT_OVERLOAD_CMP(<=);
TLFLOAT_OVERLOAD_CMP(>);
TLFLOAT_OVERLOAD_CMP(>=);

#undef TLFLOAT_OVERLOAD_CMP

static inline tlfloat_octuple tlfloat_fmao(const tlfloat_octuple x, const tlfloat_octuple y, const tlfloat_octuple z) { return tlfloat_fmao(tlfloat_octuple_(x), tlfloat_octuple_(y), tlfloat_octuple_(z)); }
static inline tlfloat_octuple tlfloat_sqrto(const tlfloat_octuple x) { return tlfloat_sqrto(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_fabso(const tlfloat_octuple x) { return tlfloat_fabso(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_copysigno(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_copysigno(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_fmaxo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fmaxo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_fmino(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fmino(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_fdimo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fdimo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_ldexpo(const tlfloat_octuple x, const int y) { return tlfloat_ldexpo(tlfloat_octuple_(x), y); }
static inline tlfloat_octuple tlfloat_frexpo(const tlfloat_octuple x, int *y) { return tlfloat_frexpo(tlfloat_octuple_(x), y); }
static inline tlfloat_octuple tlfloat_modfo(const tlfloat_octuple x, tlfloat_octuple *y) { return tlfloat_modfo(tlfloat_octuple_(x), (tlfloat_octuple_ *)y); }
static inline tlfloat_octuple tlfloat_nextaftero(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_nextaftero(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline int tlfloat_ilogbo(const tlfloat_octuple x) { return tlfloat_ilogbo(tlfloat_octuple_(x)); }
static inline int tlfloat_isnano(const tlfloat_octuple x) { return tlfloat_isnano(tlfloat_octuple_(x)); }
static inline int tlfloat_isinfo(const tlfloat_octuple x) { return tlfloat_isinfo(tlfloat_octuple_(x)); }
static inline int tlfloat_finiteo(const tlfloat_octuple x) { return tlfloat_finiteo(tlfloat_octuple_(x)); }
static inline int tlfloat_signbito(const tlfloat_octuple x) { return tlfloat_signbito(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_hypoto(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_hypoto(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_trunco(const tlfloat_octuple x) { return tlfloat_trunco(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_flooro(const tlfloat_octuple x) { return tlfloat_flooro(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_ceilo(const tlfloat_octuple x) { return tlfloat_ceilo(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_roundo(const tlfloat_octuple x) { return tlfloat_roundo(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_rinto(const tlfloat_octuple x) { return tlfloat_rinto(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_sino(const tlfloat_octuple x) { return tlfloat_sino(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_coso(const tlfloat_octuple x) { return tlfloat_coso(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_tano(const tlfloat_octuple x) { return tlfloat_tano(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_asino(const tlfloat_octuple x) { return tlfloat_asino(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_acoso(const tlfloat_octuple x) { return tlfloat_acoso(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_atano(const tlfloat_octuple x) { return tlfloat_atano(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_atan2o(const tlfloat_octuple y, const tlfloat_octuple x) { return tlfloat_atan2o(tlfloat_octuple_(y), tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_expo(const tlfloat_octuple x) { return tlfloat_expo(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_expm1o(const tlfloat_octuple x) { return tlfloat_expm1o(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_exp2o(const tlfloat_octuple x) { return tlfloat_exp2o(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_exp10o(const tlfloat_octuple x) { return tlfloat_exp10o(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_logo(const tlfloat_octuple x) { return tlfloat_logo(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_log1po(const tlfloat_octuple x) { return tlfloat_log1po(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_log2o(const tlfloat_octuple x) { return tlfloat_log2o(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_log10o(const tlfloat_octuple x) { return tlfloat_log10o(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_powo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_powo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_cbrto(const tlfloat_octuple x) { return tlfloat_cbrto(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_sinho(const tlfloat_octuple x) { return tlfloat_sinho(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_cosho(const tlfloat_octuple x) { return tlfloat_cosho(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_tanho(const tlfloat_octuple x) { return tlfloat_tanho(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_asinho(const tlfloat_octuple x) { return tlfloat_asinho(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_acosho(const tlfloat_octuple x) { return tlfloat_acosho(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_atanho(const tlfloat_octuple x) { return tlfloat_atanho(tlfloat_octuple_(x)); }
static inline tlfloat_octuple tlfloat_fmodo(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_fmodo(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
static inline tlfloat_octuple tlfloat_remaindero(const tlfloat_octuple x, const tlfloat_octuple y) { return tlfloat_remaindero(tlfloat_octuple_(x), tlfloat_octuple_(y)); }
#endif // #if defined(__cplusplus) || defined(TLFLOAT_DOXYGEN)

//

#endif // #ifndef __TLFLOAT_H_INCLUDED__
