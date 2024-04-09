#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#include <quadmath.h>
#define LIBQUADMATH
#endif

#include "tlfloat.h"

void checkStr(const char *p, const char *q, const char *mes) {
  if (strcmp(p, q) != 0) {
    printf("NG : %s\n", mes);
    printf("p : %s\n", p);
    printf("q : %s\n", q);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  char buf[1000];

  tlfloat_snprintf(buf, sizeof(buf), "%.8g", tlfloat_strtod("0.1", NULL));
  checkStr(buf, "0.1", "strtod");

  tlfloat_snprintf(buf, sizeof(buf), "%.8g", tlfloat_sin(tlfloat_strtod("0.1", NULL)));
  checkStr(buf, "0.099833417", "sin");

  tlfloat_snprintf(buf, sizeof(buf), "%128_.8g", tlfloat_strtoq("0.1", NULL));
  checkStr(buf, "0.1", "strtoq");

  tlfloat_snprintf(buf, sizeof(buf), "%128_.8g", tlfloat_sinq(tlfloat_strtoq("0.1", NULL)));
  checkStr(buf, "0.099833417", "sinq");

#ifdef LIBQUADMATH
  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", strtoflt128("0.1", NULL));
  checkStr(buf, "0.1", "strtoflt128");

  tlfloat_snprintf(buf, sizeof(buf), "%128_.8g", tlfloat_cast_q_f128(strtoflt128("0.1", NULL)));
  checkStr(buf, "0.1", "tlfloat_cast_q_f128");

  quadmath_snprintf(buf, sizeof(buf), "%.8Qg", tlfloat_cast_f128_q(tlfloat_strtoq("0.1", NULL)));
  checkStr(buf, "0.1", "tlfloat_cast_f128_q");
#elif defined(__SIZEOF_FLOAT128__) && defined(__GNUC__)
  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", (__float128)1.0 / (__float128)10.0);
  checkStr(buf, "0.1", "tlfloat_snprintf");

  tlfloat_snprintf(buf, sizeof(buf), "%128_.8g", tlfloat_cast_q_f128((__float128)1.0 / (__float128)10.0));
  checkStr(buf, "0.1", "tlfloat_cast_q_f128");

  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", tlfloat_cast_f128_q(tlfloat_strtoq("0.1", NULL)));
  checkStr(buf, "0.1", "tlfloat_cast_f128_q");
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)
  tlfloat_snprintf(buf, sizeof(buf), "%.8Lg", strtold("0.1", NULL));
  checkStr(buf, "0.1", "strtold");

  tlfloat_snprintf(buf, sizeof(buf), "%128_.8g", tlfloat_cast_q_f128(strtold("0.1", NULL)));
  checkStr(buf, "0.1", "tlfloat_cast_q_f128");

  snprintf(buf, sizeof(buf), "%.8Lg", tlfloat_cast_f128_q(tlfloat_strtoq("0.1", NULL)));
  checkStr(buf, "0.1", "tlfloat_cast_f128_q");
#endif

  tlfloat_snprintf(buf, sizeof(buf), "%256_.8g", tlfloat_strtoo("0.1", NULL));
  checkStr(buf, "0.1", "strtoo");

  tlfloat_snprintf(buf, sizeof(buf), "%256_.8g", tlfloat_sino(tlfloat_strtoo("0.1", NULL)));
  checkStr(buf, "0.099833417", "sino");

  printf("OK\n");
  exit(0);
}
