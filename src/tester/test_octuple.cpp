#include <cstdio>
#include <cmath>

#include "tlfloat/tlmath.hpp"
#include "tlfloat/tlfloat.h"

using namespace std;
using namespace tlfloat;

#if defined(__GLIBC__)
static_assert(Double(const_M_E<Quad>()) == M_E);
static_assert(Double(const_M_E<Octuple>()) == M_E);
static_assert(Double(const_M_LOG2E<Quad>()) == M_LOG2E);
static_assert(Double(const_M_LOG2E<Octuple>()) == M_LOG2E);
static_assert(Double(const_M_LOG10E<Quad>()) == M_LOG10E);
static_assert(Double(const_M_LOG10E<Octuple>()) == M_LOG10E);
static_assert(Double(const_M_LN2<Quad>()) == M_LN2);
static_assert(Double(const_M_LN2<Octuple>()) == M_LN2);
static_assert(Double(const_M_PI<Quad>()) == M_PI);
static_assert(Double(const_M_PI<Octuple>()) == M_PI);
static_assert(Double(const_M_PI<Quad>(-1)) == M_PI_2);
static_assert(Double(const_M_PI<Octuple>(-1)) == M_PI_2);
static_assert(Double(const_M_PI<Quad>(-2)) == M_PI_4);
static_assert(Double(const_M_PI<Octuple>(-2)) == M_PI_4);
static_assert(Double(const_M_1_PI<Quad>()) == M_1_PI);
static_assert(Double(const_M_1_PI<Octuple>()) == M_1_PI);
static_assert(Double(const_M_1_PI<Quad>(1)) == M_2_PI);
static_assert(Double(const_M_1_PI<Octuple>(1)) == M_2_PI);
static_assert(Double(const_M_1_SQRTPI<Quad>(1)) == M_2_SQRTPI);
static_assert(Double(const_M_1_SQRTPI<Octuple>(1)) == M_2_SQRTPI);
#endif

static_assert(Double(const_Euler<Quad>()) == 0.5772156649015328606);
static_assert(Double(const_Euler<Octuple>()) == 0.5772156649015328606);

template<typename T>
T AGM(int N) {
  T y = sqrt(T(2.0)) - 1;
  T a = ldexp(y * y, 1);

  for(int k=0;k<N;k++) {
    y = sqrt(sqrt(T(T(1.0f) - (y * y) * T(y * Octuple(y)))));
    y = T(Quad(1) - y) / T(y + Octuple(1.0f));
    T z = T(Double(1) + y) * T(Half(1.0) + y);
    a = z * z * a - ldexp(T((y + Float(1.0)) * Octuple(y) + 1.0) * y, 2 * k + 3);
  }

  return T(1) / a;
}

template<typename T>
consteval T Machin() {
  return 4*(T("0x0.000000000000000000000000000000004p+132")*atan(T("0.2"))-atan(1/T("0x1de00000000000000000000000000000000p-129")));
}

int main(int argc, char **argv) {
  char buf[1024];

  //

  snprint(buf, sizeof(buf), AGM<Half>(1), 'g', 0, 5);
  printf("Half (AGM)                      : %s\n", buf);

  if (strncmp(buf, "3.1", 3) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Half>(), 'g', 0, 5);
  printf("Half (Machin)                   : %s\n", buf);

  if (strncmp(buf, "3.14", 4) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.5_16g", const_M_PI<Half>());
  printf("Half (const_M_PI<Half>())       : %s\n", buf);

  if (strncmp(buf, "3.14", 4) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.5_16g", const_M_E<Half>());
  printf("Half (const_M_E<Half>())        : %s\n", buf);

  if (strncmp(buf, "2.718", 5) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), AGM<Float>(1), 'g', 0, 8);
  printf("Float (AGM)                     : %s\n", buf);

  if (strncmp(buf, "3.141592", 8) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Float>(), 'g', 0, 9);
  printf("Float (Machin)                  : %s\n", buf);

  if (strncmp(buf, "3.141592", 8) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.16_64g", AGM<Double>(2));
  printf("Double (AGM)                    : %s\n", buf);

  if (strncmp(buf, "3.141592653589", 14) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Double>(), 'g', 0, 16);
  printf("Double (Machin)                 : %s\n", buf);

  if (strncmp(buf, "3.141592653589", 14) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.35_128g", AGM<Quad>(2));
  printf("Quad (AGM)                      : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950", 34) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Quad>(), 'g', 0, 35);
  printf("Quad (Machin)                   : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950", 34) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.37_128g", const_M_PI<Quad>());
  printf("Quad (const_M_PI<Quad>())       : %s\n", buf);

  if (strncmp(buf, "3.141592653589793238462643383279502", 35) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.37_128g", const_M_E<Quad>());
  printf("Quad (const_M_E<Quad>())        : %s\n", buf);

  if (strncmp(buf, "2.718281828459045235360287471352662", 35) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.72_256g", AGM<Octuple>(3));
  printf("Octuple (AGM)                   : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Octuple>(), 'g', 0, 72);
  printf("Octuple (Machin)                : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.72_256g", const_M_PI<Octuple>());
  printf("Octuple (const_M_PI<Octuple>()) : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.72_256g", const_M_E<Octuple>());
  printf("Octuple (const_M_E<Octuple>())  : %s\n", buf);

  if (strncmp(buf, "2.71828182845904523536028747135266249775724709369995957496696762772407", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  if (fabs((double)Double(const_M_LN10<Quad>()) - log(10)) > 1e-8 ||
      fabs((double)Double(const_M_LN10<Octuple>()) - log(10)) > 1e-8) {
    printf("NG : const_M_LN10\n");
    return -1;
  }

  if (sin(BFloat16("0.1")) != sincos(BFloat16("0.1")).first || cos(BFloat16("0.1")) != sincos(BFloat16("0.1")).second) {
    printf("NG : sincos(BFloat16)\n");
    return -1;
  }
  if (sin(Half("0.1")) != sincos(Half("0.1")).first || cos(Half("0.1")) != sincos(Half("0.1")).second) {
    printf("NG : sincos(Half)\n");
    return -1;
  }
  if (sin(Float("0.1")) != sincos(Float("0.1")).first || cos(Float("0.1")) != sincos(Float("0.1")).second) {
    printf("NG : sincos(Float)\n");
    return -1;
  }
  if (sin(Double("0.1")) != sincos(Double("0.1")).first || cos(Double("0.1")) != sincos(Double("0.1")).second) {
    printf("NG : sincos(Double)\n");
    return -1;
  }
  if (sin(Quad("0.1")) != sincos(Quad("0.1")).first || cos(Quad("0.1")) != sincos(Quad("0.1")).second) {
    printf("NG : sincos(Quad)\n");
    return -1;
  }
  if (sin(Octuple("0.1")) != sincos(Octuple("0.1")).first || cos(Octuple("0.1")) != sincos(Octuple("0.1")).second) {
    printf("NG : sincos(Octuple)\n");
    return -1;
  }

  if (sinpi(BFloat16("0.1")) != sincospi(BFloat16("0.1")).first || cospi(BFloat16("0.1")) != sincospi(BFloat16("0.1")).second) {
    printf("NG : sincospi(BFloat16)\n");
    return -1;
  }
  if (sinpi(Half("0.1")) != sincospi(Half("0.1")).first || cospi(Half("0.1")) != sincospi(Half("0.1")).second) {
    printf("NG : sincospi(Half)\n");
    return -1;
  }
  if (sinpi(Float("0.1")) != sincospi(Float("0.1")).first || cospi(Float("0.1")) != sincospi(Float("0.1")).second) {
    printf("NG : sincospi(Float)\n");
    return -1;
  }
  if (sinpi(Double("0.1")) != sincospi(Double("0.1")).first || cospi(Double("0.1")) != sincospi(Double("0.1")).second) {
    printf("NG : sincospi(Double)\n");
    return -1;
  }
  if (sinpi(Quad("0.1")) != sincospi(Quad("0.1")).first || cospi(Quad("0.1")) != sincospi(Quad("0.1")).second) {
    printf("NG : sincospi(Quad)\n");
    return -1;
  }
  if (sinpi(Octuple("0.1")) != sincospi(Octuple("0.1")).first || cospi(Octuple("0.1")) != sincospi(Octuple("0.1")).second) {
    printf("NG : sincospi(Octuple)\n");
    return -1;
  }

  //

  printf("OK\n");
  return 0;
}
