#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "tlfloat/tlfloatversion.hpp"

#if defined(TLFLOAT_COMPILER_SUPPORTS_INT128) && defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#include <quadmath.h>
#define TRUE_LIBQUADMATH
#else
#define TLFLOAT_LIBQUADMATH_EMULATION
#endif

#include "tlfloat/tlfloat.h"

#ifdef __STDC_VERSION__
const tlfloat_quad testvar = M_PIq;
#endif

void checkStr(const char *p, const char *q, const char *mes) {
  if (strcmp(p, q) != 0) {
    printf("NG : %s\n", mes);
    printf("p : %s\n", p);
    printf("q : %s\n", q);
    exit(-1);
  }
}

void checkInt(const int u, const int v, const char *mes) {
  if (u != v) {
    printf("NG : %s\n", mes);
    printf("u : %d\n", u);
    printf("v : %d\n", v);
    exit(-1);
  }
}

void checkQuo(const int u, const int v, const char *mes) {
  int u2 = u & 0x7, v2 = v & 0x7;
  if (u < 0) u2 = -u2;
  if (v < 0) v2 = -v2;
  if (u2 != v2) {
    printf("NG : %s\n", mes);
    printf("u : %d %d\n", u, u2);
    printf("v : %d %d\n", v, v2);
    exit(-1);
  }
}

void checkDouble(const double u, const double v, const char *mes) {
  if (isinf(u) && isinf(v)) return;
  if (u != v) {
    printf("NG : %s\n", mes);
    printf("u : %.20g\n", u);
    printf("v : %.20g\n", v);
    exit(-1);
  }
}

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
__int128_t rndi128() {
  __int128_t r = 0;
  for(int i=0;i<8;i++) r = (r << 16) ^ rand();
  r &= ~((~(__uint128_t)0) << (rand() & 127));
  return r;
}

__uint128_t rndu128() {
  __uint128_t r = 0;
  for(int i=0;i<8;i++) r = (r << 16) ^ rand();
  r &= ~((~(__uint128_t)0) << (rand() & 127));
  return r;
}

void checkInt128(const tlfloat_int128_t u_, const __int128_t v, const char *mes) {
  __int128_t u;
  memcpy(&u, &u_, sizeof(u));

  if (u != v) {
    printf("NG : %s\n", mes);
    printf("u : 0x%016llx%016llx\n", (unsigned long long)(u >> 64), (unsigned long long)u);
    printf("v : 0x%016llx%016llx\n", (unsigned long long)(v >> 64), (unsigned long long)v);
    exit(-1);
  }
}

void checkUInt128(const tlfloat_uint128_t u_, const __uint128_t v, const char *mes) {
  __uint128_t u;
  memcpy(&u, &u_, sizeof(u));

  if (u != v) {
    printf("NG : %s\n", mes);
    printf("u : 0x%016llx%016llx\n", (unsigned long long)(u >> 64), (unsigned long long)u);
    printf("v : 0x%016llx%016llx\n", (unsigned long long)(v >> 64), (unsigned long long)v);
    exit(-1);
  }
}

void doTestInt128(const __int128_t x, const __int128_t y) {
  tlfloat_int128_t x_, y_;
  memcpy(&x_, &x, sizeof(x_));
  memcpy(&y_, &y, sizeof(y_));

  //printf("x : 0x%016llx%016llx\n", (unsigned long long)(x >> 64), (unsigned long long)x);
  //printf("y : 0x%016llx%016llx\n", (unsigned long long)(y >> 64), (unsigned long long)y);

  checkInt128(tlfloat_add_i128_i128(x_, y_), x + y, "int128 +");
  checkInt128(tlfloat_sub_i128_i128(x_, y_), x - y, "int128 -");
  checkInt128(tlfloat_mul_i128_i128(x_, y_), x * y, "int128 *");
  if (y != 0) {
    checkInt128(tlfloat_div_i128_i128(x_, y_), x / y, "int128 /");
    checkInt128(tlfloat_mod_i128_i128(x_, y_), x % y, "int128 %");
  }
  checkInt128(tlfloat_neg_i128     (x_    ), -x   , "int128 -");

  checkInt128(tlfloat_and_i128_i128(x_, y_), x & y, "int128 &");
  checkInt128(tlfloat_or_i128_i128 (x_, y_), x | y, "int128 |");
  checkInt128(tlfloat_xor_i128_i128(x_, y_), x ^ y, "int128 ^");
  checkInt128(tlfloat_not_i128     (x_    ), ~x   , "int128 ~");

  checkInt128(tlfloat_shl_i128_i   (x_, (int)(y & 127)), x << (int)(y & 127), "int128 <<");
  checkInt128(tlfloat_shr_i128_i   (x_, (int)(y & 127)), x >> (int)(y & 127), "int128 >>");

  checkInt(tlfloat_eq_i128_i128(x_, y_), x == y, "int128 ==");
  checkInt(tlfloat_ne_i128_i128(x_, y_), x != y, "int128 !=");
  checkInt(tlfloat_gt_i128_i128(x_, y_), x >  y, "int128 >");
  checkInt(tlfloat_ge_i128_i128(x_, y_), x >= y, "int128 >=");
  checkInt(tlfloat_lt_i128_i128(x_, y_), x <  y, "int128 <");
  checkInt(tlfloat_le_i128_i128(x_, y_), x <= y, "int128 <=");

  checkDouble(tlfloat_cast_d_i128(x_), (double)x, "cast double <= int128");
  checkInt128(tlfloat_cast_i128_d((double)x), (__int128)(double)x, "cast int128 <= double");
}

void doTestUInt128(const __uint128_t x, const __uint128_t y) {
  tlfloat_uint128_t x_, y_;
  memcpy(&x_, &x, sizeof(x_));
  memcpy(&y_, &y, sizeof(y_));

  //printf("x : 0x%016llx%016llx\n", (unsigned long long)(x >> 64), (unsigned long long)x);
  //printf("y : 0x%016llx%016llx\n", (unsigned long long)(y >> 64), (unsigned long long)y);

  checkUInt128(tlfloat_add_u128_u128(x_, y_), x + y, "uint128 +");
  checkUInt128(tlfloat_sub_u128_u128(x_, y_), x - y, "uint128 -");
  checkUInt128(tlfloat_mul_u128_u128(x_, y_), x * y, "uint128 *");
  if (y != 0) {
    checkUInt128(tlfloat_div_u128_u128(x_, y_), x / y, "uint128 /");
    checkUInt128(tlfloat_mod_u128_u128(x_, y_), x % y, "uint128 %");
  }

  checkUInt128(tlfloat_and_u128_u128(x_, y_), x & y, "uint128 &");
  checkUInt128(tlfloat_or_u128_u128 (x_, y_), x | y, "uint128 |");
  checkUInt128(tlfloat_xor_u128_u128(x_, y_), x ^ y, "uint128 ^");
  checkUInt128(tlfloat_not_u128     (x_    ), ~x   , "uint128 ~");

  checkUInt128(tlfloat_shl_u128_i   (x_, (int)(y & 127)), x << (int)(y & 127), "uint128 <<");
  checkUInt128(tlfloat_shr_u128_i   (x_, (int)(y & 127)), x >> (int)(y & 127), "uint128 >>");

  checkInt(tlfloat_eq_u128_u128(x_, y_), x == y, "uint128 ==");
  checkInt(tlfloat_ne_u128_u128(x_, y_), x != y, "uint128 !=");
  checkInt(tlfloat_gt_u128_u128(x_, y_), x >  y, "uint128 >");
  checkInt(tlfloat_ge_u128_u128(x_, y_), x >= y, "uint128 >=");
  checkInt(tlfloat_lt_u128_u128(x_, y_), x <  y, "uint128 <");
  checkInt(tlfloat_le_u128_u128(x_, y_), x <= y, "uint128 <=");

  checkDouble(tlfloat_cast_d_u128(x_), (double)x, "cast double <= uint128");
  checkUInt128(tlfloat_cast_u128_d((double)x), (__uint128_t)(double)x, "cast uint128 <= double");
}
#endif

char buf0[1000], buf1[1000];

int main(int argc, char **argv) {
  if (tlfloat_version() != TLFLOAT_VERSION_MAJOR * 1000000ULL + TLFLOAT_VERSION_MINOR * 1000ULL + TLFLOAT_VERSION_PATCHLEVEL) {
    printf("NG : tlfloat_version()\n");
    exit(-1);
  }

  const char *endptr = NULL, *oneTenth = "0.1";

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8g", tlfloat_strtod(oneTenth, &endptr));
  checkStr(buf0, "0.1", "strtod");

  checkInt(endptr - oneTenth, 3, "tlfloat_strtod endptr");

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8g", tlfloat_sin(tlfloat_strtod("0.1", NULL)));
  checkStr(buf0, "0.099833417", "sin");

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Qg", tlfloat_strtoq("0.1", NULL));
  checkStr(buf0, "0.1", "strtoq");

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Qg", tlfloat_sinq(tlfloat_strtoq("0.1", NULL)));
  checkStr(buf0, "0.099833417", "sinq");

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Qg", strtoflt128("0.1", NULL));
  checkStr(buf0, "0.1", "quadmath strtoflt128");

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Qg", sinq(strtoflt128("0.1", NULL)));
  checkStr(buf0, "0.099833417", "quadmath sinq val");

  quadmath_snprintf(buf0, sizeof(buf0), "%.8Qg", strtoflt128("0.1", NULL));
  checkStr(buf0, "0.1", "quadmath_snprintf");

#ifdef TLFLOAT_COMPILER_SUPPORTS_FLOAT128
  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Qg", (__float128)1.0 / (__float128)10.0);
  checkStr(buf0, "0.1", "tlfloat_snprintf");
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)
  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Lg", strtold("0.1", NULL));
  checkStr(buf0, "0.1", "strtold");
#endif

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Og", tlfloat_strtoo("0.1", NULL));
  checkStr(buf0, "0.1", "strtoo");

  tlfloat_snprintf(buf0, sizeof(buf0), "%.8Og", tlfloat_sino(tlfloat_strtoo("0.1", NULL)));
  checkStr(buf0, "0.099833417", "sino");

  //

  tlfloat_quad q0 = strtoflt128("0.123", NULL), q1 = strtoflt128("1.234", NULL), q2 = strtoflt128("-0.345", NULL);
  tlfloat_octuple o0 = tlfloat_strtoo("0.123", NULL), o1 = tlfloat_strtoo("1.234", NULL), o2 = tlfloat_strtoo("-0.345", NULL);

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", fmaq(q0, q1, q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_fmaq(q0, q1, q2));
  checkStr(buf0, buf1, "quadmath fmaq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_fmao(o0, o1, o2));
  checkStr(buf0, buf1, "quadmath fmao");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", sqrtq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_sqrtq(q0));
  checkStr(buf0, buf1, "quadmath sqrtq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_sqrto(o0));
  checkStr(buf0, buf1, "quadmath sqrto");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", sinq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_sinq(q0));
  checkStr(buf0, buf1, "quadmath sinq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_sino(o0));
  checkStr(buf0, buf1, "quadmath sino");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", cosq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_cosq(q0));
  checkStr(buf0, buf1, "quadmath cosq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_coso(o0));
  checkStr(buf0, buf1, "quadmath coso");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", tanq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_tanq(q0));
  checkStr(buf0, buf1, "quadmath tanq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_tano(o0));
  checkStr(buf0, buf1, "quadmath tano");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", asinq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_asinq(q0));
  checkStr(buf0, buf1, "quadmath asinq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_asino(o0));
  checkStr(buf0, buf1, "quadmath asino");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", acosq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_acosq(q0));
  checkStr(buf0, buf1, "quadmath acosq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_acoso(o0));
  checkStr(buf0, buf1, "quadmath acoso");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", atanq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_atanq(q0));
  checkStr(buf0, buf1, "quadmath atanq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_atano(o0));
  checkStr(buf0, buf1, "quadmath atano");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", atan2q(q0, q1));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_atan2q(q0, q1));
  checkStr(buf0, buf1, "quadmath atan2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_atan2o(o0, o1));
  checkStr(buf0, buf1, "quadmath atan2o");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", sinhq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_sinhq(q0));
  checkStr(buf0, buf1, "quadmath sinhq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_sinho(o0));
  checkStr(buf0, buf1, "quadmath sinho");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", coshq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_coshq(q0));
  checkStr(buf0, buf1, "quadmath coshq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_cosho(o0));
  checkStr(buf0, buf1, "quadmath cosho");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", tanhq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_tanhq(q0));
  checkStr(buf0, buf1, "quadmath tanhq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_tanho(o0));
  checkStr(buf0, buf1, "quadmath tanho");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", asinhq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_asinhq(q0));
  checkStr(buf0, buf1, "quadmath asinhq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_asinho(o0));
  checkStr(buf0, buf1, "quadmath asinho");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", acoshq(q1));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_acoshq(q1));
  checkStr(buf0, buf1, "quadmath acoshq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_acosho(o1));
  checkStr(buf0, buf1, "quadmath acosho");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", atanhq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_atanhq(q0));
  checkStr(buf0, buf1, "quadmath atanhq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_atanho(o0));
  checkStr(buf0, buf1, "quadmath atanho");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", expq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_expq(q0));
  checkStr(buf0, buf1, "quadmath expq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_expo(o0));
  checkStr(buf0, buf1, "quadmath expo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", exp2q(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_exp2q(q0));
  checkStr(buf0, buf1, "quadmath exp2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_exp2o(o0));
  checkStr(buf0, buf1, "quadmath exp2o");

#ifndef TRUE_LIBQUADMATH
  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", exp10q(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_exp10q(q0));
  checkStr(buf0, buf1, "quadmath exp10q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_exp10o(o0));
  checkStr(buf0, buf1, "quadmath exp10o");
#endif

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", expm1q(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_expm1q(q0));
  checkStr(buf0, buf1, "quadmath expm1q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_expm1o(o0));
  checkStr(buf0, buf1, "quadmath expm1o");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", logq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_logq(q0));
  checkStr(buf0, buf1, "quadmath logq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_logo(o0));
  checkStr(buf0, buf1, "quadmath logo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", log2q(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_log2q(q0));
  checkStr(buf0, buf1, "quadmath log2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_log2o(o0));
  checkStr(buf0, buf1, "quadmath log2o");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", log10q(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_log10q(q0));
  checkStr(buf0, buf1, "quadmath log10q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_log10o(o0));
  checkStr(buf0, buf1, "quadmath log10o");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", log1pq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_log1pq(q0));
  checkStr(buf0, buf1, "quadmath log1pq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_log1po(o0));
  checkStr(buf0, buf1, "quadmath log1po");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", powq(q0, q1));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_powq(q0, q1));
  checkStr(buf0, buf1, "quadmath powq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_powo(o0, o1));
  checkStr(buf0, buf1, "quadmath powo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", cbrtq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_cbrtq(q0));
  checkStr(buf0, buf1, "quadmath cbrtq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_cbrto(o0));
  checkStr(buf0, buf1, "quadmath cbrto");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", erfq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_erfq(q0));
  checkStr(buf0, buf1, "quadmath erfq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_erfo(o0));
  checkStr(buf0, buf1, "quadmath erfo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", erfcq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_erfcq(q0));
  checkStr(buf0, buf1, "quadmath erfcq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_erfco(o0));
  checkStr(buf0, buf1, "quadmath erfco");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", tgammaq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_tgammaq(q0));
  checkStr(buf0, buf1, "quadmath tgammaq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_tgammao(o0));
  checkStr(buf0, buf1, "quadmath tgammao");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", lgammaq(q0));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_lgammaq(q0));
  checkStr(buf0, buf1, "quadmath lgammaq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_lgammao(o0));
  checkStr(buf0, buf1, "quadmath lgammao");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", hypotq(q0, q1));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_hypotq(q0, q1));
  checkStr(buf0, buf1, "quadmath hypotq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_hypoto(o0, o1));
  checkStr(buf0, buf1, "quadmath hypoto");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", fmodq(q0, q1));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_fmodq(q0, q1));
  checkStr(buf0, buf1, "quadmath fmodq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_fmodo(o0, o1));
  checkStr(buf0, buf1, "quadmath fmodo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", remainderq(q0, q1));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_remainderq(q0, q1));
  checkStr(buf0, buf1, "quadmath remainderq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_remaindero(o0, o1));
  checkStr(buf0, buf1, "quadmath remaindero");

  int cquo = 0, tquo = 0;
  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", remquoq(q0, q1, &cquo));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_remquoq(q0, q1, &tquo));
  checkStr(buf0, buf1, "quadmath remquoq remainder");
  checkQuo(cquo, tquo, "quadmath remquoq quo");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_remquoo(o0, o1, &tquo));
  checkStr(buf0, buf1, "quadmath remquoo remainder");
  checkQuo(cquo, tquo, "quadmath remquoo quo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", fabsq(q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_fabsq(q2));
  checkStr(buf0, buf1, "quadmath fabsq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_fabso(o2));
  checkStr(buf0, buf1, "quadmath fabso");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", copysignq(q0, q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_copysignq(q0, q2));
  checkStr(buf0, buf1, "quadmath copysignq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_copysigno(o0, o2));
  checkStr(buf0, buf1, "quadmath copysigno");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", fmaxq(q0, q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_fmaxq(q0, q2));
  checkStr(buf0, buf1, "quadmath fmaxq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_fmaxo(o0, o2));
  checkStr(buf0, buf1, "quadmath fmaxo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", fminq(q0, q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_fminq(q0, q2));
  checkStr(buf0, buf1, "quadmath fminq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_fmino(o0, o2));
  checkStr(buf0, buf1, "quadmath fmino");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", fdimq(q0, q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_fdimq(q0, q2));
  checkStr(buf0, buf1, "quadmath fdimq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Og", tlfloat_fdimo(o0, o2));
  checkStr(buf0, buf1, "quadmath fdimo");

  quadmath_snprintf(buf0, sizeof(buf0), "%.24Qg", nextafterq(q0, q2));
  tlfloat_snprintf(buf1, sizeof(buf1), "%.24Qg", tlfloat_nextafterq(q0, q2));
  checkStr(buf0, buf1, "quadmath nextafterq");

#ifdef __STDC_VERSION__
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_Eq);
  checkStr("0x1.5bf0a8b1457695355fb8ac404e7ap+1", buf1, "M_Eq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_LOG2Eq);
  checkStr("0x1.71547652b82fe1777d0ffda0d23ap+0", buf1, "M_LOG2Eq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_LOG10Eq);
  checkStr("0x1.bcb7b1526e50e32a6ab7555f5a68p-2", buf1, "M_LOG10Eq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_LN2q);
  checkStr("0x1.62e42fefa39ef35793c7673007e6p-1", buf1, "M_LN2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_LN10q);
  checkStr("0x1.26bb1bbb5551582dd4adac5705a6p+1", buf1, "M_LN10q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_PIq);
  checkStr("0x1.921fb54442d18469898cc51701b8p+1", buf1, "M_PIq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_PI_2q);
  checkStr("0x1.921fb54442d18469898cc51701b8p+0", buf1, "M_PI_2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_PI_4q);
  checkStr("0x1.921fb54442d18469898cc51701b8p-1", buf1, "M_PI_4q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_1_PIq);
  checkStr("0x1.45f306dc9c882a53f84eafa3ea6ap-2", buf1, "M_1_PIq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_2_PIq);
  checkStr("0x1.45f306dc9c882a53f84eafa3ea6ap-1", buf1, "M_2_PIq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_2_SQRTPIq);
  checkStr("0x1.20dd750429b6d11ae3a914fed7fep+0", buf1, "M_2_SQRTPIq");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_SQRT2q);
  checkStr("0x1.6a09e667f3bcc908b2fb1366ea95p+0", buf1, "M_SQRT2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", M_SQRT1_2q);
  checkStr("0x1.6a09e667f3bcc908b2fb1366ea95p-1", buf1, "M_SQRT1_2q");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", FLT128_MAX);
  checkStr("0x1.ffffffffffffffffffffffffffffp+16383", buf1, "FLT128_MAX");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", FLT128_MIN);
  checkStr("0x1p-16382", buf1, "FLT128_MIN");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", FLT128_DENORM_MIN);
  checkStr("0x1p-16494", buf1, "FLT128_DENORM_MIN");
  tlfloat_snprintf(buf1, sizeof(buf1), "%Qa", FLT128_EPSILON);
  checkStr("0x1p-112", buf1, "FLT128_EPSILON");
#endif

  //

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
  srand(time(NULL));
  for(int i=0;i<100000;i++) {
    doTestInt128(rndi128(), rndi128());
    doTestUInt128(rndu128(), rndu128());
  }
  printf("int128 OK\n");
#endif

  //

  printf("OK\n");
  exit(0);
}
