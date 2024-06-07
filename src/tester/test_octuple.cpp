#include <cstdio>

#include "tlfloat/tlmath.hpp"
#include "tlfloat/tlfloat.h"

using namespace std;
using namespace tlfloat;

template<typename T>
T AGM(int N) {
  T y = sqrt(T(2.0)) - 1;
  T a = ldexp_(y * y, 1);

  for(int k=0;k<N;k++) {
    y = sqrt(sqrt(T(T(1.0f) - (y * y) * T(y * Octuple(y)))));
    y = T(Quad(1) - y) / T(y + Octuple(1.0f));
    T z = T(Double(1) + y) * T(Half(1.0) + y);
    a = z * z * a - ldexp_(T((y + Float(1.0)) * Octuple(y) + 1.0) * y, 2 * k + 3);
  }

  return T(1) / a;
}

template<typename T>
consteval T Machin() {
  return 4*(T(4)*atan(T("0.2"))-atan(1/T(239)));
}

int main(int argc, char **argv) {
  char buf[1024];

  //

  snprint(buf, sizeof(buf), AGM<Half>(1), 'g', 0, 5);
  printf("Half (AGM)                : %s\n", buf);

  if (strncmp(buf, "3.1", 3) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Half>(), 'g', 0, 5);
  printf("Half (Machin)             : %s\n", buf);

  if (strncmp(buf, "3.14", 4) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.5_16g", const_M_PI<Half>());
  printf("Half (const_M_PI<Half>()) : %s\n", buf);

  if (strncmp(buf, "3.14", 4) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.5_16g", const_M_E<Half>());
  printf("Half (const_M_E<Half>())  : %s\n", buf);

  if (strncmp(buf, "2.718", 5) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), AGM<Float>(1), 'g', 0, 8);
  printf("Float (AGM)               : %s\n", buf);

  if (strncmp(buf, "3.141592", 8) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Float>(), 'g', 0, 9);
  printf("Float (Machin)            : %s\n", buf);

  if (strncmp(buf, "3.141592", 8) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.16_64g", AGM<Double>(2));
  printf("Double (AGM)              : %s\n", buf);

  if (strncmp(buf, "3.141592653589", 14) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Double>(), 'g', 0, 16);
  printf("Double (Machin)           : %s\n", buf);

  if (strncmp(buf, "3.141592653589", 14) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.35_128g", AGM<Quad>(2));
  printf("Quad (AGM)                : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950", 34) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Quad>(), 'g', 0, 35);
  printf("Quad (Machin)             : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950", 34) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.37_128g", const_M_PIq());
  printf("Quad (const_M_PIq())      : %s\n", buf);

  if (strncmp(buf, "3.141592653589793238462643383279502", 35) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.37_128g", const_M_Eq());
  printf("Quad (const_M_Eq())       : %s\n", buf);

  if (strncmp(buf, "2.718281828459045235360287471352662", 35) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.72_256g", AGM<Octuple>(3));
  printf("Octuple (AGM)             : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Octuple>(), 'g', 0, 72);
  printf("Octuple (Machin)          : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.72_256g", const_M_PIo());
  printf("Octuple (const_M_PIo())   : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%.72_256g", const_M_Eo());
  printf("Octuple (const_M_Eo())    : %s\n", buf);

  if (strncmp(buf, "2.71828182845904523536028747135266249775724709369995957496696762772407", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  printf("OK\n");
  return 0;
}
