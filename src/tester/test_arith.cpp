#include <iostream>
#include <utility>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <climits>

#include "suppress.hpp"

#include "tlfloat/tlfloat.hpp"
#include "testerutil.hpp"

#if defined(TLFLOAT_COMPILER_SUPPORTS_INT128) && defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#include <quadmath.h>
#endif

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

static_assert(is_trivially_copyable_v<Float> == true);
static_assert(is_trivially_copyable_v<Double> == true);
static_assert(is_trivially_copyable_v<Quad> == true);
static_assert(is_trivially_copyable_v<Octuple> == true);

static_assert(Float::flt_min() == FLT_MIN);
static_assert(Float::flt_true_min() == FLT_TRUE_MIN);
static_assert(Float::flt_max() == FLT_MAX);
static_assert(Float::flt_epsilon() == FLT_EPSILON);

static_assert(Double::flt_min() == DBL_MIN);
static_assert(Double::flt_true_min() == DBL_TRUE_MIN);
static_assert(Double::flt_max() == DBL_MAX);
static_assert(Double::flt_epsilon() == DBL_EPSILON);

static void check(const char* msg, double x, double y) {
  if (isnan(x) && isnan(y)) return;

  uint64_t u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

  if (u == v) return;

  cout << "NG " << msg << endl;
  printf("x : %.18g %016llx\n", x, (unsigned long long)u);
  printf("y : %.18g %016llx\n", y, (unsigned long long)v);
  exit(-1);
}

static void check(const char* msg, Octuple x, Octuple y) { check(msg, (double)x, (double)y); }

int main(int argc, char **argv) {
  uint64_t niter = 100000;

  if (argc >= 2) niter = strtoull(argv[1], nullptr, 10);

  if (niter == 0) {
#if defined(__GNUC__)
    cout << "Build " << __DATE__ << " " << __TIME__ << endl;
#endif
    cout << "Testing continuously" << endl;
  }

  typedef UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;
  typedef UnpackedFloat<uint64_t, BigUInt<7>, 11, 52> udouble;

  typedef UnpackedFloat<uint32_t, uint64_t, 0, 30> xfloat;
  typedef UnpackedFloat<uint64_t, BigUInt<7>, 0, 62> xdouble;

  shared_ptr<RNG> rng = createPreferredRNG();

  for(uint64_t i=0;niter == 0 || i<niter;i++) {
    float f1 = rndf(rng), f2 = rndf(rng), f3 = rndf(rng);
    ufloat tlf1(f1), tlf2(f2), tlf3(f3);

    double d1 = rndd(rng), d2 = rndd(rng), d3 = rndd(rng);
    udouble tld1(d1), tld2(d2), tld3(d3);

#ifdef ENABLE_QUAD
    quad q1 = rndq(rng), q2 = rndq(rng), q3 = rndq(rng);
    uquad tlq1(q1), tlq2(q2), tlq3(q3);
#endif

    //

    float f4t = float(ufloat::addsub(tlf1, tlf2, false));
    float f4c = f1 + f2;

    if (!cmpf(f4t, f4c)) {
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2: " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "+t: " << f4t << " : " << to_string_d(ufloat(f4t)) << endl;
      cout << "+c: " << f4c << " : " << to_string_d(ufloat(f4c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    float f5t = float(ufloat::addsub(tlf1, tlf2, true));
    float f5c = f1 - f2;

    if (!cmpf(f5t, f5c)) {
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2: " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "-t: " << f5t << " : " << to_string_d(ufloat(f5t)) << endl;
      cout << "-c: " << f5c << " : " << to_string_d(ufloat(f5c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    float f6t = float(ufloat::mul(tlf1, tlf2));
    float f6c = f1 * f2;

    if (!cmpf(f6t, f6c)) {
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2: " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "*t: " << f6t << " : " << to_string_d(ufloat(f6t)) << endl;
      cout << "*c: " << f6c << " : " << to_string_d(ufloat(f6c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    float f7t = float(ufloat::div(tlf1, tlf2));
    float f7c = f1 / f2;

    if (!cmpf(f7t, f7c)) {
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2: " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "/t: " << f7t << " : " << to_string_d(ufloat(f7t)) << endl;
      cout << "/c: " << f7c << " : " << to_string_d(ufloat(f7c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlf1 == tlf2) != (f1 == f2)) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "==t: " << (tlf1 == tlf2) << endl;
      cout << "==c: " << (f1 == f2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlf1 != tlf2) != (f1 != f2)) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "!=t: " << (tlf1 != tlf2) << endl;
      cout << "!=c: " << (f1 != f2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlf1 < tlf2) != (f1 < f2)) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "< t: " << (tlf1 < tlf2) << endl;
      cout << "< c: " << (f1 < f2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlf1 > tlf2) != (f1 > f2)) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "> t: " << (tlf1 > tlf2) << endl;
      cout << "> c: " << (f1 > f2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlf1 <= tlf2) != (f1 <= f2)) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "<=t: " << (tlf1 <= tlf2) << endl;
      cout << "<=c: " << (f1 <= f2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlf1 >= tlf2) != (f1 >= f2)) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << ">=t: " << (tlf1 >= tlf2) << endl;
      cout << ">=c: " << (f1 >= f2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

#if defined(__GLIBC__) || defined(FP_FAST_FMA)
    float f8t = float(ufloat::fma(tlf1, tlf2, tlf3));
    float f8c = fmaf(f1, f2, f3);

    if (!cmpf(f8t, f8c)) {
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1 )) << endl;
      cout << "f2: " << f2  << " : " << to_string_d(ufloat(f2 )) << endl;
      cout << "f3: " << f3  << " : " << to_string_d(ufloat(f3 )) << endl;
      cout << "ft: " << f8t << " : " << to_string_d(ufloat(f8t)) << endl;
      cout << "fc: " << f8c << " : " << to_string_d(ufloat(f8c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

    float f9t = float(tld1.cast((const ufloat *)nullptr));
    float f9c = d1;

    if (!cmpf(f9t, f9c)) {
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "ct: " << f9t << " : " << to_string_d(ufloat(f9t)) << endl;
      cout << "cc: " << f9c << " : " << to_string_d(ufloat(f9c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

#ifdef ENABLE_QUAD
    float fat = float(tlq1.cast((const ufloat *)nullptr));
    float fac = q1;

    if (!cmpf(fat, fac)) {
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "ct: " << fat << " : " << to_string_d(ufloat(fat)) << endl;
      cout << "cc: " << fac << " : " << to_string_d(ufloat(fac)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

    if ((i & 31) == 0 && fabs(f1) > 1e-30 && fabs(f1) < 1e+30) {
      float fbt = (float)Float(to_string(Float(tlf1), 9).c_str());
      float fbc = (float)tlf1;

      if (!cmpf(fbt, fbc, 7)) {
	cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
	cout << "s : " << to_string(Float(tlf1), 9) << endl;
	cout << "ct: " << fbt << " : " << to_string_d(ufloat(fbt)) << endl;
	cout << "cc: " << fbc << " : " << to_string_d(ufloat(fbc)) << endl;
	cout << "NG" << endl;
	exit(-1);
      }
    }

    float fc, ft, fn = f1;

    fc = truncf(fn);
    ft = (float)trunc(Float(fn).getUnpacked());

    if (!cmpf(fc, ft)) {
      printf("\ntruncf\nfn = %.10g, fc = %.10g, ft = %.10g\n", fn, fc, ft);
      exit(-1);
    }

    fc = floorf(fn);
    ft = (float)floor(Float(fn).getUnpacked());

    if (!cmpf(fc, ft)) {
      printf("\nfloorf\nfn = %.10g, fc = %.10g, ft = %.10g\n", fn, fc, ft);
      exit(-1);
    }

    fc = ceilf(fn);
    ft = (float)ceil(Float(fn).getUnpacked());

    if (!cmpf(fc, ft)) {
      printf("\nceilf\nfn = %.10g, fc = %.10g, ft = %.10g\n", fn, fc, ft);
      exit(-1);
    }

    fc = roundf(fn);
    ft = (float)round(Float(fn).getUnpacked());

    if (!cmpf(fc, ft)) {
      printf("\nroundf\nfn = %.10g, fc = %.10g, ft = %.10g\n", fn, fc, ft);
      exit(-1);
    }

    fc = rintf(fn);
    ft = (float)rint(Float(fn).getUnpacked());

    if (!cmpf(fc, ft)) {
      printf("\nrintf\nfn = %.10g, fc = %.10g, ft = %.10g\n", fn, fc, ft);
      exit(-1);
    }

    //

    if (isnanf(f1) != isnan(Float(f1))) {
      printf("\nisnanf f1 = %.10g\n", f1);
      exit(-1);
    }

    if (isinff(f1) != isinf(Float(f1))) {
      printf("\nisinff f1 = %.10g\n", f1);
      exit(-1);
    }

    if (finitef(f1) != finite(Float(f1))) {
      printf("\nfinitef f1 = %.10g\n", f1);
      exit(-1);
    }

    //

    double d1t = double(tlf1.cast((const udouble *)nullptr));
    double d1c = f1;

    if (!cmpd(d1t, d1c)) {
      cout << "cast float to double" << endl;
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "ct: " << d1t << " : " << to_string_d(udouble(d1t)) << endl;
      cout << "cc: " << d1c << " : " << to_string_d(udouble(d1c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= 0 && f1 < ldexp(1, 16) && tlf1.castToInt((const uint16_t *)nullptr) != uint16_t(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "ust: " << d1t << " : " << tlf1.castToInt((const uint16_t *)nullptr) << endl;
      cout << "usc: " << d1c << " : " << uint16_t(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= -ldexp(1, 15) && f1 < ldexp(1, 15) && tlf1.castToInt((const int16_t *)nullptr) != int16_t(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "sst: " << d1t << " : " << tlf1.castToInt((const int16_t *)nullptr) << endl;
      cout << "ssc: " << d1c << " : " << int16_t(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= 0 && f1 < ldexp(1, 32) && tlf1.castToInt((const uint32_t *)nullptr) != uint32_t(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "uit: " << tlf1.castToInt((const uint32_t *)nullptr) << endl;
      cout << "uic: " << uint32_t(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= -ldexp(1, 31) && f1 < ldexp(1, 31) && tlf1.castToInt((const int32_t *)nullptr) != int32_t(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "sit: " << tlf1.castToInt((const int32_t *)nullptr) << endl;
      cout << "sic: " << int32_t(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= 0 && f1 < ldexp(1, 64) && tlf1.castToInt((const uint64_t *)nullptr) != uint64_t(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "ult: " << tlf1.castToInt((const uint64_t *)nullptr) << endl;
      cout << "ulc: " << uint64_t(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= -ldexp(1, 63) && f1 < ldexp(1, 63) && tlf1.castToInt((const int64_t *)nullptr) != int64_t(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "slt: " << tlf1.castToInt((const int64_t *)nullptr) << endl;
      cout << "slc: " << int64_t(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
    if (f1 >= 0 && f1 < ldexp(1, 128) && tlf1.castToInt((const BigUInt<7> *)nullptr) != UINT128(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "u128t: " << tlf1.castToInt((const BigUInt<7> *)nullptr) << endl;
      cout << "u128c: " << UINT128(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (f1 >= -ldexp(1, 127) && f1 < ldexp(1, 127) && tlf1.castToInt((const BigInt<7> *)nullptr) != INT128(float(tlf1))) {
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "s128t: " << tlf1.castToInt((const BigInt<7> *)nullptr) << endl;
      cout << "s128c: " << INT128(float(tlf1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

    //

    double d4t = double(udouble::addsub(tld1, tld2, false));
    double d4c = d1 + d2;

    if (!cmpd(d4t, d4c)) {
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2: " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "+t: " << d4t << " : " << to_string_d(udouble(d4t)) << endl;
      cout << "+c: " << d4c << " : " << to_string_d(udouble(d4c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    double d5t = double(udouble::addsub(tld1, tld2, true));
    double d5c = d1 - d2;

    if (!cmpd(d5t, d5c)) {
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2: " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "-t: " << d5t << " : " << to_string_d(udouble(d5t)) << endl;
      cout << "-c: " << d5c << " : " << to_string_d(udouble(d5c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    double d6t = double(udouble::mul(tld1, tld2));
    double d6c = d1 * d2;

    if (!cmpd(d6t, d6c)) {
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2: " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "*t: " << d6t << " : " << to_string_d(udouble(d6t)) << endl;
      cout << "*c: " << d6c << " : " << to_string_d(udouble(d6c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    double d7t = double(udouble::div(tld1, tld2));
    double d7c = d1 / d2;

    if (!cmpd(d7t, d7c)) {
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2: " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "/t: " << d7t << " : " << to_string_d(udouble(d7t)) << endl;
      cout << "/c: " << d7c << " : " << to_string_d(udouble(d7c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

#if defined(__GLIBC__) || defined(FP_FAST_FMA)
    double d8t = double(udouble::fma(tld1, tld2, tld3));
    double d8c = fma(d1, d2, d3);

    if (!cmpd(d8t, d8c)) {
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2: " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "d3: " << d3  << " : " << to_string_d(udouble(d3 )) << endl;
      cout << "ft: " << d8t << " : " << to_string_d(udouble(d8t)) << endl;
      cout << "fc: " << d8c << " : " << to_string_d(udouble(d8c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

#ifdef ENABLE_QUAD
    double d9t = double(tlq1.cast((const udouble *)nullptr));
    double d9c = q1;

    if (!cmpf(d9t, d9c)) {
      cout << "cast double to quad" << endl;
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "ct: " << d9t << " : " << to_string_d(udouble(d9t)) << endl;
      cout << "cc: " << d9c << " : " << to_string_d(udouble(d9c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

    if (d1 >= 0 && d1 < ldexp(1, 16) && tld1.castToInt((const uint16_t *)nullptr) != uint16_t(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "ust: " << tld1.castToInt((const uint16_t *)nullptr) << endl;
      cout << "usc: " << uint16_t(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (d1 >= -ldexp(1, 15) && d1 < ldexp(1, 15) && tld1.castToInt((const int16_t *)nullptr) != int16_t(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "sst: " << tld1.castToInt((const int16_t *)nullptr) << endl;
      cout << "ssc: " << int16_t(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (d1 >= 0 && d1 < ldexp(1, 32) && tld1.castToInt((const uint32_t *)nullptr) != uint32_t(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "uit: " << tld1.castToInt((const uint32_t *)nullptr) << endl;
      cout << "uic: " << uint32_t(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (d1 >= -ldexp(1, 31) && d1 < ldexp(1, 31) && tld1.castToInt((const int32_t *)nullptr) != int32_t(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "sit: " << tld1.castToInt((const int32_t *)nullptr) << endl;
      cout << "sic: " << int32_t(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (d1 >= 0 && d1 < ldexp(1, 64) && tld1.castToInt((const uint64_t *)nullptr) != uint64_t(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "ult: " << tld1.castToInt((const uint64_t *)nullptr) << endl;
      cout << "ulc: " << uint64_t(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (d1 >= -ldexp(1, 63) && d1 < ldexp(1, 63) && tld1.castToInt((const int64_t *)nullptr) != int64_t(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "slt: " << tld1.castToInt((const int64_t *)nullptr) << endl;
      cout << "slc: " << int64_t(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
    if (d1 >= 0 && d1 < ldexp(1, 128) && tld1.castToInt((const BigUInt<7> *)nullptr) != UINT128(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "u128t: " << tld1.castToInt((const BigUInt<7> *)nullptr) << endl;
      cout << "u128c: " << UINT128(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (d1 >= -ldexp(1, 127) && d1 < ldexp(1, 127) && tld1.castToInt((const BigInt<7> *)nullptr) != INT128(double(tld1))) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "s128t: " << tld1.castToInt((const BigInt<7> *)nullptr) << endl;
      cout << "s128c: " << INT128(double(tld1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

    if ((tld1 == tld2) != (d1 == d2)) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "==t: " << (tld1 == tld2) << endl;
      cout << "==c: " << (d1 == d2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tld1 != tld2) != (d1 != d2)) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "!=t: " << (tld1 != tld2) << endl;
      cout << "!=c: " << (d1 != d2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tld1 < tld2) != (d1 < d2)) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "< t: " << (tld1 < tld2) << endl;
      cout << "< c: " << (d1 < d2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tld1 > tld2) != (d1 > d2)) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "> t: " << (tld1 > tld2) << endl;
      cout << "> c: " << (d1 > d2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tld1 <= tld2) != (d1 <= d2)) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << "<=t: " << (tld1 <= tld2) << endl;
      cout << "<=c: " << (d1 <= d2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tld1 >= tld2) != (d1 >= d2)) {
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1 )) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2 )) << endl;
      cout << ">=t: " << (tld1 >= tld2) << endl;
      cout << ">=c: " << (d1 >= d2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((i & 31) == 0 && fabs(d1) > 1e-250 && fabs(d1) < 1e+250) {
      double dbt = (double)Double(to_string(Double(tld1), 18).c_str());
      double dbc = (double)tld1;

      if (!cmpd(dbt, dbc, 7)) {
	cout << "d1: " << d1  << " : " << to_string_d(udouble(d1)) << endl;
	cout << "s : " << to_string(Double(tld1), 18) << endl;
	cout << "ct: " << dbt << " : " << to_string_d(udouble(dbt)) << endl;
	cout << "cc: " << dbc << " : " << to_string_d(udouble(dbc)) << endl;
	cout << "NG" << endl;
	exit(-1);
      }
    }

    double dc, dt, dn = d1;

    dc = trunc(dn);
    dt = (double)trunc(Double(dn).getUnpacked());

    if (!cmpd(dc, dt)) {
      printf("\ntrunc\ndn = %.18lg, dc = %.18lg, dt = %.18lg\n", dn, dc, dt);
      exit(-1);
    }

    dc = floor(dn);
    dt = (double)floor(Double(dn).getUnpacked());

    if (!cmpd(dc, dt)) {
      printf("\nfloor\ndn = %.18lg, dc = %.18lg, dt = %.18lg\n", dn, dc, dt);
      exit(-1);
    }

    dc = ceil(dn);
    dt = (double)ceil(Double(dn).getUnpacked());

    if (!cmpd(dc, dt)) {
      printf("\nceil\ndn = %.18lg, dc = %.18lg, dt = %.18lg\n", dn, dc, dt);
      exit(-1);
    }

    dc = round(dn);
    dt = (double)round(Double(dn).getUnpacked());

    if (!cmpd(dc, dt)) {
      printf("\nround\ndn = %.18lg, dc = %.18lg, dt = %.18lg\n", dn, dc, dt);
      exit(-1);
    }

    dc = rint(dn);
    dt = (double)rint(Double(dn).getUnpacked());

    if (!cmpd(dc, dt)) {
      printf("\nrint\ndn = %.18lg, dc = %.18lg, dt = %.18lg\n", dn, dc, dt);
      exit(-1);
    }

    //

    if (isnan(d1) != isnan(Double(d1))) {
      printf("\nisnan d1 = %.10g\n", d1);
      exit(-1);
    }

    if (isinf(d1) != isinf(Double(d1))) {
      printf("\nisinf d1 = %.10g\n", d1);
      exit(-1);
    }

    if (finite(d1) != finite(Double(d1))) {
      printf("\nfinite d1 = %.10g\n", d1);
      exit(-1);
    }

    if (fpclassify(d1) != fpclassify(Double(d1))) {
      printf("\nfpclassify d1 = %.10g, c = %d, t = %d\n", d1, fpclassify(d1), fpclassify(Double(d1)));
      cout << to_string_d(Double(d1).getUnpacked()) << endl;
      exit(-1);
    }

    //

#ifdef ENABLE_QUAD
    quad q1t = quad(tlf1.cast((const uquad *)nullptr));
    quad q1c = f1;

    if (!cmpq(q1t, q1c)) {
      cout << "cast from float to quad" << endl;
      cout << "f1: " << q1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "ct: " << q1t << " : " << to_string_d(uquad(q1t)) << endl;
      cout << "cc: " << q1c << " : " << to_string_d(uquad(q1c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    q1t = quad(tld1.cast((const uquad *)nullptr));
    q1c = d1;

    if (!cmpq(q1t, q1c)) {
      cout << "cast from double to quad" << endl;
      cout << "d1: " << q1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "ct: " << q1t << " : " << to_string_d(uquad(q1t)) << endl;
      cout << "cc: " << q1c << " : " << to_string_d(uquad(q1c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= 0 && q1 < ldexp(1, 16) && tlq1.castToInt((const uint16_t *)nullptr) != uint16_t(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "ust: " << q1t << " : " << tlq1.castToInt((const uint16_t *)nullptr) << endl;
      cout << "usc: " << q1c << " : " << uint16_t(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= -ldexp(1, 15) && q1 < ldexp(1, 15) && tlq1.castToInt((const int16_t *)nullptr) != int16_t(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "sst: " << q1t << " : " << tlq1.castToInt((const int16_t *)nullptr) << endl;
      cout << "ssc: " << q1c << " : " << int16_t(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= 0 && q1 < ldexp(1, 32) && tlq1.castToInt((const uint32_t *)nullptr) != uint32_t(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "uit: " << tlq1.castToInt((const uint32_t *)nullptr) << endl;
      cout << "uic: " << uint32_t(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= -ldexp(1, 31) && q1 < ldexp(1, 31) && tlq1.castToInt((const int32_t *)nullptr) != int32_t(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "sit: " << tlq1.castToInt((const int32_t *)nullptr) << endl;
      cout << "sic: " << int32_t(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= 0 && q1 < ldexp(1, 64) && tlq1.castToInt((const uint64_t *)nullptr) != uint64_t(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "ult: " << tlq1.castToInt((const uint64_t *)nullptr) << endl;
      cout << "ulc: " << uint64_t(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= -ldexp(1, 63) && q1 < ldexp(1, 63) && tlq1.castToInt((const int64_t *)nullptr) != int64_t(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "slt: " << tlq1.castToInt((const int64_t *)nullptr) << endl;
      cout << "slc: " << int64_t(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
    if (q1 >= 0 && q1 < ldexp(1, 128) && tlq1.castToInt((const BigUInt<7> *)nullptr) != UINT128(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "u128t: " << tlq1.castToInt((const BigUInt<7> *)nullptr) << endl;
      cout << "u128c: " << UINT128(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (q1 >= -ldexp(1, 127) && q1 < ldexp(1, 127) && tlq1.castToInt((const BigInt<7> *)nullptr) != INT128(quad(tlq1))) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "s128t: " << tlq1.castToInt((const BigInt<7> *)nullptr) << endl;
      cout << "s128c: " << INT128(quad(tlq1)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }
#endif

    //

    quad q4t = quad(uquad::addsub(tlq1, tlq2, false));
    quad q4c = q1 + q2;

    if (!cmpq(d4t, d4c)) {
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2: " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "+t: " << d4t << " : " << to_string_d(uquad(q4t)) << endl;
      cout << "+c: " << d4c << " : " << to_string_d(uquad(q4c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    quad q5t = quad(uquad::addsub(tlq1, tlq2, true));
    quad q5c = q1 - q2;

    if (!cmpq(d5t, d5c)) {
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2: " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "-t: " << d5t << " : " << to_string_d(uquad(q5t)) << endl;
      cout << "-c: " << d5c << " : " << to_string_d(uquad(q5c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    quad q6t = quad(uquad::mul(tlq1, tlq2));
    quad q6c = q1 * q2;

    if (!cmpq(d6t, d6c)) {
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2: " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "*t: " << d6t << " : " << to_string_d(uquad(q6t)) << endl;
      cout << "*c: " << d6c << " : " << to_string_d(uquad(q6c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    quad q7t = quad(uquad::div(tlq1, tlq2));
    quad q7c = q1 / q2;

    if (!cmpq(d7t, d7c)) {
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2: " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "/t: " << d7t << " : " << to_string_d(uquad(q7t)) << endl;
      cout << "/c: " << d7c << " : " << to_string_d(uquad(q7c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    quad q8t = quad(uquad::fma(tlq1, tlq2, tlq3));
    quad q8c = fmaq(q1, q2, q3);

    if (!cmpq(d8t, d8c)) {
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2: " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "q3: " << q3  << " : " << to_string_d(uquad(q3 )) << endl;
      cout << "ft: " << d8t << " : " << to_string_d(uquad(q8t)) << endl;
      cout << "fc: " << d8c << " : " << to_string_d(uquad(q8c)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlq1 == tlq2) != (q1 == q2)) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2 : " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "==t: " << (tlq1 == tlq2) << endl;
      cout << "==c: " << (q1 == q2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlq1 != tlq2) != (q1 != q2)) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2 : " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "!=t: " << (tlq1 != tlq2) << endl;
      cout << "!=c: " << (q1 != q2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlq1 < tlq2) != (q1 < q2)) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2 : " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "< t: " << (tlq1 < tlq2) << endl;
      cout << "< c: " << (q1 < q2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlq1 > tlq2) != (q1 > q2)) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2 : " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "> t: " << (tlq1 > tlq2) << endl;
      cout << "> c: " << (q1 > q2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlq1 <= tlq2) != (q1 <= q2)) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2 : " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << "<=t: " << (tlq1 <= tlq2) << endl;
      cout << "<=c: " << (q1 <= q2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if ((tlq1 >= tlq2) != (q1 >= q2)) {
      cout << "q1 : " << q1  << " : " << to_string_d(uquad(q1 )) << endl;
      cout << "q2 : " << q2  << " : " << to_string_d(uquad(q2 )) << endl;
      cout << ">=t: " << (tlq1 >= tlq2) << endl;
      cout << ">=c: " << (q1 >= q2) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    static const quad qp1k = (quad)(0x1p+512) * (quad)(0x1p+512), qn1k = (quad)(0x1p-1024);

    if ((i & 3) == 0) {
      if (ilogbq(q1) != ilogb(Quad(q1))) {
	cout << "ilogb" << endl;
	cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
	cout << "c : " << ilogbq(q1) << endl;
	cout << "t : " << ilogb(Quad(q1)) << endl;
	cout << "NG" << endl;
	exit(-1);
      }

      if (qn1k * 1e-300 < q1 && q1 < qp1k * 1e+300 && !cmpq((quad)sqrt(Quad(q1)), sqrtq(q1), 1)) {
	cout << "sqrtq" << endl;
	cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
	cout << "c : " << sqrtq(q1)  << " : " << to_string_d(uquad(sqrtq(q1))) << endl;
	cout << "t : " << sqrt(Quad(q1)) << " : " << to_string_d(uquad((quad)sqrt(Quad(q1)))) << endl;
	cout << "NG" << endl;
	exit(-1);
      }

      Octuple o1 = Quad(q1);
      Octuple o2 = sqrt(o1);
      if (!cmpq((quad)(Quad)sqrt(o1), (quad)(Quad)o2)) {
	cout << "sqrtq vs sqrto" << endl;
	cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
	cout << "c : " << (quad)(Quad)o2 << " : " << to_string_d(Quad(o2).getUnpacked()) << endl;
	cout << "t : " << sqrt(Quad(tlq1)) << " : " << to_string_d(sqrt(Quad(tlq1)).getUnpacked()) << endl;
	cout << "NG" << endl;
	exit(-1);
      }
    }

    if ((i & 31) == 0 && fabsq(q1) > qn1k * qn1k * qn1k * qn1k && fabsq(q1) < qp1k * qp1k * qp1k * qp1k) {
      quad qbt = (quad)Quad(to_string(Quad(tlq1), 36).c_str());
      quad qbc = (quad)tlq1;

      if (!cmpq(qbt, qbc, 7)) {
	cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
	cout << "s : " << to_string(Quad(tlq1), 36) << endl;
	cout << "ct: " << qbt << " : " << to_string_d(uquad(qbt)) << endl;
	cout << "cc: " << qbc << " : " << to_string_d(uquad(qbc)) << endl;
	cout << "NG" << endl;
	exit(-1);
      }
    }

    quad qc, qt, qn = q1;

    qc = truncq(qn);
    qt = (quad)trunc(Quad(qn).getUnpacked());

    if (!cmpq(qc, qt)) {
      printf("\ntruncq\n");
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "s : " << to_string(Quad(tlq1), 36) << endl;
      cout << "ct: " << qt << " : " << to_string_d(uquad(qt)) << endl;
      cout << "cc: " << qc << " : " << to_string_d(uquad(qc)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    qc = floorq(qn);
    qt = (quad)floor(Quad(qn).getUnpacked());

    if (!cmpq(qc, qt)) {
      printf("\nfloorq\n");
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "s : " << to_string(Quad(tlq1), 36) << endl;
      cout << "ct: " << qt << " : " << to_string_d(uquad(qt)) << endl;
      cout << "cc: " << qc << " : " << to_string_d(uquad(qc)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    qc = ceilq(qn);
    qt = (quad)ceil(Quad(qn).getUnpacked());

    if (!cmpq(qc, qt)) {
      printf("\nceilq\n");
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "s : " << to_string(Quad(tlq1), 36) << endl;
      cout << "ct: " << qt << " : " << to_string_d(uquad(qt)) << endl;
      cout << "cc: " << qc << " : " << to_string_d(uquad(qc)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    qc = roundq(qn);
    qt = (quad)round(Quad(qn).getUnpacked());

    if (!cmpq(qc, qt)) {
      printf("\nroundq\n");
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "s : " << to_string(Quad(tlq1), 36) << endl;
      cout << "ct: " << qt << " : " << to_string_d(uquad(qt)) << endl;
      cout << "cc: " << qc << " : " << to_string_d(uquad(qc)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    qc = rintq(qn);
    qt = (quad)rint(Quad(qn).getUnpacked());

    if (!cmpq(qc, qt)) {
      printf("\nrintq\n");
      cout << "q1: " << q1  << " : " << to_string_d(uquad(q1)) << endl;
      cout << "s : " << to_string(Quad(tlq1), 36) << endl;
      cout << "ct: " << qt << " : " << to_string_d(uquad(qt)) << endl;
      cout << "cc: " << qc << " : " << to_string_d(uquad(qc)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    //

    if (isnanq(q1) != isnan(Quad(q1))) {
      printf("\nisnan q1 = %.10g\n", (double)q1);
      exit(-1);
    }

    if (isinfq(q1) != isinf(Quad(q1))) {
      printf("\nisinf q1 = %.10g\n", (double)q1);
      exit(-1);
    }

    if (finiteq(q1) != finite(Quad(q1))) {
      printf("\nfinite q1 = %.10g\n", (double)q1);
      exit(-1);
    }

    if (fpclassifyq(q1) != fpclassify(Quad(q1))) {
      printf("\nfpclassify q1 = %.10g\n", (double)q1);
      exit(-1);
    }
#endif

    //

    xfloat xf1 = tlf1.cast((const xfloat*)0);
    xfloat xf2 = tlf2.cast((const xfloat*)0);
    xfloat xf3 = tlf3.cast((const xfloat*)0);

    if (!cmpf((float)xf1.cast((const ufloat*)0), f1)) {
      cout << "cast between xfloat and ufloat" << endl;
      cout << "f1: " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "t1: " << to_string_d(xf1) << endl;
      cout << "t2: " << (float)xf1.cast((const ufloat*)0) << " : " << to_string_d(xf1.cast((const ufloat*)0)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xfloat xf4 = xfloat::addsub(xf1, xf2, false);
    if (!cmpf((float)xf4.cast((const ufloat*)0), f1 + f2, 1)) {
      cout << "xfloat add" << endl;
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2)) << endl;
      cout << "xf1: " << to_string_d(xf1) << endl;
      cout << "xf2: " << to_string_d(xf2) << endl;
      cout << "t  : " << (float)xf4.cast((const ufloat*)0) << " : " << to_string_d(xf4.cast((const ufloat*)0)) << endl;
      cout << "c  : " << (f1 + f2) << " : " << to_string_d(ufloat((f1 + f2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xfloat xf5 = xfloat::addsub(xf1, xf2, true);
    if (!cmpf((float)xf5.cast((const ufloat*)0), f1 - f2, 1)) {
      cout << "xfloat sub" << endl;
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2)) << endl;
      cout << "xf1: " << to_string_d(xf1) << endl;
      cout << "xf2: " << to_string_d(xf2) << endl;
      cout << "t  : " << (float)xf5.cast((const ufloat*)0) << " : " << to_string_d(xf5.cast((const ufloat*)0)) << endl;
      cout << "c  : " << (f1 - f2) << " : " << to_string_d(ufloat((f1 - f2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xfloat xf6 = xfloat::mul(xf1, xf2);
    if (!cmpf((float)xf6.cast((const ufloat*)0), f1 * f2, 1)) {
      cout << "xfloat mul" << endl;
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2)) << endl;
      cout << "xf1: " << to_string_d(xf1) << endl;
      cout << "xf2: " << to_string_d(xf2) << endl;
      cout << "t  : " << (float)xf6.cast((const ufloat*)0) << " : " << to_string_d(xf6.cast((const ufloat*)0)) << endl;
      cout << "c  : " << (f1 * f2) << " : " << to_string_d(ufloat((f1 * f2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xfloat xf7 = xfloat::div(xf1, xf2);
    if (!cmpf((float)xf7.cast((const ufloat*)0), f1 / f2, 1)) {
      cout << "xfloat div" << endl;
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2)) << endl;
      cout << "xf1: " << to_string_d(xf1) << endl;
      cout << "xf2: " << to_string_d(xf2) << endl;
      cout << "t  : " << (float)xf7.cast((const ufloat*)0) << " : " << to_string_d(xf7.cast((const ufloat*)0)) << endl;
      cout << "c  : " << (f1 / f2) << " : " << to_string_d(ufloat((f1 / f2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xfloat xf8 = xfloat::fma(xf1, xf2, xf3);
    if (!cmpf((float)xf8.cast((const ufloat*)0), fmaf(f1, f2, f3), 1)) {
      cout << "xfloat fma" << endl;
      cout << "f1 : " << f1  << " : " << to_string_d(ufloat(f1)) << endl;
      cout << "f2 : " << f2  << " : " << to_string_d(ufloat(f2)) << endl;
      cout << "f3 : " << f3  << " : " << to_string_d(ufloat(f3)) << endl;
      cout << "xf1: " << to_string_d(xf1) << endl;
      cout << "xf2: " << to_string_d(xf2) << endl;
      cout << "xf3: " << to_string_d(xf3) << endl;
      cout << "t  : " << (float)xf8.cast((const ufloat*)0) << " : " << to_string_d(xf8.cast((const ufloat*)0)) << endl;
      cout << "c  : " << fmaf(f1, f2, f3) << " : " << to_string_d(ufloat(fma(f1, f2, f3))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xdouble xd1 = tld1.cast((const xdouble*)0);
    xdouble xd2 = tld2.cast((const xdouble*)0);
    xdouble xd3 = tld3.cast((const xdouble*)0);

    if (!cmpd((double)xd1.cast((const udouble*)0), d1)) {
      cout << "cast between xdouble and udouble" << endl;
      cout << "d1: " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "t1: " << to_string_d(xd1) << endl;
      cout << "t2: " << (double)xd1.cast((const udouble*)0) << " : " << to_string_d(xd1.cast((const udouble*)0)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xdouble xd4 = xdouble::addsub(xd1, xd2, false);
    if (!cmpd((double)xd4.cast((const udouble*)0), d1 + d2, 1)) {
      cout << "xdouble add" << endl;
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2)) << endl;
      cout << "xd1: " << to_string_d(xd1) << endl;
      cout << "xd2: " << to_string_d(xd2) << endl;
      cout << "t  : " << (double)xd4.cast((const udouble*)0) << " : " << to_string_d(xd4.cast((const udouble*)0)) << endl;
      cout << "c  : " << (d1 + d2) << " : " << to_string_d(udouble((d1 + d2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xdouble xd5 = xdouble::addsub(xd1, xd2, true);
    if (!cmpd((double)xd5.cast((const udouble*)0), d1 - d2, 1)) {
      cout << "xdouble sub" << endl;
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2)) << endl;
      cout << "xd1: " << to_string_d(xd1) << endl;
      cout << "xd2: " << to_string_d(xd2) << endl;
      cout << "t  : " << (double)xd5.cast((const udouble*)0) << " : " << to_string_d(xd5.cast((const udouble*)0)) << endl;
      cout << "c  : " << (d1 - d2) << " : " << to_string_d(udouble((d1 - d2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xdouble xd6 = xdouble::mul(xd1, xd2);
    if (!cmpd((double)xd6.cast((const udouble*)0), d1 * d2, 1)) {
      cout << "xdouble mul" << endl;
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2)) << endl;
      cout << "xd1: " << to_string_d(xd1) << endl;
      cout << "xd2: " << to_string_d(xd2) << endl;
      cout << "t  : " << (double)xd6.cast((const udouble*)0) << " : " << to_string_d(xd6.cast((const udouble*)0)) << endl;
      cout << "c  : " << (d1 * d2) << " : " << to_string_d(udouble((d1 * d2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xdouble xd7 = xdouble::div(xd1, xd2);
    if (!cmpd((double)xd7.cast((const udouble*)0), d1 / d2, 1)) {
      cout << "xdouble div" << endl;
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2)) << endl;
      cout << "xd1: " << to_string_d(xd1) << endl;
      cout << "xd2: " << to_string_d(xd2) << endl;
      cout << "t  : " << (double)xd7.cast((const udouble*)0) << " : " << to_string_d(xd7.cast((const udouble*)0)) << endl;
      cout << "c  : " << (d1 / d2) << " : " << to_string_d(udouble((d1 / d2))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    xdouble xd8 = xdouble::fma(xd1, xd2, xd3);
    if (!cmpd((double)xd8.cast((const udouble*)0), fma(d1, d2, d3), 1)) {
      cout << "xdouble fma" << endl;
      cout << "d1 : " << d1  << " : " << to_string_d(udouble(d1)) << endl;
      cout << "d2 : " << d2  << " : " << to_string_d(udouble(d2)) << endl;
      cout << "d3 : " << d3  << " : " << to_string_d(udouble(d3)) << endl;
      cout << "xd1: " << to_string_d(xd1) << endl;
      cout << "xd2: " << to_string_d(xd2) << endl;
      cout << "xd3: " << to_string_d(xd3) << endl;
      cout << "t  : " << (double)xd8.cast((const udouble*)0) << " : " << to_string_d(xd8.cast((const udouble*)0)) << endl;
      cout << "c  : " << fma(d1, d2, d3) << " : " << to_string_d(udouble(fma(d1, d2, d3))) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    //

    {
      int16_t i16;
      rng->nextBytes((unsigned char *)&i16, sizeof(i16));

      ufloat uf = ufloat::castFromInt(i16);
      if (!cmpf(float(uf), float(i16))) {
	cout << "float i16 " << i16 << endl;
      }

      udouble ud = udouble::castFromInt(i16);
      if (!cmpd(double(ud), double(i16))) {
	cout << "double i16 " << i16 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(i16);
      if (!cmpd(quad(uq), quad(i16))) {
	cout << "quad i16 " << i16 << endl;
      }
#endif
    }

    {
      uint16_t u16;
      rng->nextBytes((unsigned char *)&u16, sizeof(u16));

      ufloat uf = ufloat::castFromInt(u16);
      if (!cmpf(float(uf), float(u16))) {
	cout << "float u16 " << u16 << endl;
      }

      udouble ud = udouble::castFromInt(u16);
      if (!cmpd(double(ud), double(u16))) {
	cout << "double u16 " << u16 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(u16);
      if (!cmpd(quad(uq), quad(u16))) {
	cout << "quad u16 " << u16 << endl;
      }
#endif
    }

    {
      int32_t i32;
      rng->nextBytes((unsigned char *)&i32, sizeof(i32));

      ufloat uf = ufloat::castFromInt(i32);
      if (!cmpf(float(uf), float(i32))) {
	cout << "float i32 " << i32 << endl;
      }

      udouble ud = udouble::castFromInt(i32);
      if (!cmpd(double(ud), double(i32))) {
	cout << "double i32 " << i32 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(i32);
      if (!cmpd(quad(uq), quad(i32))) {
	cout << "quad i32 " << i32 << endl;
      }
#endif
    }

    {
      uint32_t u32;
      rng->nextBytes((unsigned char *)&u32, sizeof(u32));

      ufloat uf = ufloat::castFromInt(u32);
      if (!cmpf(float(uf), float(u32))) {
	cout << "float u32 " << u32 << endl;
      }

      udouble ud = udouble::castFromInt(u32);
      if (!cmpd(double(ud), double(u32))) {
	cout << "double u32 " << u32 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(u32);
      if (!cmpd(quad(uq), quad(u32))) {
	cout << "quad u32 " << u32 << endl;
      }
#endif
    }

    {
      int64_t i64;
      rng->nextBytes((unsigned char *)&i64, sizeof(i64));

      ufloat uf = ufloat::castFromInt(i64);
      if (!cmpf(float(uf), float(i64))) {
	cout << "float i64 " << i64 << endl;
      }

      udouble ud = udouble::castFromInt(i64);
      if (!cmpd(double(ud), double(i64))) {
	cout << "double i64 " << i64 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(i64);
      if (!cmpd(quad(uq), quad(i64))) {
	cout << "quad i64 " << i64 << endl;
      }
#endif
    }

    {
      uint64_t u64;
      rng->nextBytes((unsigned char *)&u64, sizeof(u64));

      ufloat uf = ufloat::castFromInt(u64);
      if (!cmpf(float(uf), float(u64))) {
	cout << "float u64 " << u64 << endl;
      }

      udouble ud = udouble::castFromInt(u64);
      if (!cmpd(double(ud), double(u64))) {
	cout << "double u64 " << u64 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(u64);
      if (!cmpd(quad(uq), quad(u64))) {
	cout << "quad u64 " << u64 << endl;
      }
#endif
    }

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
    {
      INT128 i128;
      rng->nextBytes((unsigned char *)&i128, sizeof(i128));

      ufloat uf = ufloat::castFromInt(i128);
      if (!cmpf(float(uf), float(i128))) {
	cout << "float i128 " << i128 << endl;
      }

      udouble ud = udouble::castFromInt(i128);
      if (!cmpd(double(ud), double(i128))) {
	cout << "double i128 " << i128 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(i128);
      if (!cmpd(quad(uq), quad(i128))) {
	cout << "quad i128 " << i128 << endl;
      }
#endif
    }

    {
      UINT128 u128;
      rng->nextBytes((unsigned char *)&u128, sizeof(u128));

      ufloat uf = ufloat::castFromInt(u128);
      if (!cmpf(float(uf), float(u128))) {
	cout << "float u128 " << u128 << endl;
      }

      udouble ud = udouble::castFromInt(u128);
      if (!cmpd(double(ud), double(u128))) {
	cout << "double u128 " << u128 << endl;
      }

#ifdef ENABLE_QUAD
      uquad uq = uquad::castFromInt(u128);
      if (!cmpd(quad(uq), quad(u128))) {
	cout << "quad u128 " << u128 << endl;
      }
#endif
    }
#endif // #ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
  }

  for(uint64_t i=0;niter == 0 || i<niter;i++) {
    float f1 = rndf(rng), f2 = rndf(rng), f3 = rndf(rng);
    double d1 = f1, d2 = f2, d3 = f3, d0;
    Quad q1 = f1, q2 = f2, q3 = f3;
    Octuple o1 = f1, o2 = f2, o3 = f3, o0;
    int i1 = (int)rng->nextLT(65536) - 32768, i2 = (int)rng->nextLT(65536) - 32768;

    //

    check("(double)Double(Octuple(d1))", (double)Double(Octuple(d1)), d1);
    check("(double)(Double)Octuple(i1)", (double)(Double)Octuple(i1), (double)i1);

    check("o1 + o2", o1 + o2, d1 + d2);
    check("d1 + o2", d1 + o2, d1 + d2);
    check("f1 + o2", f1 + o2, d1 + d2);
    check("o1 + f2", o1 + f2, d1 + d2);
    check("i1 + o2", i1 + o2, i1 + d2);
    check("o1 + i2", o1 + i2, d1 + i2);
    o0 = o1; o0 += o2; check("o0 = o1; o0 += o2;", o0, d1 + d2);
    o0 = o1; o0 += q2; check("o0 = o1; o0 += q2;", o0, d1 + d2);
    o0 = o1; o0 += d2; check("o0 = o1; o0 += d2;", o0, d1 + d2);
    o0 = o1; o0 += i2; check("o0 = o1; o0 += i2;", o0, d1 + i2);

    o0 = o1; d0 = d1; check("o0 = o1; o0 ++;", o0++, d0++); check("o0 = o1; o0 ++; (2)", o0, d0);
    o0 = o1; d0 = d1; check("o0 = o1; ++ o0;", ++o0, ++d0); check("o0 = o1; ++ o0; (2)", o0, d0);

    check("o1 - o2", o1 - o2, d1 - d2);
    check("d1 - o2", d1 - o2, d1 - d2);
    check("f1 - o2", f1 - o2, d1 - d2);
    check("o1 - f2", o1 - f2, d1 - d2);
    check("i1 - o2", i1 - o2, i1 - d2);
    check("o1 - i2", o1 - i2, d1 - i2);
    o0 = o1; o0 -= o2; check("o0 = o1; o0 -= o2;", o0, d1 - d2);
    o0 = o1; o0 -= q2; check("o0 = o1; o0 -= q2;", o0, d1 - d2);
    o0 = o1; o0 -= d2; check("o0 = o1; o0 -= d2;", o0, d1 - d2);
    o0 = o1; o0 -= i2; check("o0 = o1; o0 -= i2;", o0, d1 - i2);

    o0 = o1; d0 = d1; check("o0 = o1; o0 --;", o0--, d0--); check("o0 = o1; o0 --; (2)", o0, d0);
    o0 = o1; d0 = d1; check("o0 = o1; -- o0;", --o0, --d0); check("o0 = o1; -- o0; (2)", o0, d0);

    check("o1 * o2", o1 * o2, d1 * d2);
    check("d1 * o2", d1 * o2, d1 * d2);
    check("f1 * o2", f1 * o2, d1 * d2);
    check("o1 * f2", o1 * f2, d1 * d2);
    check("i1 * o2", i1 * o2, i1 * d2);
    check("o1 * i2", o1 * i2, d1 * i2);
    o0 = o1; o0 *= o2; check("o0 = o1; o0 *= o2;", o0, d1 * d2);
    o0 = o1; o0 *= q2; check("o0 = o1; o0 *= q2;", o0, d1 * d2);
    o0 = o1; o0 *= d2; check("o0 = o1; o0 *= d2;", o0, d1 * d2);
    o0 = o1; o0 *= i2; check("o0 = o1; o0 *= i2;", o0, d1 * i2);

    check("o1 / o2", o1 / o2, d1 / d2);
    check("d1 / o2", d1 / o2, d1 / d2);
    check("f1 / o2", f1 / o2, d1 / d2);
    check("o1 / f2", o1 / f2, d1 / d2);
    check("i1 / o2", i1 / o2, i1 / d2);
    check("o1 / i2", o1 / i2, d1 / i2);
    o0 = o1; o0 /= o2; check("o0 = o1; o0 /= o2;", o0, d1 / d2);
    o0 = o1; o0 /= q2; check("o0 = o1; o0 /= q2;", o0, d1 / d2);
    o0 = o1; o0 /= d2; check("o0 = o1; o0 /= d2;", o0, d1 / d2);
    o0 = o1; o0 /= i2; check("o0 = o1; o0 /= i2;", o0, d1 / i2);
  }

  cout << "OK" << endl;
  exit(0);
}
