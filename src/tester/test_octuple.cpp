#include <cstdio>

#include "tlmath.hpp"
#include "tlfloat.h"

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

  snprint(buf, sizeof(buf), AGM<Half>(1), 'g', 0, 5);
  printf("Half (AGM)       : %s\n", buf);

  if (strncmp(buf, "3.1", 3) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Half>(), 'g', 0, 5);
  printf("Half (Machin)    : %s\n", buf);

  if (strncmp(buf, "3.14", 4) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), AGM<Float>(1), 'g', 0, 8);
  printf("Float (AGM)      : %s\n", buf);

  if (strncmp(buf, "3.141592", 8) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Float>(), 'g', 0, 8);
  printf("Float (Machin)   : %s\n", buf);

  if (strncmp(buf, "3.141592", 8) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%64_.15g", AGM<Double>(2));
  printf("Double (AGM)     : %s\n", buf);

  if (strncmp(buf, "3.141592653589", 14) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Double>(), 'g', 0, 15);
  printf("Double (Machin)  : %s\n", buf);

  if (strncmp(buf, "3.141592653589", 14) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%128_.35g", AGM<Quad>(2));
  printf("Quad (AGM)       : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950", 34) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Quad>(), 'g', 0, 35);
  printf("Quad (Machin)    : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950", 34) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  tlfloat_snprintf(buf, sizeof(buf), "%256_.71g", AGM<Octuple>(3));
  printf("Octuple (AGM)    : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  snprint(buf, sizeof(buf), Machin<Octuple>(), 'g', 0, 71);
  printf("Octuple (Machin) : %s\n", buf);

  if (strncmp(buf, "3.14159265358979323846264338327950288419716939937510582097494459230781", 70) != 0) {
    printf("NG\n");
    return -1;
  }

  //

  printf("OK\n");
  return 0;
}
