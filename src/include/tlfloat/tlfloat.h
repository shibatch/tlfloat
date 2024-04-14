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

  /** The tlfloat_strtod family of functions provides functionality equivalent to the standard strtod family of functions,
   * but corresponds to the types defined in the TLFloat library. */
  double tlfloat_strtod(const char *, const char **);
  float tlfloat_strtof(const char *, const char **);
  tlfloat_quad tlfloat_strtoq(const char *, const char **);
  tlfloat_octuple tlfloat_strtoo(const char *, const char **);

  //

  double tlfloat_cast_d_q(const tlfloat_quad x         );
  double tlfloat_cast_d_o(const tlfloat_octuple x      );

  tlfloat_quad tlfloat_cast_q_d(const double x         );
  tlfloat_quad tlfloat_cast_q_o(const tlfloat_octuple x);

  tlfloat_octuple tlfloat_cast_o_d(const double x      );
  tlfloat_octuple tlfloat_cast_o_q(const tlfloat_quad x);

  int64_t tlfloat_cast_i64_q(const tlfloat_quad x);
  tlfloat_quad tlfloat_cast_q_i64(const int64_t x);

  int64_t tlfloat_cast_i64_o(const tlfloat_octuple x);
  tlfloat_octuple tlfloat_cast_o_i64(const int64_t x);

  uint64_t tlfloat_cast_u64_q(const tlfloat_quad x);
  tlfloat_quad tlfloat_cast_q_u64(const uint64_t x);

  uint64_t tlfloat_cast_u64_o(const tlfloat_octuple x);
  tlfloat_octuple tlfloat_cast_o_u64(const uint64_t x);

  //

  int tlfloat_eq_q_q (const tlfloat_quad x, const tlfloat_quad y);
  int tlfloat_neq_q_q(const tlfloat_quad x, const tlfloat_quad y);
  int tlfloat_lt_q_q (const tlfloat_quad x, const tlfloat_quad y);
  int tlfloat_le_q_q (const tlfloat_quad x, const tlfloat_quad y);
  int tlfloat_gt_q_q (const tlfloat_quad x, const tlfloat_quad y);
  int tlfloat_ge_q_q (const tlfloat_quad x, const tlfloat_quad y);

  int tlfloat_eq_o_o (const tlfloat_octuple x, const tlfloat_octuple y);
  int tlfloat_neq_o_o(const tlfloat_octuple x, const tlfloat_octuple y);
  int tlfloat_lt_o_o (const tlfloat_octuple x, const tlfloat_octuple y);
  int tlfloat_le_o_o (const tlfloat_octuple x, const tlfloat_octuple y);
  int tlfloat_gt_o_o (const tlfloat_octuple x, const tlfloat_octuple y);
  int tlfloat_ge_o_o (const tlfloat_octuple x, const tlfloat_octuple y);

  //

  tlfloat_quad tlfloat_addq(const tlfloat_quad x, const tlfloat_quad y);
  tlfloat_octuple tlfloat_addo(const tlfloat_octuple x, const tlfloat_octuple y);

  tlfloat_quad tlfloat_subq(const tlfloat_quad x, const tlfloat_quad y);
  tlfloat_octuple tlfloat_subo(const tlfloat_octuple x, const tlfloat_octuple y);

  tlfloat_quad tlfloat_mulq(const tlfloat_quad x, const tlfloat_quad y);
  tlfloat_octuple tlfloat_mulo(const tlfloat_octuple x, const tlfloat_octuple y);

  tlfloat_quad tlfloat_divq(const tlfloat_quad x, const tlfloat_quad y);
  tlfloat_octuple tlfloat_divo(const tlfloat_octuple x, const tlfloat_octuple y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_fmaf(const float x, const float y, const float z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_fma(const double x, const double y, const double z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_fmaq(const tlfloat_quad x, const tlfloat_quad y, const tlfloat_quad z);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_fmao(const tlfloat_octuple x, const tlfloat_octuple y, const tlfloat_octuple z);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_sqrtf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_sqrt(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_sqrtq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_sqrto(const tlfloat_octuple x);

  //

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_hypotf(const float x, const float y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_hypot(const double x, const double y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_hypotq(const tlfloat_quad x, const tlfloat_quad y);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_hypoto(const tlfloat_octuple x, const tlfloat_octuple y);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_truncf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_trunc(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_truncq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_trunco(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_floorf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_floor(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_floorq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_flooro(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_ceilf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_ceil(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_ceilq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_ceilo(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_roundf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_round(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_roundq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_roundo(const tlfloat_octuple x);

  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  float tlfloat_rintf(const float x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  double tlfloat_rint(const double x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_quad tlfloat_rintq(const tlfloat_quad x);
  /** This function is for calling the corresponding function defined in tlfloat namespace from C language. Link with -ltlfloat. */
  tlfloat_octuple tlfloat_rinto(const tlfloat_octuple x);

  //

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
  static inline tlfloat_quad fmaq(const tlfloat_quad x, const tlfloat_quad y, const tlfloat_quad z) { return tlfloat_fmaq(x, y, z); }

  /** This function has the same functionality as the corresponding function in quadmath.h.
   * This function is available only if TLFLOAT_LIBQUADMATH_EMULATION macro is defined.
   * Link with -ltlfloat. */
  static inline tlfloat_quad sqrtq(const tlfloat_quad x) { return tlfloat_sqrtq(x); }

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
#endif

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // #ifndef __TLFLOAT_H_INCLUDED__
