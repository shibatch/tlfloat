#include <iostream>
#include <type_traits>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <suppress.hpp>

#if __cplusplus >= 202002L
#include <tlfloat/tlfloat.hpp>
#endif

#define TLFLOAT_LIBQUADMATH_EMULATION
#include <tlfloat/tlfloat.h>

#include <testerutil.hpp>

using namespace std;
using namespace tlfloat;

static_assert(is_trivially_copyable<tlfloat_quad>::value, "tlfloat_quad not trivially copyable");
static_assert(is_trivially_copyable<tlfloat_octuple>::value, "tlfloat_octuple not trivially copyable");
static_assert(is_trivially_copyable<tlfloat_int128_t>::value, "tlfloat_int128_t not trivially copyable");
static_assert(is_trivially_copyable<tlfloat_uint128_t>::value, "tlfloat_uint128_t not trivially copyable");

extern const tlfloat_quad testvar = M_PIq;

static void check(const string& msg, double x, double y, int t=0) {
  uint64_t u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

#if 0
  printf("%s : ", msg.c_str());
  printf("x : %.18g %016llx, ", x, (unsigned long long)u);
  printf("y : %.18g %016llx\n", y, (unsigned long long)v);
#endif

  if (isnan(x) && isnan(y)) return;

  if (t == 0 && u == v) return;

  int d = int(int64_t(u) - int64_t(v));
  if (-t <= d && d <= t) return;

  cout << "NG " << msg << endl;
  printf("x : %.18g %016llx\n", x, (unsigned long long)u);
  printf("y : %.18g %016llx\n", y, (unsigned long long)v);
  exit(-1);
}

static void check(const string& msg, tlfloat_octuple x, double y, int t=0) {
  check(msg, double(x), y, t);
}

static void check(const string& msg, tlfloat_quad x, double y, int t=0) {
  check(msg, double(x), y, t);
}

static void check(const string& msg, int x, int y) {
  if (x == y) return;

  cout << "NG " << msg << endl;
  printf("x : %d\n", x);
  printf("y : %d\n", y);
  exit(-1);
}

static void check_ilogb(const string& msg, int t, int c) {
  if (t == TLFLOAT_FP_ILOGB0 && c == FP_ILOGB0) return;
  if (t == TLFLOAT_FP_ILOGBNAN && c == FP_ILOGBNAN) return;
  if (t == c) return;

  cout << "NG " << msg << endl;
  printf("t : %d\n", t);
  printf("c : %d\n", c);
  exit(-1);
}

static void check(const string& msg, bool x, bool y) {
  check(msg, (int)x, (int)y);
}

static void checkQuo(const string& msg, int x, int y) {
  int x2 = x & 0x7, y2 = y & 0x7;
  if (x < 0) x2 = -x2;
  if (y < 0) y2 = -y2;
  if (x2 == y2) return;

  cout << "NG " << msg << endl;
  printf("x : %x %d\n", x, x2);
  printf("y : %x %d\n", y, y2);
  exit(-1);
}

static void check(const string& msg, const char* x, const char* y) {
  if (strncmp(x, y, strlen(y)) == 0) return;

  cout << "NG " << msg << endl;
  printf("x : %s\n", x);
  printf("y : %s\n", y);
  exit(-1);
}

static void checkStr(const char *p, const char *q, const char *mes) {
  if (strcmp(p, q) != 0) {
    printf("NG : %s\n", mes);
    printf("p : %s\n", p);
    printf("q : %s\n", q);
    exit(-1);
  }
}

static double o2d(tlfloat_octuple o) { return (double)o; }
static double q2d(tlfloat_quad q) { return (double)q; }

void doTest(const string &s1, const string &s2) {
  tlfloat_octuple o0;
  tlfloat_octuple o1 = tlfloat_strtoo(s1.c_str(), NULL);
  tlfloat_octuple o2 = tlfloat_strtoo(s2.c_str(), NULL);

  tlfloat_octuple q0;
  tlfloat_quad q1 = tlfloat_strtoq(s1.c_str(), NULL);
  tlfloat_quad q2 = strtoflt128(s2.c_str(), NULL);

  double d0;
  double d1 = atof(s1.c_str());
  double d2 = atof(s2.c_str());

  float f1 = d1, f2 = d2;

  int i1 = atoi(s1.c_str());
  int i2 = atoi(s2.c_str());

  tlfloat_int128_t j1 = tlfloat_strtoi128(s1.c_str(), NULL, 10);
  tlfloat_int128_t j2 = tlfloat_strtoi128(s2.c_str(), NULL, 10);

  long long k1 = strtoll(s1.c_str(), NULL, 10);
  long long k2 = strtoll(s2.c_str(), NULL, 10);

  check("tlfloat_stroo", o1, d1);
  check("tlfloat_stroq", q1, d1);

  char strx[256], stry[256];

  snprintf(stry, sizeof(stry), "%.12g %.12g %d\n", d1, d2, 12345678);
  tlfloat_snprintf(strx, sizeof(strx), "%.12_256g %.12_256g %d\n", o1, o2, 12345678);
  check("tlfloat_snprintf tlfloat_octuple", strx, stry);
  tlfloat_snprintf(strx, sizeof(strx), "%.12Qg %.12Qg %d\n", q1, q2, 12345678);
  check("tlfloat_snprintf tlfloat_quad", strx, stry);
  quadmath_snprintf(strx, sizeof(strx), "%.12Qg %.12Qg %d\n", q1, q2, 12345678);
  check("quadmath_snprintf tlfloat_quad", strx, stry);

  check("double(tlfloat_octuple(d1))", double(tlfloat_octuple(d1)), d1);
  check("double(tlfloat_quad(d1))", double(tlfloat_quad(d1)), d1);

  check("tlfloat_octuple(q1)", (double)tlfloat_octuple(q1), d1);
  check("tlfloat_octuple(d1)", (double)tlfloat_octuple(d1), d1);
  check("tlfloat_octuple(f1)", (double)tlfloat_octuple(f1), d1);
  check("tlfloat_octuple(i1)", (double)tlfloat_octuple(i1), (double)i1);
  check("tlfloat_octuple(j1)", (double)tlfloat_octuple(j1), (double)j1);
  check("tlfloat_octuple(k1)", (double)tlfloat_octuple(k1), (double)k1);

  check("tlfloat_quad(o1)", (double)tlfloat_quad(o1), d1);
  check("tlfloat_quad(d1)", (double)tlfloat_quad(d1), d1);
  check("tlfloat_quad(f1)", (double)tlfloat_quad(f1), d1);
  check("tlfloat_quad(i1)", (double)tlfloat_quad(i1), (double)i1);
  check("tlfloat_quad(j1)", (double)tlfloat_quad(j1), (double)j1);
  check("tlfloat_quad(k1)", (double)tlfloat_quad(k1), (double)k1);

  check("tlfloat_int128_t(o1)", (double)tlfloat_int128_t(o1), (double)o1);
  check("o2d(i1)", o2d(i1), (double)i1);
  check("o2d(j1)", o2d(j1), (double)j1);
  check("o2d(k1)", o2d(k1), (double)k1);
  check("tlfloat_int128_t(q1)", (double)tlfloat_int128_t(q1), (double)q1);
  check("o2d(q1)", o2d(q1), (double)q1);
  check("q2d(d1)", q2d(d1), d1);
  check("q2d(i1)", q2d(i1), (double)i1);

  check("o1 + o2", o1 + o2, d1 + d2);
  check("d1 + o2", d1 + o2, d1 + d2);
  check("f1 + o2", f1 + o2, d1 + d2);
  check("o1 + f2", o1 + f2, d1 + d2);
  check("i1 + o2", i1 + o2, i1 + d2);
  check("j1 + o2", j1 + o2, (double)j1 + d2);
  check("k1 + o2", k1 + o2, (double)k1 + d2);
  check("o1 + i2", o1 + i2, d1 + i2);
  check("o1 + j2", o1 + j2, d1 + (double)j2);
  check("o1 + k2", o1 + k2, d1 + (double)k2);
  o0 = o1; o0 += o2; check("o0 = o1; o0 += o2;", o0, d1 + d2);
  o0 = o1; o0 += q2; check("o0 = o1; o0 += q2;", o0, d1 + d2);
  o0 = o1; o0 += d2; check("o0 = o1; o0 += d2;", o0, d1 + d2);
  o0 = o1; o0 += i2; check("o0 = o1; o0 += i2;", o0, d1 + i2);
  o0 = o1; o0 += j2; check("o0 = o1; o0 += j2;", o0, d1 + (double)j2);
  o0 = o1; o0 += k2; check("o0 = o1; o0 += k2;", o0, d1 + (double)k2);

  o0 = o1; d0 = d1; check("o0 = o1; o0 ++;", o0++, d0++); check("o0 = o1; o0 ++; (2)", o0, d0);
  o0 = o1; d0 = d1; check("o0 = o1; ++ o0;", ++o0, ++d0); check("o0 = o1; ++ o0; (2)", o0, d0);

  check("o1 - o2", o1 - o2, d1 - d2);
  check("d1 - o2", d1 - o2, d1 - d2);
  check("f1 - o2", f1 - o2, d1 - d2);
  check("o1 - f2", o1 - f2, d1 - d2);
  check("i1 - o2", i1 - o2, i1 - d2);
  check("j1 - o2", j1 - o2, (double)j1 - d2);
  check("k1 - o2", k1 - o2, (double)k1 - d2);
  check("o1 - i2", o1 - i2, d1 - i2);
  check("o1 - j2", o1 - j2, d1 - (double)j2);
  check("o1 - k2", o1 - k2, d1 - (double)k2);
  o0 = o1; o0 -= o2; check("o0 = o1; o0 -= o2;", o0, d1 - d2);
  o0 = o1; o0 -= q2; check("o0 = o1; o0 -= q2;", o0, d1 - d2);
  o0 = o1; o0 -= d2; check("o0 = o1; o0 -= d2;", o0, d1 - d2);
  o0 = o1; o0 -= i2; check("o0 = o1; o0 -= i2;", o0, d1 - i2);
  o0 = o1; o0 -= j2; check("o0 = o1; o0 -= j2;", o0, d1 - (double)j2);
  o0 = o1; o0 -= k2; check("o0 = o1; o0 -= k2;", o0, d1 - (double)k2);

  o0 = o1; d0 = d1; check("o0 = o1; o0 --;", o0--, d0--); check("o0 = o1; o0 --; (2)", o0, d0);
  o0 = o1; d0 = d1; check("o0 = o1; -- o0;", --o0, --d0); check("o0 = o1; -- o0; (2)", o0, d0);

  check("o1 * o2", o1 * o2, d1 * d2);
  check("d1 * o2", d1 * o2, d1 * d2);
  check("f1 * o2", f1 * o2, d1 * d2);
  check("o1 * f2", o1 * f2, d1 * d2);
  check("i1 * o2", i1 * o2, i1 * d2);
  check("j1 * o2", j1 * o2, (double)j1 * d2);
  check("k1 * o2", k1 * o2, (double)k1 * d2);
  check("o1 * i2", o1 * i2, d1 * i2);
  check("o1 * j2", o1 * j2, d1 * (double)j2);
  check("o1 * k2", o1 * k2, d1 * (double)k2);
  o0 = o1; o0 *= o2; check("o0 = o1; o0 *= o2;", o0, d1 * d2);
  o0 = o1; o0 *= q2; check("o0 = o1; o0 *= q2;", o0, d1 * d2);
  o0 = o1; o0 *= d2; check("o0 = o1; o0 *= d2;", o0, d1 * d2);
  o0 = o1; o0 *= i2; check("o0 = o1; o0 *= i2;", o0, d1 * i2);
  o0 = o1; o0 *= j2; check("o0 = o1; o0 *= j2;", o0, d1 * (double)j2);
  o0 = o1; o0 *= k2; check("o0 = o1; o0 *= k2;", o0, d1 * (double)k2);

  check("o1 / o2", o1 / o2, d1 / d2);
  check("d1 / o2", d1 / o2, d1 / d2);
  check("f1 / o2", f1 / o2, d1 / d2);
  check("o1 / f2", o1 / f2, d1 / d2);
  check("i1 / o2", i1 / o2, i1 / d2);
  check("j1 / o2", j1 / o2, (double)j1 / d2);
  check("k1 / o2", k1 / o2, (double)k1 / d2);
  check("o1 / i2", o1 / i2, d1 / i2);
  check("o1 / j2", o1 / j2, d1 / (double)j2);
  check("o1 / k2", o1 / k2, d1 / (double)k2);
  o0 = o1; o0 /= o2; check("o0 = o1; o0 /= o2;", o0, d1 / d2);
  o0 = o1; o0 /= q2; check("o0 = o1; o0 /= q2;", o0, d1 / d2);
  o0 = o1; o0 /= d2; check("o0 = o1; o0 /= d2;", o0, d1 / d2);
  o0 = o1; o0 /= i2; check("o0 = o1; o0 /= i2;", o0, d1 / i2);
  o0 = o1; o0 /= j2; check("o0 = o1; o0 /= j2;", o0, d1 / (double)j2);
  o0 = o1; o0 /= k2; check("o0 = o1; o0 /= k2;", o0, d1 / (double)k2);

  check("q1 + q2", q1 + q2, d1 + d2);
  check("d1 + q2", d1 + q2, d1 + d2);
  check("f1 + q2", f1 + q2, d1 + d2);
  check("q1 + f2", q1 + f2, d1 + d2);
  check("i1 + q2", i1 + q2, i1 + d2);
  check("j1 + q2", (tlfloat_quad)j1 + q2, (double)j1 + d2);
  check("k1 + q2", (tlfloat_quad)k1 + q2, (double)k1 + d2);
  check("q1 + i2", q1 + i2, d1 + i2);
  check("q1 + j2", q1 + (tlfloat_quad)j2, d1 + (double)j2);
  check("q1 + k2", q1 + (tlfloat_quad)k2, d1 + (double)k2);
  q0 = q1; q0 += o2; check("q0 = q1; q0 += o2;", q0, d1 + d2);
  q0 = q1; q0 += q2; check("q0 = q1; q0 += q2;", q0, d1 + d2);
  q0 = q1; q0 += d2; check("q0 = q1; q0 += d2;", q0, d1 + d2);
  q0 = q1; q0 += i2; check("q0 = q1; q0 += i2;", q0, d1 + i2);
  q0 = q1; q0 += j2; check("q0 = q1; q0 += j2;", q0, d1 + (double)j2);
  q0 = q1; q0 += k2; check("q0 = q1; q0 += k2;", q0, d1 + (double)k2);

  check("q1 - q2", q1 - q2, d1 - d2);
  check("d1 - q2", d1 - q2, d1 - d2);
  check("f1 - q2", f1 - q2, d1 - d2);
  check("q1 - f2", q1 - f2, d1 - d2);
  check("i1 - q2", i1 - q2, i1 - d2);
  check("j1 - q2", (tlfloat_quad)j1 - q2, (double)j1 - d2);
  check("k1 - q2", (tlfloat_quad)k1 - q2, (double)k1 - d2);
  check("q1 - i2", q1 - i2, d1 - i2);
  check("q1 - j2", q1 - (tlfloat_quad)j2, d1 - (double)j2);
  check("q1 - k2", q1 - (tlfloat_quad)k2, d1 - (double)k2);
  q0 = q1; q0 -= o2; check("q0 = q1; q0 -= o2;", q0, d1 - d2);
  q0 = q1; q0 -= q2; check("q0 = q1; q0 -= q2;", q0, d1 - d2);
  q0 = q1; q0 -= d2; check("q0 = q1; q0 -= d2;", q0, d1 - d2);
  q0 = q1; q0 -= i2; check("q0 = q1; q0 -= i2;", q0, d1 - i2);
  q0 = q1; q0 -= j2; check("q0 = q1; q0 -= j2;", q0, d1 - (double)j2);
  q0 = q1; q0 -= k2; check("q0 = q1; q0 -= k2;", q0, d1 - (double)k2);

  check("q1 * q2", q1 * q2, d1 * d2);
  check("d1 * q2", d1 * q2, d1 * d2);
  check("f1 * q2", f1 * q2, d1 * d2);
  check("q1 * f2", q1 * f2, d1 * d2);
  check("i1 * q2", i1 * q2, i1 * d2);
  check("j1 * q2", (tlfloat_quad)j1 * q2, (double)j1 * d2);
  check("k1 * q2", (tlfloat_quad)k1 * q2, (double)k1 * d2);
  check("q1 * i2", q1 * i2, d1 * i2);
  check("q1 * j2", q1 * (tlfloat_quad)j2, d1 * (double)j2);
  check("q1 * k2", q1 * (tlfloat_quad)k2, d1 * (double)k2);
  q0 = q1; q0 *= o2; check("q0 = q1; q0 *= o2;", q0, d1 * d2);
  q0 = q1; q0 *= q2; check("q0 = q1; q0 *= q2;", q0, d1 * d2);
  q0 = q1; q0 *= d2; check("q0 = q1; q0 *= d2;", q0, d1 * d2);
  q0 = q1; q0 *= i2; check("q0 = q1; q0 *= i2;", q0, d1 * i2);
  q0 = q1; q0 *= j2; check("q0 = q1; q0 *= j2;", q0, d1 * (double)j2);
  q0 = q1; q0 *= k2; check("q0 = q1; q0 *= k2;", q0, d1 * (double)k2);

  check("q1 / q2", q1 / q2, d1 / d2);
  check("d1 / q2", d1 / q2, d1 / d2);
  check("f1 / q2", f1 / q2, d1 / d2);
  check("q1 / f2", q1 / f2, d1 / d2);
  check("i1 / q2", i1 / q2, i1 / d2);
  check("j1 / q2", (tlfloat_quad)j1 / q2, (double)j1 / d2);
  check("k1 / q2", (tlfloat_quad)k1 / q2, (double)k1 / d2);
  check("q1 / i2", q1 / i2, d1 / i2);
  check("q1 / j2", q1 / (tlfloat_quad)j2, d1 / (double)j2);
  check("q1 / k2", q1 / (tlfloat_quad)k2, d1 / (double)k2);
  q0 = q1; q0 /= o2; check("q0 = q1; q0 /= o2;", q0, d1 / d2);
  q0 = q1; q0 /= q2; check("q0 = q1; q0 /= q2;", q0, d1 / d2);
  q0 = q1; q0 /= d2; check("q0 = q1; q0 /= d2;", q0, d1 / d2);
  q0 = q1; q0 /= i2; check("q0 = q1; q0 /= i2;", q0, d1 / i2);
  q0 = q1; q0 /= j2; check("q0 = q1; q0 /= j2;", q0, d1 / (double)j2);
  q0 = q1; q0 /= k2; check("q0 = q1; q0 /= k2;", q0, d1 / (double)k2);

  const int T = 4;

  check("tlfloat_sqrto(o1)", tlfloat_sqrto(o1), sqrt(d1), T);
  check("sqrt_(o1)", sqrt_(o1), sqrt(d1), T);
  check("tlfloat_fabso(o1)", tlfloat_fabso(o1), fabs(d1), T);
  check("fabs_(o1)", fabs_(o1), fabs(d1), T);
  check("tlfloat_trunco(o1)", tlfloat_trunco(o1), trunc(d1), T);
  check("trunc_(o1)", trunc_(o1), trunc(d1), T);
  check("tlfloat_flooro(o1)", tlfloat_flooro(o1), floor(d1), T);
  check("floor_(o1)", floor_(o1), floor(d1), T);
  check("tlfloat_ceilo(o1)", tlfloat_ceilo(o1), ceil(d1), T);
  check("ceil_(o1)", ceil_(o1), ceil(d1), T);
  check("tlfloat_roundo(o1)", tlfloat_roundo(o1), round(d1), T);
  check("round_(o1)", round_(o1), round(d1), T);
  check("tlfloat_rinto(o1)", tlfloat_rinto(o1), rint(d1), T);
  check("rint_(o1)", rint_(o1), rint(d1), T);
  check("tlfloat_sino(o1)", tlfloat_sino(o1), sin(d1), T);
  check("sin_(o1)", sin_(o1), sin(d1), T);
  check("tlfloat_coso(o1)", tlfloat_coso(o1), cos(d1), T);
  check("cos_(o1)", cos_(o1), cos(d1), T);
  check("tlfloat_tano(o1)", tlfloat_tano(o1), tan(d1), T);
  check("tan_(o1)", tan_(o1), tan(d1), T);
  if (trunc(d1*2) != d1*2) {
    check("tlfloat_sinpio(o1)", tlfloat_sinpio(o1), sin(M_PI*d1), T);
    check("sinpi_(o1)", sinpi_(o1), sin(M_PI*d1), T);
    check("tlfloat_cospio(o1)", tlfloat_cospio(o1), cos(M_PI*d1), T);
    check("cospi_(o1)", cospi_(o1), cos(M_PI*d1), T);
    check("tlfloat_tanpio(o1)", tlfloat_tanpio(o1), tan(M_PI*d1), T);
    check("tanpi_(o1)", tanpi_(o1), tan(M_PI*d1), T);
  }
  check("tlfloat_asino(o1)", tlfloat_asino(o1), asin(d1), T);
  check("asin_(o1)", asin_(o1), asin(d1), T);
  check("tlfloat_acoso(o1)", tlfloat_acoso(o1), acos(d1), T);
  check("acos_(o1)", acos_(o1), acos(d1), T);
  check("tlfloat_atano(o1)", tlfloat_atano(o1), atan(d1), T);
  check("atan_(o1)", atan_(o1), atan(d1), T);
  check("tlfloat_sinho(o1)", tlfloat_sinho(o1), sinh(d1), T);
  check("sinh_(o1)", sinh_(o1), sinh(d1), T);
  check("tlfloat_cosho(o1)", tlfloat_cosho(o1), cosh(d1), T);
  check("cosh_(o1)", cosh_(o1), cosh(d1), T);
  check("tlfloat_tanho(o1)", tlfloat_tanho(o1), tanh(d1), T);
  check("tanh_(o1)", tanh_(o1), tanh(d1), T);
  check("tlfloat_asinho(o1)", tlfloat_asinho(o1), asinh(d1), T);
  check("asinh_(o1)", asinh_(o1), asinh(d1), T);
  check("tlfloat_acosho(o1)", tlfloat_acosho(o1), acosh(d1), T);
  check("acosh_(o1)", acosh_(o1), acosh(d1), T);
  check("tlfloat_atanho(o1)", tlfloat_atanho(o1), atanh(d1), T);
  check("atanh_(o1)", atanh_(o1), atanh(d1), T);
  check("tlfloat_erfo(o1)", tlfloat_erfo(o1), erf(d1), T);
  check("erf_(o1)", erf_(o1), erf(d1), T);
  check("tlfloat_erfco(o1)", tlfloat_erfco(o1), erfc(d1), T);
  check("erfc_(o1)", erfc_(o1), erfc(d1), T);
  check("tlfloat_tgammao(o1)", tlfloat_tgammao(o1), tgamma(d1), T);
  check("tgamma_(o1)", tgamma_(o1), tgamma(d1), T);
  check("tlfloat_expo(o1)", tlfloat_expo(o1), exp(d1), T);
  check("exp_(o1)", exp_(o1), exp(d1), T);
  check("tlfloat_exp2o(o1)", tlfloat_exp2o(o1), exp2(d1), T);
  check("exp2_(o1)", exp2_(o1), exp2(d1), T);
#ifdef _GNU_SOURCE
  check("tlfloat_exp10o(o1)", tlfloat_exp10o(o1), exp10(d1), T);
  check("exp10_(o1)", exp10_(o1), exp10(d1), T);
#endif
  check("tlfloat_expm1o(o1)", tlfloat_expm1o(o1), expm1(d1), T);
  check("expm1_(o1)", expm1_(o1), expm1(d1), T);
  check("tlfloat_logo(o1)", tlfloat_logo(o1), log(d1), T);
  check("log_(o1)", log_(o1), log(d1), T);
  check("tlfloat_log2o(o1)", tlfloat_log2o(o1), log2(d1), T);
  check("log2_(o1)", log2_(o1), log2(d1), T);
  check("tlfloat_log10o(o1)", tlfloat_log10o(o1), log10(d1), T);
  check("log10_(o1)", log10_(o1), log10(d1), T);
  check("tlfloat_logp1o(o1)", tlfloat_log1po(o1), log1p(d1), T);
  check("logp1_(o1)", log1p_(o1), log1p(d1), T);
  check("tlfloat_cbrto(o1)", tlfloat_cbrto(o1), cbrt(d1), T);
  check("cbrt_(o1)", cbrt_(o1), cbrt(d1), T);
  check("tlfloat_copysigno(o1, o2)", tlfloat_copysigno(o1, o2), copysign(d1, d2), T);
  check("copysign_(o1, o2)", copysign_(o1, o2), copysign(d1, d2), T);
  check("tlfloat_fmaxo(o1, o2)", tlfloat_fmaxo(o1, o2), fmax(d1, d2), T);
  check("fmax_(o1, o2)", fmax_(o1, o2), fmax(d1, d2), T);
  check("tlfloat_fmino(o1, o2)", tlfloat_fmino(o1, o2), fmin(d1, d2), T);
  check("fmin_(o1, o2)", fmin_(o1, o2), fmin(d1, d2), T);
  check("tlfloat_fdimo(o1, o2)", tlfloat_fdimo(o1, o2), fdim(d1, d2), T);
  check("fdim_(o1, o2)", fdim_(o1, o2), fdim(d1, d2), T);
  check("tlfloat_nextaftero(o1, o2)", tlfloat_nextaftero(o1, o2), nextafter(d1, d2), T);
  check("nextafter_(o1, o2)", nextafter_(o1, o2), nextafter(d1, d2), T);
  check("tlfloat_hypoto(o1, o2)", tlfloat_hypoto(o1, o2), hypot(d1, d2), T);
  check("hypot_(o1, o2)", hypot_(o1, o2), hypot(d1, d2), T);
  check("tlfloat_fmodo(o1, o2)", tlfloat_fmodo(o1, o2), fmod(d1, d2), T);
  check("fmod_(o1, o2)", fmod_(o1, o2), fmod(d1, d2), T);
  check("tlfloat_remaindero(o1, o2)", tlfloat_remaindero(o1, o2), remainder(d1, d2), T);
  check("remainder_(o1, o2)", remainder_(o1, o2), remainder(d1, d2), T);
  int qt = 0, qc = 0;
#ifndef __i386__
  check("tlfloat_remquoo(o1, o2, &quo) remainder", tlfloat_remquoo(o1, o2, &qt), remquo(d1, d2, &qc), T);
  check("remquo_(o1, o2, &quo) remainder", remquo_(o1, o2, &qt), remquo(d1, d2, &qc), T);
  checkQuo("tlfloat_remquoo(o1, o2, &quo) quo", qt, qc);
#endif
  check_ilogb("tlfloat_ilogbo(o1)", tlfloat_ilogbo(o1), ilogb(d1));
  check_ilogb("ilogb_(o1)", ilogb_(o1), ilogb(d1));
  check("tlfloat_isnano(o1)", (bool)tlfloat_isnano(o1), (bool)isnan(d1));
  check("isnan_(o1)", (bool)isnan_(o1), (bool)isnan(d1));
  check("tlfloat_isinfo(o1)", (bool)tlfloat_isinfo(o1), (bool)isinf(d1));
  check("isinf_(o1)", (bool)isinf_(o1), (bool)isinf(d1));
  check("tlfloat_finiteo(o1)", (bool)tlfloat_finiteo(o1), (bool)finite__(d1));
  check("finite_(o1)", (bool)finite_(o1), (bool)finite__(d1));
  check("tlfloat_signbito(o1)", (bool)tlfloat_signbito(o1), (bool)signbit(d1));
  check("signbit_(o1)", (bool)signbit_(o1), (bool)signbit(d1));
  check("tlfloat_ldexpo(o1, i2)", tlfloat_ldexpo(o1, i2), ldexp(d1, i2), T);
  check("ldexp_(o1, i2)", ldexp_(o1, i2), ldexp(d1, i2), T);
  check("tlfloat_frexpo(o1, &qt)", tlfloat_frexpo(o1, &qt), frexp(d1, &qc), T);
  check("frexp_(o1, &qt)", frexp_(o1, &qt), frexp(d1, &qc), T);
  if (!isnan(d1) && !isinf(d1)) check("exp in tlfloat_frexpo(o1, &qt)", qt, qc);
  check("tlfloat_powo(o1, o2)", tlfloat_powo(o1, o2), pow(d1, d2), T);
  check("pow_(o1, o2)", pow_(o1, o2), pow(d1, d2), T);
  check("tlfloat_atan2o(o1, o2)", tlfloat_atan2o(o1, o2), atan2(d1, d2), T);
  check("atan2_(o1, o2)", atan2_(o1, o2), atan2(d1, d2), T);
  check("tlfloat_fmao(o1, o2, o2)", tlfloat_fmao(o1, o2, o2), fma(d1, d2, d2), T);
  check("fma_(o1, o2, o2)", fma_(o1, o2, o2), fma(d1, d2, d2), T);

  check("tlfloat_sqrtq(q1)", tlfloat_sqrtq(q1), sqrt(d1), T);
  check("sqrt_(q1)", sqrt_(q1), sqrt(d1), T);
  check("tlfloat_fabsq(q1)", tlfloat_fabsq(q1), fabs(d1), T);
  check("fabs_(q1)", fabs_(q1), fabs(d1), T);
  check("tlfloat_truncq(q1)", tlfloat_truncq(q1), trunc(d1), T);
  check("trunc_(q1)", trunc_(q1), trunc(d1), T);
  check("tlfloat_floorq(q1)", tlfloat_floorq(q1), floor(d1), T);
  check("floor_(q1)", floor_(q1), floor(d1), T);
  check("tlfloat_ceilq(q1)", tlfloat_ceilq(q1), ceil(d1), T);
  check("ceil_(q1)", ceil_(q1), ceil(d1), T);
  check("tlfloat_roundq(q1)", tlfloat_roundq(q1), round(d1), T);
  check("round_(q1)", round_(q1), round(d1), T);
  check("tlfloat_rintq(q1)", tlfloat_rintq(q1), rint(d1), T);
  check("rint_(q1)", rint_(q1), rint(d1), T);
  check("tlfloat_sinq(q1)", tlfloat_sinq(q1), sin(d1), T);
  check("sin_(q1)", sin_(q1), sin(d1), T);
  check("tlfloat_cosq(q1)", tlfloat_cosq(q1), cos(d1), T);
  check("cos_(q1)", cos_(q1), cos(d1), T);
  check("tlfloat_tanq(q1)", tlfloat_tanq(q1), tan(d1), T);
  check("tan_(q1)", tan_(q1), tan(d1), T);
  if (trunc(d1*2) != d1*2) {
    check("tlfloat_sinpiq(q1)", tlfloat_sinpiq(q1), sin(M_PI*d1), T);
    check("sinpi_(q1)", sinpi_(q1), sin(M_PI*d1), T);
    check("tlfloat_cospiq(q1)", tlfloat_cospiq(q1), cos(M_PI*d1), T);
    check("cospi_(q1)", cospi_(q1), cos(M_PI*d1), T);
    check("tlfloat_tanpiq(q1)", tlfloat_tanpiq(q1), tan(M_PI*d1), T);
    check("tanpi_(q1)", tanpi_(q1), tan(M_PI*d1), T);
  }
  check("tlfloat_asinq(q1)", tlfloat_asinq(q1), asin(d1), T);
  check("asin_(q1)", asin_(q1), asin(d1), T);
  check("tlfloat_acosq(q1)", tlfloat_acosq(q1), acos(d1), T);
  check("acos_(q1)", acos_(q1), acos(d1), T);
  check("tlfloat_atanq(q1)", tlfloat_atanq(q1), atan(d1), T);
  check("atan_(q1)", atan_(q1), atan(d1), T);
  check("tlfloat_sinhq(q1)", tlfloat_sinhq(q1), sinh(d1), T);
  check("sinh_(q1)", sinh_(q1), sinh(d1), T);
  check("tlfloat_coshq(q1)", tlfloat_coshq(q1), cosh(d1), T);
  check("cosh_(q1)", cosh_(q1), cosh(d1), T);
  check("tlfloat_tanhq(q1)", tlfloat_tanhq(q1), tanh(d1), T);
  check("tanh_(q1)", tanh_(q1), tanh(d1), T);
  check("tlfloat_asinhq(q1)", tlfloat_asinhq(q1), asinh(d1), T);
  check("asinh_(q1)", asinh_(q1), asinh(d1), T);
  check("tlfloat_acoshq(q1)", tlfloat_acoshq(q1), acosh(d1), T);
  check("acosh_(q1)", acosh_(q1), acosh(d1), T);
  check("tlfloat_atanhq(q1)", tlfloat_atanhq(q1), atanh(d1), T);
  check("atanh_(q1)", atanh_(q1), atanh(d1), T);
  check("tlfloat_erfq(q1)", tlfloat_erfq(q1), erf(d1), T);
  check("erf_(q1)", erf_(q1), erf(d1), T);
  check("tlfloat_erfcq(q1)", tlfloat_erfcq(q1), erfc(d1), T);
  check("erfc_(q1)", erfc_(q1), erfc(d1), T);
  check("tlfloat_tgammaq(q1)", tlfloat_tgammaq(q1), tgamma(d1), T);
  check("tgamma_(q1)", tgamma_(q1), tgamma(d1), T);
  check("tlfloat_expq(q1)", tlfloat_expq(q1), exp(d1), T);
  check("exp_(q1)", exp_(q1), exp(d1), T);
  check("tlfloat_exp2q(q1)", tlfloat_exp2q(q1), exp2(d1), T);
  check("exp2_(q1)", exp2_(q1), exp2(d1), T);
#ifdef _GNU_SOURCE
  check("tlfloat_exp10q(q1)", tlfloat_exp10q(q1), exp10(d1), T);
  check("exp10_(q1)", exp10_(q1), exp10(d1), T);
#endif
  check("tlfloat_expm1q(q1)", tlfloat_expm1q(q1), expm1(d1), T);
  check("expm1_(q1)", expm1_(q1), expm1(d1), T);
  check("tlfloat_logq(q1)", tlfloat_logq(q1), log(d1), T);
  check("log_(q1)", log_(q1), log(d1), T);
  check("tlfloat_log2q(q1)", tlfloat_log2q(q1), log2(d1), T);
  check("log2_(q1)", log2_(q1), log2(d1), T);
  check("tlfloat_log10q(q1)", tlfloat_log10q(q1), log10(d1), T);
  check("log10_(q1)", log10_(q1), log10(d1), T);
  check("tlfloat_logp1q(q1)", tlfloat_log1pq(q1), log1p(d1), T);
  check("logp1_(q1)", log1p_(q1), log1p(d1), T);
  check("tlfloat_cbrtq(q1)", tlfloat_cbrtq(q1), cbrt(d1), T);
  check("cbrt_(q1)", cbrt_(q1), cbrt(d1), T);
  check("tlfloat_copysignq(q1, q2)", tlfloat_copysignq(q1, q2), copysign(d1, d2), T);
  check("copysign_(q1, q2)", copysign_(q1, q2), copysign(d1, d2), T);
  check("tlfloat_fmaxq(q1, q2)", tlfloat_fmaxq(q1, q2), fmax(d1, d2), T);
  check("fmax_(q1, q2)", fmax_(q1, q2), fmax(d1, d2), T);
  check("tlfloat_fminq(q1, q2)", tlfloat_fminq(q1, q2), fmin(d1, d2), T);
  check("fmin_(q1, q2)", fmin_(q1, q2), fmin(d1, d2), T);
  check("tlfloat_fdimq(q1, q2)", tlfloat_fdimq(q1, q2), fdim(d1, d2), T);
  check("fdim_(q1, q2)", fdim_(q1, q2), fdim(d1, d2), T);
  check("tlfloat_nextafterq(q1, q2)", tlfloat_nextafterq(q1, q2), nextafter(d1, d2), T);
  check("nextafter_(q1, q2)", nextafter_(q1, q2), nextafter(d1, d2), T);
  check("tlfloat_hypotq(q1, q2)", tlfloat_hypotq(q1, q2), hypot(d1, d2), T);
  check("hypot_(q1, q2)", hypot_(q1, q2), hypot(d1, d2), T);
  check("tlfloat_fmodq(q1, q2)", tlfloat_fmodq(q1, q2), fmod(d1, d2), T);
  check("fmod_(q1, q2)", fmod_(q1, q2), fmod(d1, d2), T);
  check("tlfloat_remainderq(q1, q2)", tlfloat_remainderq(q1, q2), remainder(d1, d2), T);
  check("remainder_(q1, q2)", remainder_(q1, q2), remainder(d1, d2), T);
  qt = qc = 0;
#ifndef __i386__
  check("tlfloat_remquoq(q1, q2, &quo) remainder", tlfloat_remquoq(q1, q2, &qt), remquo(d1, d2, &qc), T);
  check("remquo_(q1, q2, &quo) remainder", remquo_(q1, q2, &qt), remquo(d1, d2, &qc), T);
  checkQuo("tlfloat_remquoq(q1, q2, &quo) quo", qt, qc);
#endif
  check_ilogb("tlfloat_ilogbq(q1)", tlfloat_ilogbq(q1), ilogb(d1));
  check_ilogb("ilogb_(q1)", ilogb_(q1), ilogb(d1));
  check("tlfloat_isnanq(q1)", (bool)tlfloat_isnanq(q1), (bool)isnan(d1));
  check("isnan_(q1)", (bool)isnan_(q1), (bool)isnan(d1));
  check("tlfloat_isinfq(q1)", (bool)tlfloat_isinfq(q1), (bool)isinf(d1));
  check("isinf_(q1)", (bool)isinf_(q1), (bool)isinf(d1));
  check("tlfloat_finiteq(q1)", (bool)tlfloat_finiteq(q1), (bool)finite__(d1));
  check("finite_(q1)", (bool)finite_(q1), (bool)finite__(d1));
  check("tlfloat_signbitq(q1)", (bool)tlfloat_signbitq(q1), (bool)signbit(d1));
  check("signbit_(q1)", (bool)signbit_(q1), (bool)signbit(d1));
  check("tlfloat_ldexpq(q1, i2)", tlfloat_ldexpq(q1, i2), ldexp(d1, i2), T);
  check("ldexp_(q1, i2)", ldexp_(q1, i2), ldexp(d1, i2), T);
  check("tlfloat_frexpq(q1, &qt)", tlfloat_frexpq(q1, &qt), frexp(d1, &qc), T);
  check("frexp_(q1, &qt)", frexp_(q1, &qt), frexp(d1, &qc), T);
  if (!isnan(d1) && !isinf(d1)) check("exp in tlfloat_frexpq(q1, &qt)", qt, qc);
  check("tlfloat_powq(q1, q2)", tlfloat_powq(q1, q2), pow(d1, d2), T);
  check("pow_(q1, q2)", pow_(q1, q2), pow(d1, d2), T);
  check("tlfloat_atan2q(q1, q2)", tlfloat_atan2q(q1, q2), atan2(d1, d2), T);
  check("atan2_(q1, q2)", atan2_(q1, q2), atan2(d1, d2), T);
  check("tlfloat_fmaq(q1, q2, q2)", tlfloat_fmaq(q1, q2, q2), fma(d1, d2, d2), T);
  check("fma_(q1, q2, q2)", fma_(q1, q2, q2), fma(d1, d2, d2), T);

  check("sqrtq(q1)", sqrtq(q1), sqrt(d1), T);
  check("fabsq(q1)", fabsq(q1), fabs(d1), T);
  check("truncq(q1)", truncq(q1), trunc(d1), T);
  check("floorq(q1)", floorq(q1), floor(d1), T);
  check("ceilq(q1)", ceilq(q1), ceil(d1), T);
  check("roundq(q1)", roundq(q1), round(d1), T);
  check("rintq(q1)", rintq(q1), rint(d1), T);
  check("sinq(q1)", sinq(q1), sin(d1), T);
  check("cosq(q1)", cosq(q1), cos(d1), T);
  {
    tlfloat_quad s, c;
    sincosq(q1, &s, &c);
    check("sin in sincosq(q1)", s, sin(d1), T);
    check("cos in sincosq(q1)", c, cos(d1), T);
  }
  check("tanq(q1)", tanq(q1), tan(d1), T);
  check("asinq(q1)", asinq(q1), asin(d1), T);
  check("acosq(q1)", acosq(q1), acos(d1), T);
  check("atanq(q1)", atanq(q1), atan(d1), T);
  check("sinhq(q1)", sinhq(q1), sinh(d1), T);
  check("coshq(q1)", coshq(q1), cosh(d1), T);
  check("tanhq(q1)", tanhq(q1), tanh(d1), T);
  check("asinhq(q1)", asinhq(q1), asinh(d1), T);
  check("acoshq(q1)", acoshq(q1), acosh(d1), T);
  check("atanhq(q1)", atanhq(q1), atanh(d1), T);
  check("erfq(q1)", erfq(q1), erf(d1), T);
  check("erfcq(q1)", erfcq(q1), erfc(d1), T);
  check("tgammaq(q1)", tgammaq(q1), tgamma(d1), T);
  //check("lgammaq(q1)", lgammaq(q1), lgamma(d1), T);
  check("expq(q1)", expq(q1), exp(d1), T);
  check("exp2q(q1)", exp2q(q1), exp2(d1), T);
#ifdef _GNU_SOURCE
  check("exp10q(q1)", exp10q(q1), exp10(d1), T);
#endif
  check("expm1q(q1)", expm1q(q1), expm1(d1), T);
  check("logq(q1)", logq(q1), log(d1), T);
  check("log2q(q1)", log2q(q1), log2(d1), T);
  check("log10q(q1)", log10q(q1), log10(d1), T);
  check("logp1q(q1)", log1pq(q1), log1p(d1), T);
  check("cbrtq(q1)", cbrtq(q1), cbrt(d1), T);
  check("copysignq(q1, q2)", copysignq(q1, q2), copysign(d1, d2), T);
  check("fmaxq(q1, q2)", fmaxq(q1, q2), fmax(d1, d2), T);
  check("fminq(q1, q2)", fminq(q1, q2), fmin(d1, d2), T);
  check("fdimq(q1, q2)", fdimq(q1, q2), fdim(d1, d2), T);
  check("nextafterq(q1, q2)", nextafterq(q1, q2), nextafter(d1, d2), T);
  check("hypotq(q1, q2)", hypotq(q1, q2), hypot(d1, d2), T);
  check("fmodq(q1, q2)", fmodq(q1, q2), fmod(d1, d2), T);
  check("remainderq(q1, q2)", remainderq(q1, q2), remainder(d1, d2), T);
  qt = qc = 0;
#ifndef __i386__
  check("remquoq(q1, q2, &quo) remainder", remquoq(q1, q2, &qt), remquo(d1, d2, &qc), T);
  checkQuo("remquoq(q1, q2, &quo) quo", qt, qc);
#endif
  check_ilogb("ilogbq(q1)", ilogbq(q1), ilogb(d1));
  check("isnanq(q1)", (bool)isnanq(q1), (bool)isnan(d1));
  check("isinfq(q1)", (bool)isinfq(q1), (bool)isinf(d1));
  check("finiteq(q1)", (bool)finiteq(q1), (bool)finite_(d1));
  check("signbitq(q1)", (bool)signbitq(q1), (bool)signbit(d1));
  check("ldexpq(q1, i2)", ldexpq(q1, i2), ldexp(d1, i2), T);
  check("powq(q1, q2)", powq(q1, q2), pow(d1, d2), T);
  check("atan2q(q1, q2)", atan2q(q1, q2), atan2(d1, d2), T);
  check("fmaq(q1, q2, q2)", fmaq(q1, q2, q2), fma(d1, d2, d2), T);

  check("+o1 == +o2", +o1 == +o2, +d1 == +d2);
  check("-o1 == -o2", -o1 == -o2, -d1 == -d2);

  check("o1 == o2", o1 == o2, d1 == d2);
  check("o1 != o2", o1 != o2, d1 != d2);
  check("o1 > o2", o1 > o2, d1 > d2);
  check("o1 >= o2", o1 >= o2, d1 >= d2);
  check("o1 < o2", o1 < o2, d1 < d2);
  check("o1 <= o2", o1 <= o2, d1 <= d2);

  check("o1 == q2", o1 == q2, d1 == d2);
  check("o1 != q2", o1 != q2, d1 != d2);
  check("o1 > q2", o1 > q2, d1 > d2);
  check("o1 >= q2", o1 >= q2, d1 >= d2);
  check("o1 < q2", o1 < q2, d1 < d2);
  check("o1 <= q2", o1 <= q2, d1 <= d2);

  check("o1 == d2", o1 == d2, d1 == d2);
  check("o1 != d2", o1 != d2, d1 != d2);
  check("o1 > d2", o1 > d2, d1 > d2);
  check("o1 >= d2", o1 >= d2, d1 >= d2);
  check("o1 < d2", o1 < d2, d1 < d2);
  check("o1 <= d2", o1 <= d2, d1 <= d2);

  check("o1 == i2", o1 == i2, d1 == i2);
  check("o1 != i2", o1 != i2, d1 != i2);
  check("o1 > i2", o1 > i2, d1 > i2);
  check("o1 >= i2", o1 >= i2, d1 >= i2);
  check("o1 < i2", o1 < i2, d1 < i2);
  check("o1 <= i2", o1 <= i2, d1 <= i2);

  check("o1 == j2", o1 == j2, d1 == (double)j2);
  check("o1 != j2", o1 != j2, d1 != (double)j2);
  check("o1 > j2", o1 > j2, d1 > (double)j2);
  check("o1 >= j2", o1 >= j2, d1 >= (double)j2);
  check("o1 < j2", o1 < j2, d1 < (double)j2);
  check("o1 <= j2", o1 <= j2, d1 <= (double)j2);

  check("o1 == k2", o1 == k2, d1 == (double)k2);
  check("o1 != k2", o1 != k2, d1 != (double)k2);
  check("o1 > k2", o1 > k2, d1 > (double)k2);
  check("o1 >= k2", o1 >= k2, d1 >= (double)k2);
  check("o1 < k2", o1 < k2, d1 < (double)k2);
  check("o1 <= k2", o1 <= k2, d1 <= (double)k2);

  check("+q1 == +q2", +q1 == +q2, +d1 == +d2);
  check("-q1 == -q2", -q1 == -q2, -d1 == -d2);

  check("q1 == o2", q1 == o2, d1 == d2);
  check("q1 != o2", q1 != o2, d1 != d2);
  check("q1 > o2", q1 > o2, d1 > d2);
  check("q1 >= o2", q1 >= o2, d1 >= d2);
  check("q1 < o2", q1 < o2, d1 < d2);
  check("q1 <= o2", q1 <= o2, d1 <= d2);

  check("q1 == q2", q1 == q2, d1 == d2);
  check("q1 != q2", q1 != q2, d1 != d2);
  check("q1 > q2", q1 > q2, d1 > d2);
  check("q1 >= q2", q1 >= q2, d1 >= d2);
  check("q1 < q2", q1 < q2, d1 < d2);
  check("q1 <= q2", q1 <= q2, d1 <= d2);

  check("q1 == d2", q1 == d2, d1 == d2);
  check("q1 != d2", q1 != d2, d1 != d2);
  check("q1 > d2", q1 > d2, d1 > d2);
  check("q1 >= d2", q1 >= d2, d1 >= d2);
  check("q1 < d2", q1 < d2, d1 < d2);
  check("q1 <= d2", q1 <= d2, d1 <= d2);

  check("q1 == i2", q1 == i2, d1 == i2);
  check("q1 != i2", q1 != i2, d1 != i2);
  check("q1 > i2", q1 > i2, d1 > i2);
  check("q1 >= i2", q1 >= i2, d1 >= i2);
  check("q1 < i2", q1 < i2, d1 < i2);
  check("q1 <= i2", q1 <= i2, d1 <= i2);

  check("q1 == j2", q1 == (tlfloat_quad)j2, d1 == (double)j2);
  check("q1 != j2", q1 != (tlfloat_quad)j2, d1 != (double)j2);
  check("q1 > j2", q1 > (tlfloat_quad)j2, d1 > (double)j2);
  check("q1 >= j2", q1 >= (tlfloat_quad)j2, d1 >= (double)j2);
  check("q1 < j2", q1 < (tlfloat_quad)j2, d1 < (double)j2);
  check("q1 <= j2", q1 <= (tlfloat_quad)j2, d1 <= (double)j2);

  check("q1 == k2", q1 == (tlfloat_quad)k2, d1 == (double)k2);
  check("q1 != k2", q1 != (tlfloat_quad)k2, d1 != (double)k2);
  check("q1 > k2", q1 > (tlfloat_quad)k2, d1 > (double)k2);
  check("q1 >= k2", q1 >= (tlfloat_quad)k2, d1 >= (double)k2);
  check("q1 < k2", q1 < (tlfloat_quad)k2, d1 < (double)k2);
  check("q1 <= k2", q1 <= (tlfloat_quad)k2, d1 <= (double)k2);

#if __cplusplus >= 202002L
  o1 = (tlfloat_octuple)tlfloat::Octuple(s1.c_str());
  check("cast Octuple -> tlfloat_octuple", o1, d1);
  check("cast tlfloat_octuple -> Octuple", isnan(tlfloat::Octuple(o1)) || tlfloat::Octuple(o1) == tlfloat::Octuple(s1.c_str()), true);

  q1 = (tlfloat_quad)tlfloat::Quad(s1.c_str());
  check("cast Quad -> tlfloat_quad", q1, d1);
  check("cast tlfloat_quad -> Quad", isnan(tlfloat::Quad(q1)) || tlfloat::Quad(q1) == tlfloat::Quad(s1.c_str()), true);
#endif
}

tlfloat_octuple AGMo(int N) {
  tlfloat_octuple y = tlfloat_sqrto(2.0) - 1;
  tlfloat_octuple a = tlfloat_ldexpo(y * y, 1);

  for(int k=0;k<N;k++) {
    y = tlfloat_sqrto(tlfloat_sqrto(tlfloat_octuple(tlfloat_octuple(1.0f) - (y * y) * tlfloat_octuple(y * tlfloat_octuple(y)))));
    y = tlfloat_octuple(tlfloat_quad(1) - y) / tlfloat_octuple(y + tlfloat_octuple(1.0f));
    tlfloat_octuple z = tlfloat_octuple(1.0 + y) * tlfloat_octuple(1.0f + y);
    a = z * z * a - tlfloat_ldexpo(tlfloat_octuple((y + 1.0f) * tlfloat_octuple(y) + 1.0) * y, 2 * k + 3);
  }

  return tlfloat_octuple(1) / a;
}

tlfloat_quad AGMq(int N) {
  tlfloat_quad y = tlfloat_quad(tlfloat_sqrto(2.0) - 1);
  tlfloat_quad a = tlfloat_ldexpq(y * y, 1);

  for(int k=0;k<N;k++) {
    y = tlfloat_sqrtq((tlfloat_quad)tlfloat_sqrto(tlfloat_quad(tlfloat_quad(1.0f) - (y * y) * tlfloat_quad(y * tlfloat_octuple(y)))));
    y = tlfloat_quad(tlfloat_octuple(1) - y) / tlfloat_quad(y + tlfloat_quad(1.0f));
    tlfloat_quad z = tlfloat_quad(1.0 + y) * tlfloat_quad(1.0f + y);
    a = z * z * a - (tlfloat_quad)tlfloat_ldexpo(tlfloat_quad((y + 1.0f) * tlfloat_quad(y) + 1.0) * y, 2 * k + 3);
  }

  return tlfloat_quad(1) / a;
}

int main(int argc, char **argv) {
  const char* s[] = {
    "+.5", "-.5", "+.875", "-.875", "+1.1875", "-1.1875", "65536", "-65536", ".00390625", "-.00390625", "+1", "-1",
    "36893488147419103232", "-36893488147419103232", "nan", "+0.0", "-0.0", "inf", "-inf"
  };
  const int N = sizeof(s) / sizeof(s[0]);

  for(int i=0;i<N;i++) {
    for(int j=0;j<N;j++) {
      doTest(s[i], s[j]);
    }
  }

  char str[1000];

  tlfloat_snprintf(str, sizeof(str), "%.70_256g\n", AGMo(3));
  check("AGMo", str, "3.141592653589793238462643383279502884197169399375105820974944592307818");

  tlfloat_snprintf(str, sizeof(str), "%.35Qg\n", AGMq(2));
  check("AGMq", str, "3.14159265358979323846264338327950");

  check("AGMo string", tlfloat::to_string(AGMo(3), 70).c_str(), "3.141592653589793238462643383279502884197169399375105820974944592307818");
  check("AGMq string", tlfloat::to_string(AGMq(2), 35).c_str(), "3.14159265358979323846264338327950");

  tlfloat_snprintf(str, sizeof(str), "%Qa", (-M_Eq) + M_PIq);
  checkStr("0x1.b1786497ead779a14ea0c6b599fp-2", str, "(-M_Eq) + M_PIq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_Eq);
  checkStr("0x1.5bf0a8b1457695355fb8ac404e7ap+1", str, "M_Eq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_LOG2Eq);
  checkStr("0x1.71547652b82fe1777d0ffda0d23ap+0", str, "M_LOG2Eq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_LOG10Eq);
  checkStr("0x1.bcb7b1526e50e32a6ab7555f5a68p-2", str, "M_LOG10Eq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_LN2q);
  checkStr("0x1.62e42fefa39ef35793c7673007e6p-1", str, "M_LN2q");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_LN10q);
  checkStr("0x1.26bb1bbb5551582dd4adac5705a6p+1", str, "M_LN10q");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_PIq);
  checkStr("0x1.921fb54442d18469898cc51701b8p+1", str, "M_PIq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_PI_2q);
  checkStr("0x1.921fb54442d18469898cc51701b8p+0", str, "M_PI_2q");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_PI_4q);
  checkStr("0x1.921fb54442d18469898cc51701b8p-1", str, "M_PI_4q");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_1_PIq);
  checkStr("0x1.45f306dc9c882a53f84eafa3ea6ap-2", str, "M_1_PIq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_2_PIq);
  checkStr("0x1.45f306dc9c882a53f84eafa3ea6ap-1", str, "M_2_PIq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_2_SQRTPIq);
  checkStr("0x1.20dd750429b6d11ae3a914fed7fep+0", str, "M_2_SQRTPIq");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_SQRT2q);
  checkStr("0x1.6a09e667f3bcc908b2fb1366ea95p+0", str, "M_SQRT2q");
  tlfloat_snprintf(str, sizeof(str), "%Qa", M_SQRT1_2q);
  checkStr("0x1.6a09e667f3bcc908b2fb1366ea95p-1", str, "M_SQRT1_2q");
  tlfloat_snprintf(str, sizeof(str), "%Qa", FLT128_MAX);
  checkStr("0x1.ffffffffffffffffffffffffffffp+16383", str, "FLT128_MAX");
  tlfloat_snprintf(str, sizeof(str), "%Qa", FLT128_MIN);
  checkStr("0x1p-16382", str, "FLT128_MIN");
  tlfloat_snprintf(str, sizeof(str), "%Qa", FLT128_DENORM_MIN);
  checkStr("0x1p-16494", str, "FLT128_DENORM_MIN");
  tlfloat_snprintf(str, sizeof(str), "%Qa", FLT128_EPSILON);
  checkStr("0x1p-112", str, "FLT128_EPSILON");

  tlfloat_snprintf(str, sizeof(str), "%Oa", (-TLFLOAT_M_Eo) + TLFLOAT_M_PIo);
  checkStr("0x1.b1786497ead779a14ea0c6b599edfdf50488884bc3a95de582a886526a8p-2", str, "(-TLFLOAT_M_Eo) + TLFLOAT_M_PIo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", (-TLFLOAT_M_Eo) + TLFLOAT_M_PIq);
  checkStr("0x1.b1786497ead779a14ea0c6b599ec30e27463a7c31d2c96a561ec9beeb98p-2", str, "(-TLFLOAT_M_Eo) + TLFLOAT_M_PIq");
  tlfloat_snprintf(str, sizeof(str), "%Oa", (-TLFLOAT_M_Eq) + TLFLOAT_M_PIo);
  checkStr("0x1.b1786497ead779a14ea0c6b599f1cd129024e088a67cc74020bbea63b1p-2" , str, "(-TLFLOAT_M_Eq) + TLFLOAT_M_PIo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_Eo);
  checkStr("0x1.5bf0a8b1457695355fb8ac404e7a79e3b1738b079c5a6d2b53c26c8228dp+1", str, "TLFLOAT_M_Eo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_LOG2Eo);
  checkStr("0x1.71547652b82fe1777d0ffda0d23a7d11d6aef551bad2b4b1164a2cd9a34p+0", str, "TLFLOAT_M_LOG2Eo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_LOG10Eo);
  checkStr("0x1.bcb7b1526e50e32a6ab7555f5a67b8647dc68c048b934404747e5a89ef2p-2", str, "TLFLOAT_M_LOG10Eo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_LN2o);
  checkStr("0x1.62e42fefa39ef35793c7673007e5ed5e81e6864ce5316c5b141a2eb7175p-1", str, "TLFLOAT_M_LN2o");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_LN10o);
  checkStr("0x1.26bb1bbb5551582dd4adac5705a61451c51fd9f3b4bbf21d078c3d0403ep+1", str, "TLFLOAT_M_LN10o");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_PIo);
  checkStr("0x1.921fb54442d18469898cc51701b839a252049c1114cf98e804177d4c762p+1", str, "TLFLOAT_M_PIo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_PI_2o);
  checkStr("0x1.921fb54442d18469898cc51701b839a252049c1114cf98e804177d4c762p+0", str, "TLFLOAT_M_PI_2o");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_PI_4o);
  checkStr("0x1.921fb54442d18469898cc51701b839a252049c1114cf98e804177d4c762p-1", str, "TLFLOAT_M_PI_4o");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_1_PIo);
  checkStr("0x1.45f306dc9c882a53f84eafa3ea69bb81b6c52b3278872083fca2c757bd7p-2", str, "TLFLOAT_M_1_PIo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_2_PIo);
  checkStr("0x1.45f306dc9c882a53f84eafa3ea69bb81b6c52b3278872083fca2c757bd7p-1", str, "TLFLOAT_M_2_PIo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_2_SQRTPIo);
  checkStr("0x1.20dd750429b6d11ae3a914fed7fd8688281341d7587cea2e7342b06199dp+0", str, "TLFLOAT_M_2_SQRTPIo");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_SQRT2o);
  checkStr("0x1.6a09e667f3bcc908b2fb1366ea957d3e3adec17512775099da2f590b066p+0", str, "TLFLOAT_M_SQRT2o");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_M_SQRT1_2o);
  checkStr("0x1.6a09e667f3bcc908b2fb1366ea957d3e3adec17512775099da2f590b066p-1", str, "TLFLOAT_M_SQRT1_2o");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_FLT256_MAX);
  checkStr("0x1.fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffp+262143", str, "TLFLOAT_FLT256_MAX");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_FLT256_MIN);
  checkStr("0x1p-262142", str, "TLFLOAT_FLT256_MIN");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_FLT256_DENORM_MIN);
  checkStr("0x1p-262378", str, "TLFLOAT_FLT256_DENORM_MIN");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_FLT256_TRUE_MIN);
  checkStr("0x1p-262378", str, "TLFLOAT_FLT256_TRUE_MIN");
  tlfloat_snprintf(str, sizeof(str), "%Oa", TLFLOAT_FLT256_EPSILON);
  checkStr("0x1p-236", str, "TLFLOAT_FLT256_EPSILON");

  cout << "OK" << endl;

  return 0;
}
