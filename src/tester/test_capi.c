#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#include <quadmath.h>
#else
#define TLFLOAT_LIBQUADMATH_EMULATION
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

void checkInt(const int u, const int v, const char *mes) {
  if (u != v) {
    printf("NG : %s\n", mes);
    printf("u : %d\n", u);
    printf("v : %d\n", v);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  char buf[1000];
  const char *endptr = NULL, *oneTenth = "0.1";

  tlfloat_snprintf(buf, sizeof(buf), "%.8g", tlfloat_strtod(oneTenth, &endptr));
  checkStr(buf, "0.1", "strtod");

  checkInt(endptr - oneTenth, 3, "tlfloat_strtod endptr");

  tlfloat_snprintf(buf, sizeof(buf), "%.8g", tlfloat_sin(tlfloat_strtod("0.1", NULL)));
  checkStr(buf, "0.099833417", "sin");

  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", tlfloat_strtoq("0.1", NULL));
  checkStr(buf, "0.1", "strtoq");

  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", tlfloat_sinq(tlfloat_strtoq("0.1", NULL)));
  checkStr(buf, "0.099833417", "sinq");

  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", strtoflt128("0.1", NULL));
  checkStr(buf, "0.1", "quadmath strtoflt128");

  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", sinq(strtoflt128("0.1", NULL)));
  checkStr(buf, "0.099833417", "quadmath sinq");

  quadmath_snprintf(buf, sizeof(buf), "%.8Qg", strtoflt128("0.1", NULL));
  checkStr(buf, "0.1", "quadmath_snprintf");

#if defined(__SIZEOF_FLOAT128__) && defined(__GNUC__)
  tlfloat_snprintf(buf, sizeof(buf), "%.8Qg", (__float128)1.0 / (__float128)10.0);
  checkStr(buf, "0.1", "tlfloat_snprintf");
#elif defined(TLFLOAT_LONGDOUBLE_IS_FLOAT128)
  tlfloat_snprintf(buf, sizeof(buf), "%.8Lg", strtold("0.1", NULL));
  checkStr(buf, "0.1", "strtold");
#endif

  tlfloat_snprintf(buf, sizeof(buf), "%.8_256g", tlfloat_strtoo("0.1", NULL));
  checkStr(buf, "0.1", "strtoo");

  tlfloat_snprintf(buf, sizeof(buf), "%.8_256g", tlfloat_sino(tlfloat_strtoo("0.1", NULL)));
  checkStr(buf, "0.099833417", "sino");

  printf("OK\n");
  exit(0);
}
