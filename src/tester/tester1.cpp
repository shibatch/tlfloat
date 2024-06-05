#include <iostream>

#if defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#include <quadmath.h>
#endif

#if (defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)) || defined(__aarch64__)
#define MPFR_WANT_FLOAT128
#endif
#include <mpfr.h>

#include "suppress.hpp"

#include <tlfloat/tlmath.hpp>
#include <testerutil.hpp>

using namespace std;
using namespace tlfloat;

int mpfr_lgamma_(mpfr_t rop, const mpfr_t op, mpfr_rnd_t rnd) {
  int sign;
  return mpfr_lgamma(rop, &sign, op, rnd);
}

template<typename T>
using Func1 = T (*)(const T&);

template<typename T>
using Func2 = T (*)(const T&, const T&);

template<typename T>
using Func3 = T (*)(const T&, const T&, const T&);

using MPFRFunc1 = int (*)(mpfr_t rop, const mpfr_t op, mpfr_rnd_t rnd);
using MPFRFunc1NR = int (*)(mpfr_t rop, const mpfr_t op);
using MPFRFunc2 = int (*)(mpfr_t rop, const mpfr_t op1, const mpfr_t op2, mpfr_rnd_t rnd);
using MPFRFunc3 = int (*)(mpfr_t rop, const mpfr_t op1, const mpfr_t op2, const mpfr_t op3, mpfr_rnd_t rnd);

template<typename T> static constexpr T fabs_(const T &a1) { return fabs(a1); }
template<typename T> static constexpr T copysign_(const T &a1, const T &a2) { return copysign(a1, a2); }
template<typename T> static constexpr T fmax_(const T &a1, const T &a2) { return fmax(a1, a2); }
template<typename T> static constexpr T fmin_(const T &a1, const T &a2) { return fmin(a1, a2); }
template<typename T> static constexpr T fdim_(const T &a1, const T &a2) { return fdim(a1, a2); }
template<typename T> static constexpr T add_(const T &a1, const T &a2) { return a1 + a2; }
template<typename T> static constexpr T sub_(const T &a1, const T &a2) { return a1 - a2; }
template<typename T> static constexpr T mul_(const T &a1, const T &a2) { return a1 * a2; }
template<typename T> static constexpr T div_(const T &a1, const T &a2) { return a1 / a2; }
template<typename T> static constexpr T fma_(const T &a1, const T &a2, const T &a3) { return fma(a1, a2, a3); }
template<typename T> static constexpr T sqrt_(const T &a) { return sqrt(a); }
template<typename T> static constexpr T hypot_(const T &a1, const T &a2) { return hypot(a1, a2); }
template<typename T> static constexpr T trunc_(const T &a) { return trunc(a); }
template<typename T> static constexpr T floor_(const T &a) { return floor(a); }
template<typename T> static constexpr T ceil_(const T &a) { return ceil(a); }
template<typename T> static constexpr T round_(const T &a) { return round(a); }
template<typename T> static constexpr T rint_(const T &a) { return rint(a); }
template<typename T> static constexpr T nextafter_(const T &a1, const T &a2) { return nextafter(a1, a2); }
template<typename T> static constexpr T lgamma_(const T &a1) { return lgamma(a1, nullptr); }

template<typename T, Func1<T> func, MPFRFunc1 mpfrFunc>
void doTest(const char *mes, T a1, mpfr_t &mr, mpfr_t &ma1) {
  T r = func(a1);
  typedef decltype(r.getUnpacked()) Unpacked_t;

  mpfr_set_unpacked(ma1, a1.getUnpacked(), GMP_RNDN);
  mpfrFunc(mr, ma1, GMP_RNDN);
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatdenormmin(), Unpacked_t::floatmax(), true);

  if (ulp <= 1.0) return;

  cout << mes << endl;
  cout << "arg1    : " << to_string(a1, 72) << endl;
  cout << "tlfloat : " << to_string(r, 72) << endl;
  cout << "mpfr    : " << to_string(mr, 72) << endl;
  exit(-1);
}

template<typename T, Func1<T> func, MPFRFunc1NR mpfrFunc>
void doTest(const char *mes, T a1, mpfr_t &mr, mpfr_t &ma1) {
  T r = func(a1);
  typedef decltype(r.getUnpacked()) Unpacked_t;

  mpfr_set_unpacked(ma1, a1.getUnpacked(), GMP_RNDN);
  mpfrFunc(mr, ma1);
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatdenormmin(), Unpacked_t::floatmax(), true);

  if (ulp <= 1.0) return;

  cout << mes << endl;
  cout << "arg1    : " << to_string(a1, 72) << endl;
  cout << "tlfloat : " << to_string(r, 72) << endl;
  cout << "mpfr    : " << to_string(mr, 72) << endl;
  exit(-1);
}

template<typename T, Func2<T> func, MPFRFunc2 mpfrFunc>
void doTest(const char *mes, T a1, T a2, mpfr_t &mr, mpfr_t &ma1, mpfr_t &ma2, bool skipnan = false) {
  if (skipnan && isnan(a2)) return;
  T r = func(a1, a2);
  typedef decltype(r.getUnpacked()) Unpacked_t;

  mpfr_set_unpacked(ma1, a1.getUnpacked(), GMP_RNDN);
  mpfr_set_unpacked(ma2, a2.getUnpacked(), GMP_RNDN);
  mpfrFunc(mr, ma1, ma2, GMP_RNDN);
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatdenormmin(), Unpacked_t::floatmax(), true);

  if (ulp <= 1.0) return;

  cout << mes << endl;
  cout << "arg1    : " << to_string(a1, 72) << endl;
  cout << "arg2    : " << to_string(a2, 72) << endl;
  cout << "tlfloat : " << to_string(r, 72) << endl;
  cout << "mpfr    : " << to_string(mr, 72) << endl;
  exit(-1);
}

template<typename T, Func3<T> func, MPFRFunc3 mpfrFunc>
void doTest(const char *mes, T a1, T a2, T a3, mpfr_t &mr, mpfr_t &ma1, mpfr_t &ma2, mpfr_t &ma3, bool skipnan = false) {
  if (skipnan && isnan(a2)) return;
  T r = func(a1, a2, a3);
  typedef decltype(r.getUnpacked()) Unpacked_t;

  mpfr_set_unpacked(ma1, a1.getUnpacked(), GMP_RNDN);
  mpfr_set_unpacked(ma2, a2.getUnpacked(), GMP_RNDN);
  mpfr_set_unpacked(ma3, a3.getUnpacked(), GMP_RNDN);
  mpfrFunc(mr, ma1, ma2, ma3, GMP_RNDN);
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatdenormmin(), Unpacked_t::floatmax(), true);

  if (ulp <= 1.0) return;

  cout << mes << endl;
  cout << "arg1    : " << to_string(a1, 72) << endl;
  cout << "arg2    : " << to_string(a2, 72) << endl;
  cout << "tlfloat : " << to_string(r, 72) << endl;
  cout << "mpfr    : " << to_string(mr, 72) << endl;
  exit(-1);
}

int main(int argc, char **argv) {
  int n = 100;
  if (argc >= 2) n = atoi(argv[1]);

  mpfr_set_default_prec(256);

  mpfr_t mr, ma1, ma2, ma3;
  mpfr_inits(mr, ma1, ma2, ma3, NULL);

  auto rng = createPreferredRNG();

  vector<Half> hvalues = genTestValues<Half>(n, rng);
  vector<Float> fvalues = genTestValues<Float>(n, rng);
  vector<Double> dvalues = genTestValues<Double>(n, rng);
  vector<Quad> qvalues = genTestValues<Quad>(n, rng);
  vector<Octuple> ovalues = genTestValues<Octuple>(n, rng);

  int N = hvalues.size();

  for(int index0 = 0;index0 < N;index0++) {
    doTest<Half, fabs_, mpfr_abs>("Half fabs", hvalues[index0], mr, ma1);
    doTest<Float, fabs_, mpfr_abs>("Float fabs", fvalues[index0], mr, ma1);
    doTest<Double, fabs_, mpfr_abs>("Double fabs", dvalues[index0], mr, ma1);
    doTest<Quad, fabs_, mpfr_abs>("Quad fabs", qvalues[index0], mr, ma1);
    doTest<Octuple, fabs_, mpfr_abs>("Octuple fabs", ovalues[index0], mr, ma1);

    doTest<Half, sqrt_, mpfr_sqrt>("Half sqrt", hvalues[index0], mr, ma1);
    doTest<Float, sqrt_, mpfr_sqrt>("Float sqrt", fvalues[index0], mr, ma1);
    doTest<Double, sqrt_, mpfr_sqrt>("Double sqrt", dvalues[index0], mr, ma1);
    doTest<Quad, sqrt_, mpfr_sqrt>("Quad sqrt", qvalues[index0], mr, ma1);
    doTest<Octuple, sqrt_, mpfr_sqrt>("Octuple sqrt", ovalues[index0], mr, ma1);

    doTest<Half, trunc_, mpfr_trunc>("Half trunc", hvalues[index0], mr, ma1);
    doTest<Float, trunc_, mpfr_trunc>("Float trunc", fvalues[index0], mr, ma1);
    doTest<Double, trunc_, mpfr_trunc>("Double trunc", dvalues[index0], mr, ma1);
    doTest<Quad, trunc_, mpfr_trunc>("Quad trunc", qvalues[index0], mr, ma1);
    doTest<Octuple, trunc_, mpfr_trunc>("Octuple trunc", ovalues[index0], mr, ma1);

    doTest<Half, floor_, mpfr_floor>("Half floor", hvalues[index0], mr, ma1);
    doTest<Float, floor_, mpfr_floor>("Float floor", fvalues[index0], mr, ma1);
    doTest<Double, floor_, mpfr_floor>("Double floor", dvalues[index0], mr, ma1);
    doTest<Quad, floor_, mpfr_floor>("Quad floor", qvalues[index0], mr, ma1);
    doTest<Octuple, floor_, mpfr_floor>("Octuple floor", ovalues[index0], mr, ma1);

    doTest<Half, ceil_, mpfr_ceil>("Half ceil", hvalues[index0], mr, ma1);
    doTest<Float, ceil_, mpfr_ceil>("Float ceil", fvalues[index0], mr, ma1);
    doTest<Double, ceil_, mpfr_ceil>("Double ceil", dvalues[index0], mr, ma1);
    doTest<Quad, ceil_, mpfr_ceil>("Quad ceil", qvalues[index0], mr, ma1);
    doTest<Octuple, ceil_, mpfr_ceil>("Octuple ceil", ovalues[index0], mr, ma1);

    doTest<Half, round_, mpfr_round>("Half round", hvalues[index0], mr, ma1);
    doTest<Float, round_, mpfr_round>("Float round", fvalues[index0], mr, ma1);
    doTest<Double, round_, mpfr_round>("Double round", dvalues[index0], mr, ma1);
    doTest<Quad, round_, mpfr_round>("Quad round", qvalues[index0], mr, ma1);
    doTest<Octuple, round_, mpfr_round>("Octuple round", ovalues[index0], mr, ma1);

    doTest<Half, rint_, mpfr_rint>("Half rint", hvalues[index0], mr, ma1);
    doTest<Float, rint_, mpfr_rint>("Float rint", fvalues[index0], mr, ma1);
    doTest<Double, rint_, mpfr_rint>("Double rint", dvalues[index0], mr, ma1);
    doTest<Quad, rint_, mpfr_rint>("Quad rint", qvalues[index0], mr, ma1);
    doTest<Octuple, rint_, mpfr_rint>("Octuple rint", ovalues[index0], mr, ma1);

    doTest<Half, sin, mpfr_sin>("Half sin", hvalues[index0], mr, ma1);
    doTest<Float, sin, mpfr_sin>("Float sin", fvalues[index0], mr, ma1);
    doTest<Double, sin, mpfr_sin>("Double sin", dvalues[index0], mr, ma1);
    doTest<Quad, sin, mpfr_sin>("Quad sin", qvalues[index0], mr, ma1);
    doTest<Octuple, sin, mpfr_sin>("Octuple sin", ovalues[index0], mr, ma1);

    doTest<Half, cos, mpfr_cos>("Half cos", hvalues[index0], mr, ma1);
    doTest<Float, cos, mpfr_cos>("Float cos", fvalues[index0], mr, ma1);
    doTest<Double, cos, mpfr_cos>("Double cos", dvalues[index0], mr, ma1);
    doTest<Quad, cos, mpfr_cos>("Quad cos", qvalues[index0], mr, ma1);
    doTest<Octuple, cos, mpfr_cos>("Octuple cos", ovalues[index0], mr, ma1);

    doTest<Half, tan, mpfr_tan>("Half tan", hvalues[index0], mr, ma1);
    doTest<Float, tan, mpfr_tan>("Float tan", fvalues[index0], mr, ma1);
    doTest<Double, tan, mpfr_tan>("Double tan", dvalues[index0], mr, ma1);
    doTest<Quad, tan, mpfr_tan>("Quad tan", qvalues[index0], mr, ma1);
    doTest<Octuple, tan, mpfr_tan>("Octuple tan", ovalues[index0], mr, ma1);

    doTest<Half, asin, mpfr_asin>("Half asin", hvalues[index0], mr, ma1);
    doTest<Float, asin, mpfr_asin>("Float asin", fvalues[index0], mr, ma1);
    doTest<Double, asin, mpfr_asin>("Double asin", dvalues[index0], mr, ma1);
    doTest<Quad, asin, mpfr_asin>("Quad asin", qvalues[index0], mr, ma1);
    doTest<Octuple, asin, mpfr_asin>("Octuple asin", ovalues[index0], mr, ma1);

    doTest<Half, acos, mpfr_acos>("Half acos", hvalues[index0], mr, ma1);
    doTest<Float, acos, mpfr_acos>("Float acos", fvalues[index0], mr, ma1);
    doTest<Double, acos, mpfr_acos>("Double acos", dvalues[index0], mr, ma1);
    doTest<Quad, acos, mpfr_acos>("Quad acos", qvalues[index0], mr, ma1);
    doTest<Octuple, acos, mpfr_acos>("Octuple acos", ovalues[index0], mr, ma1);

    doTest<Half, atan, mpfr_atan>("Half atan", hvalues[index0], mr, ma1);
    doTest<Float, atan, mpfr_atan>("Float atan", fvalues[index0], mr, ma1);
    doTest<Double, atan, mpfr_atan>("Double atan", dvalues[index0], mr, ma1);
    doTest<Quad, atan, mpfr_atan>("Quad atan", qvalues[index0], mr, ma1);
    doTest<Octuple, atan, mpfr_atan>("Octuple atan", ovalues[index0], mr, ma1);

    doTest<Half, log, mpfr_log>("Half log", hvalues[index0], mr, ma1);
    doTest<Float, log, mpfr_log>("Float log", fvalues[index0], mr, ma1);
    doTest<Double, log, mpfr_log>("Double log", dvalues[index0], mr, ma1);
    doTest<Quad, log, mpfr_log>("Quad log", qvalues[index0], mr, ma1);
    doTest<Octuple, log, mpfr_log>("Octuple log", ovalues[index0], mr, ma1);

    doTest<Half, log2, mpfr_log2>("Half log2", hvalues[index0], mr, ma1);
    doTest<Float, log2, mpfr_log2>("Float log2", fvalues[index0], mr, ma1);
    doTest<Double, log2, mpfr_log2>("Double log2", dvalues[index0], mr, ma1);
    doTest<Quad, log2, mpfr_log2>("Quad log2", qvalues[index0], mr, ma1);
    doTest<Octuple, log2, mpfr_log2>("Octuple log2", ovalues[index0], mr, ma1);

    doTest<Half, log10, mpfr_log10>("Half log10", hvalues[index0], mr, ma1);
    doTest<Float, log10, mpfr_log10>("Float log10", fvalues[index0], mr, ma1);
    doTest<Double, log10, mpfr_log10>("Double log10", dvalues[index0], mr, ma1);
    doTest<Quad, log10, mpfr_log10>("Quad log10", qvalues[index0], mr, ma1);
    doTest<Octuple, log10, mpfr_log10>("Octuple log10", ovalues[index0], mr, ma1);

    doTest<Half, log1p, mpfr_log1p>("Half log1p", hvalues[index0], mr, ma1);
    doTest<Float, log1p, mpfr_log1p>("Float log1p", fvalues[index0], mr, ma1);
    doTest<Double, log1p, mpfr_log1p>("Double log1p", dvalues[index0], mr, ma1);
    doTest<Quad, log1p, mpfr_log1p>("Quad log1p", qvalues[index0], mr, ma1);
    doTest<Octuple, log1p, mpfr_log1p>("Octuple log1p", ovalues[index0], mr, ma1);

    doTest<Half, exp, mpfr_exp>("Half exp", hvalues[index0], mr, ma1);
    doTest<Float, exp, mpfr_exp>("Float exp", fvalues[index0], mr, ma1);
    doTest<Double, exp, mpfr_exp>("Double exp", dvalues[index0], mr, ma1);
    doTest<Quad, exp, mpfr_exp>("Quad exp", qvalues[index0], mr, ma1);
    doTest<Octuple, exp, mpfr_exp>("Octuple exp", ovalues[index0], mr, ma1);

    doTest<Half, exp2, mpfr_exp2>("Half exp2", hvalues[index0], mr, ma1);
    doTest<Float, exp2, mpfr_exp2>("Float exp2", fvalues[index0], mr, ma1);
    doTest<Double, exp2, mpfr_exp2>("Double exp2", dvalues[index0], mr, ma1);
    doTest<Quad, exp2, mpfr_exp2>("Quad exp2", qvalues[index0], mr, ma1);
    doTest<Octuple, exp2, mpfr_exp2>("Octuple exp2", ovalues[index0], mr, ma1);

    doTest<Half, exp10, mpfr_exp10>("Half exp10", hvalues[index0], mr, ma1);
    doTest<Float, exp10, mpfr_exp10>("Float exp10", fvalues[index0], mr, ma1);
    doTest<Double, exp10, mpfr_exp10>("Double exp10", dvalues[index0], mr, ma1);
    doTest<Quad, exp10, mpfr_exp10>("Quad exp10", qvalues[index0], mr, ma1);
    doTest<Octuple, exp10, mpfr_exp10>("Octuple exp10", ovalues[index0], mr, ma1);

    doTest<Half, expm1, mpfr_expm1>("Half expm1", hvalues[index0], mr, ma1);
    doTest<Float, expm1, mpfr_expm1>("Float expm1", fvalues[index0], mr, ma1);
    doTest<Double, expm1, mpfr_expm1>("Double expm1", dvalues[index0], mr, ma1);
    doTest<Quad, expm1, mpfr_expm1>("Quad expm1", qvalues[index0], mr, ma1);
    doTest<Octuple, expm1, mpfr_expm1>("Octuple expm1", ovalues[index0], mr, ma1);

    doTest<Half, sinh, mpfr_sinh>("Half sinh", hvalues[index0], mr, ma1);
    doTest<Float, sinh, mpfr_sinh>("Float sinh", fvalues[index0], mr, ma1);
    doTest<Double, sinh, mpfr_sinh>("Double sinh", dvalues[index0], mr, ma1);
    doTest<Quad, sinh, mpfr_sinh>("Quad sinh", qvalues[index0], mr, ma1);
    doTest<Octuple, sinh, mpfr_sinh>("Octuple sinh", ovalues[index0], mr, ma1);

    doTest<Half, cosh, mpfr_cosh>("Half cosh", hvalues[index0], mr, ma1);
    doTest<Float, cosh, mpfr_cosh>("Float cosh", fvalues[index0], mr, ma1);
    doTest<Double, cosh, mpfr_cosh>("Double cosh", dvalues[index0], mr, ma1);
    doTest<Quad, cosh, mpfr_cosh>("Quad cosh", qvalues[index0], mr, ma1);
    doTest<Octuple, cosh, mpfr_cosh>("Octuple cosh", ovalues[index0], mr, ma1);

    doTest<Half, tanh, mpfr_tanh>("Half tanh", hvalues[index0], mr, ma1);
    doTest<Float, tanh, mpfr_tanh>("Float tanh", fvalues[index0], mr, ma1);
    doTest<Double, tanh, mpfr_tanh>("Double tanh", dvalues[index0], mr, ma1);
    doTest<Quad, tanh, mpfr_tanh>("Quad tanh", qvalues[index0], mr, ma1);
    doTest<Octuple, tanh, mpfr_tanh>("Octuple tanh", ovalues[index0], mr, ma1);

    doTest<Half, asinh, mpfr_asinh>("Half asinh", hvalues[index0], mr, ma1);
    doTest<Float, asinh, mpfr_asinh>("Float asinh", fvalues[index0], mr, ma1);
    doTest<Double, asinh, mpfr_asinh>("Double asinh", dvalues[index0], mr, ma1);
    doTest<Quad, asinh, mpfr_asinh>("Quad asinh", qvalues[index0], mr, ma1);
    doTest<Octuple, asinh, mpfr_asinh>("Octuple asinh", ovalues[index0], mr, ma1);

    doTest<Half, acosh, mpfr_acosh>("Half acosh", hvalues[index0], mr, ma1);
    doTest<Float, acosh, mpfr_acosh>("Float acosh", fvalues[index0], mr, ma1);
    doTest<Double, acosh, mpfr_acosh>("Double acosh", dvalues[index0], mr, ma1);
    doTest<Quad, acosh, mpfr_acosh>("Quad acosh", qvalues[index0], mr, ma1);
    doTest<Octuple, acosh, mpfr_acosh>("Octuple acosh", ovalues[index0], mr, ma1);

    doTest<Half, atanh, mpfr_atanh>("Half atanh", hvalues[index0], mr, ma1);
    doTest<Float, atanh, mpfr_atanh>("Float atanh", fvalues[index0], mr, ma1);
    doTest<Double, atanh, mpfr_atanh>("Double atanh", dvalues[index0], mr, ma1);
    doTest<Quad, atanh, mpfr_atanh>("Quad atanh", qvalues[index0], mr, ma1);
    doTest<Octuple, atanh, mpfr_atanh>("Octuple atanh", ovalues[index0], mr, ma1);

    doTest<Half, erf, mpfr_erf>("Half erf", hvalues[index0], mr, ma1);
    doTest<Float, erf, mpfr_erf>("Float erf", fvalues[index0], mr, ma1);
    doTest<Double, erf, mpfr_erf>("Double erf", dvalues[index0], mr, ma1);
    doTest<Quad, erf, mpfr_erf>("Quad erf", qvalues[index0], mr, ma1);
    doTest<Octuple, erf, mpfr_erf>("Octuple erf", ovalues[index0], mr, ma1);

    doTest<Half, erfc, mpfr_erfc>("Half erfc", hvalues[index0], mr, ma1);
    doTest<Float, erfc, mpfr_erfc>("Float erfc", fvalues[index0], mr, ma1);
    doTest<Double, erfc, mpfr_erfc>("Double erfc", dvalues[index0], mr, ma1);
    doTest<Quad, erfc, mpfr_erfc>("Quad erfc", qvalues[index0], mr, ma1);
    doTest<Octuple, erfc, mpfr_erfc>("Octuple erfc", ovalues[index0], mr, ma1);

    doTest<Half, tgamma, mpfr_gamma>("Half tgamma", hvalues[index0], mr, ma1);
    doTest<Float, tgamma, mpfr_gamma>("Float tgamma", fvalues[index0], mr, ma1);
    doTest<Double, tgamma, mpfr_gamma>("Double tgamma", dvalues[index0], mr, ma1);
    doTest<Quad, tgamma, mpfr_gamma>("Quad tgamma", qvalues[index0], mr, ma1);
    doTest<Octuple, tgamma, mpfr_gamma>("Octuple tgamma", ovalues[index0], mr, ma1);

#if 0
    doTest<Half, lgamma_, mpfr_lgamma_>("Half lgamma", hvalues[index0], mr, ma1);
    doTest<Float, lgamma_, mpfr_lgamma_>("Float lgamma", fvalues[index0], mr, ma1);
    doTest<Double, lgamma_, mpfr_lgamma_>("Double lgamma", dvalues[index0], mr, ma1);
    doTest<Quad, lgamma_, mpfr_lgamma_>("Quad lgamma", qvalues[index0], mr, ma1);
    doTest<Octuple, lgamma_, mpfr_lgamma_>("Octuple lgamma", ovalues[index0], mr, ma1);
#endif

    for(int index1 = 0;index1 < N;index1++) {
      doTest<Half, copysign_, mpfr_copysign>("Half copysign", hvalues[index0], hvalues[index1], mr, ma1, ma2, true);
      doTest<Float, copysign_, mpfr_copysign>("Float copysign", fvalues[index0], fvalues[index1], mr, ma1, ma2, true);
      doTest<Double, copysign_, mpfr_copysign>("Double copysign", dvalues[index0], dvalues[index1], mr, ma1, ma2, true);
      doTest<Quad, copysign_, mpfr_copysign>("Quad copysign", qvalues[index0], qvalues[index1], mr, ma1, ma2, true);
      doTest<Octuple, copysign_, mpfr_copysign>("Octuple copysign", ovalues[index0], ovalues[index1], mr, ma1, ma2, true);

      doTest<Half, fdim_, mpfr_dim>("Half fdim", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, fdim_, mpfr_dim>("Float fdim", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, fdim_, mpfr_dim>("Double fdim", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, fdim_, mpfr_dim>("Quad fdim", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, fdim_, mpfr_dim>("Octuple fdim", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, add_, mpfr_add>("Half add", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, add_, mpfr_add>("Float add", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, add_, mpfr_add>("Double add", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, add_, mpfr_add>("Quad add", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, add_, mpfr_add>("Octuple add", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, sub_, mpfr_sub>("Half sub", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, sub_, mpfr_sub>("Float sub", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, sub_, mpfr_sub>("Double sub", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, sub_, mpfr_sub>("Quad sub", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, sub_, mpfr_sub>("Octuple sub", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, mul_, mpfr_mul>("Half mul", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, mul_, mpfr_mul>("Float mul", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, mul_, mpfr_mul>("Double mul", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, mul_, mpfr_mul>("Quad mul", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, mul_, mpfr_mul>("Octuple mul", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, div_, mpfr_div>("Half div", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, div_, mpfr_div>("Float div", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, div_, mpfr_div>("Double div", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, div_, mpfr_div>("Quad div", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, div_, mpfr_div>("Octuple div", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, hypot_, mpfr_hypot>("Half hypot", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, hypot_, mpfr_hypot>("Float hypot", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, hypot_, mpfr_hypot>("Double hypot", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, hypot_, mpfr_hypot>("Quad hypot", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, hypot_, mpfr_hypot>("Octuple hypot", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, atan2, mpfr_atan2>("Half atan2", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, atan2, mpfr_atan2>("Float atan2", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, atan2, mpfr_atan2>("Double atan2", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, atan2, mpfr_atan2>("Quad atan2", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, atan2, mpfr_atan2>("Octuple atan2", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, pow, mpfr_pow>("Half pow", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, pow, mpfr_pow>("Float pow", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, pow, mpfr_pow>("Double pow", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, pow, mpfr_pow>("Quad pow", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, pow, mpfr_pow>("Octuple pow", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, fmod, mpfr_fmod>("Half fmod", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, fmod, mpfr_fmod>("Float fmod", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, fmod, mpfr_fmod>("Double fmod", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, fmod, mpfr_fmod>("Quad fmod", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, fmod, mpfr_fmod>("Octuple fmod", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      doTest<Half, remainder, mpfr_remainder>("Half remainder", hvalues[index0], hvalues[index1], mr, ma1, ma2);
      doTest<Float, remainder, mpfr_remainder>("Float remainder", fvalues[index0], fvalues[index1], mr, ma1, ma2);
      doTest<Double, remainder, mpfr_remainder>("Double remainder", dvalues[index0], dvalues[index1], mr, ma1, ma2);
      doTest<Quad, remainder, mpfr_remainder>("Quad remainder", qvalues[index0], qvalues[index1], mr, ma1, ma2);
      doTest<Octuple, remainder, mpfr_remainder>("Octuple remainder", ovalues[index0], ovalues[index1], mr, ma1, ma2);

      for(int index2 = 0;index2 < N;index2++) {
	doTest<Half, fma_, mpfr_fma>("Half fma", hvalues[index0], hvalues[index1], hvalues[index2], mr, ma1, ma2, ma3);
	doTest<Float, fma_, mpfr_fma>("Float fma", fvalues[index0], fvalues[index1], fvalues[index2], mr, ma1, ma2, ma3);
	doTest<Double, fma_, mpfr_fma>("Double fma", dvalues[index0], dvalues[index1], dvalues[index2], mr, ma1, ma2, ma3);
	doTest<Quad, fma_, mpfr_fma>("Quad fma", qvalues[index0], qvalues[index1], qvalues[index2], mr, ma1, ma2, ma3);
	doTest<Octuple, fma_, mpfr_fma>("Octuple fma", ovalues[index0], ovalues[index1], ovalues[index2], mr, ma1, ma2, ma3);
      }
    }
  }

  mpfr_clears(mr, ma1, ma2, ma3, NULL);

  cout << "OK" << endl;

  exit(0);
}
