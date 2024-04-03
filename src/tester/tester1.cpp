#include <iostream>

#if (defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)) || defined(__aarch64__)
#define MPFR_WANT_FLOAT128
#endif
#include <mpfr.h>

#include <tlmath.hpp>
#include <tlfloatutil.hpp>
#include <auxiliary.hpp>

using namespace std;
using namespace tlfloat;

const int N = 64;

template<typename T> T values[N] = {
  T("+0.0"),
  T("-0.0"),
  T::nan(),
  T::inf(false),
  T::inf(true),
  T::floatmin(false),
  nextafter(T::floatmin(false), 1),
  nextafter(nextafter(T::floatmin(false), 1), 1),
  nextafter(nextafter(nextafter(T::floatmin(false), 1), 1), 1),
  nextafter(nextafter(nextafter(nextafter(T::floatmin(false), 1), 1), 1), 1),
  T::floatmin(true),
  nextafter(T::floatmin(true), -1),
  nextafter(nextafter(T::floatmin(true), -1), -1),
  nextafter(nextafter(nextafter(T::floatmin(true), -1), -1), -1),
  nextafter(nextafter(nextafter(nextafter(T::floatmin(true), -1), -1), -1), -1),
  T::floatmax(false),
  nextafter(T::floatmax(false), 0),
  nextafter(nextafter(T::floatmax(false), 0), 0),
  nextafter(nextafter(nextafter(T::floatmax(false), 0), 0), 0),
  nextafter(nextafter(nextafter(nextafter(T::floatmax(false), 0), 0), 0), 0),
  T::floatmax(true),
  nextafter(T::floatmax(true), 0),
  nextafter(nextafter(T::floatmax(true), 0), 0),
  nextafter(nextafter(nextafter(T::floatmax(true), 0), 0), 0),
  nextafter(nextafter(nextafter(nextafter(T::floatmax(true), 0), 0), 0), 0),
  T("+0.25"),
  T("-0.25"),
  T("+0.5"),
  T("-0.5"),
  T("+1.0"),
  T("-1.0"),
  T("+1.5"),
  T("-1.5"),
  T("+2.0"),
  T("-2.0"),
  T("+2.5"),
  T("-2.5"),
  T("+3.0"),
  T("-3.0"),
  T("+4.0"),
  T("-4.0"),
  T("+100.0"),
  T("-100.0"),
  T("+100.5"),
  T("-100.5"),
  T("+101.0"),
  T("-101.0"),
  T("+102.0"),
  T("-102.0"),
  T("+103.0"),
  T("-103.0"),
  T("+1.234"),
  T("-1.234"),
  T("+1.234e+10"),
  T("-1.234e+10"),
  T("+1.234e+30"),
  T("-1.234e+30"),
  T("+1.234e-10"),
  T("-1.234e-10"),
  T("+1.234e-30"),
  T("-1.234e-30"),
  T("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170676"),
  T("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170676e+30"),
  T("2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274"),
};

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

template<typename T, Func1<T> func, MPFRFunc1 mpfrFunc>
void doTest(const char *mes, T a1, mpfr_t &mr, mpfr_t &ma1) {
  T r = func(a1);
  typedef decltype(r.getUnpacked()) Unpacked_t;

  mpfr_set_unpacked(ma1, a1.getUnpacked(), GMP_RNDN);
  mpfrFunc(mr, ma1, GMP_RNDN);
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatmin(), Unpacked_t::floatmax(), true);

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
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatmin(), Unpacked_t::floatmax(), true);

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
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatmin(), Unpacked_t::floatmax(), true);

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
  double ulp = countULP(r.getUnpacked(), mr, Unpacked_t::floatmin(), Unpacked_t::floatmax(), true);

  if (ulp <= 1.0) return;

  cout << mes << endl;
  cout << "arg1    : " << to_string(a1, 72) << endl;
  cout << "arg2    : " << to_string(a2, 72) << endl;
  cout << "tlfloat : " << to_string(r, 72) << endl;
  cout << "mpfr    : " << to_string(mr, 72) << endl;
  exit(-1);
}

int main(int argc, char **argv) {
  mpfr_set_default_prec(256);

  mpfr_t mr, ma1, ma2, ma3;
  mpfr_inits(mr, ma1, ma2, ma3, NULL);

  for(int index0 = 0;index0 < N;index0++) {
    doTest<Half, fabs_, mpfr_abs>("Half fabs", values<Half>[index0], mr, ma1);
    doTest<Float, fabs_, mpfr_abs>("Float fabs", values<Float>[index0], mr, ma1);
    doTest<Double, fabs_, mpfr_abs>("Double fabs", values<Double>[index0], mr, ma1);
    doTest<Quad, fabs_, mpfr_abs>("Quad fabs", values<Quad>[index0], mr, ma1);
    doTest<Octuple, fabs_, mpfr_abs>("Octuple fabs", values<Octuple>[index0], mr, ma1);

    doTest<Half, sqrt_, mpfr_sqrt>("Half sqrt", values<Half>[index0], mr, ma1);
    doTest<Float, sqrt_, mpfr_sqrt>("Float sqrt", values<Float>[index0], mr, ma1);
    doTest<Double, sqrt_, mpfr_sqrt>("Double sqrt", values<Double>[index0], mr, ma1);
    doTest<Quad, sqrt_, mpfr_sqrt>("Quad sqrt", values<Quad>[index0], mr, ma1);
    doTest<Octuple, sqrt_, mpfr_sqrt>("Octuple sqrt", values<Octuple>[index0], mr, ma1);

    doTest<Half, trunc_, mpfr_trunc>("Half trunc", values<Half>[index0], mr, ma1);
    doTest<Float, trunc_, mpfr_trunc>("Float trunc", values<Float>[index0], mr, ma1);
    doTest<Double, trunc_, mpfr_trunc>("Double trunc", values<Double>[index0], mr, ma1);
    doTest<Quad, trunc_, mpfr_trunc>("Quad trunc", values<Quad>[index0], mr, ma1);
    doTest<Octuple, trunc_, mpfr_trunc>("Octuple trunc", values<Octuple>[index0], mr, ma1);

    doTest<Half, floor_, mpfr_floor>("Half floor", values<Half>[index0], mr, ma1);
    doTest<Float, floor_, mpfr_floor>("Float floor", values<Float>[index0], mr, ma1);
    doTest<Double, floor_, mpfr_floor>("Double floor", values<Double>[index0], mr, ma1);
    doTest<Quad, floor_, mpfr_floor>("Quad floor", values<Quad>[index0], mr, ma1);
    doTest<Octuple, floor_, mpfr_floor>("Octuple floor", values<Octuple>[index0], mr, ma1);

    doTest<Half, ceil_, mpfr_ceil>("Half ceil", values<Half>[index0], mr, ma1);
    doTest<Float, ceil_, mpfr_ceil>("Float ceil", values<Float>[index0], mr, ma1);
    doTest<Double, ceil_, mpfr_ceil>("Double ceil", values<Double>[index0], mr, ma1);
    doTest<Quad, ceil_, mpfr_ceil>("Quad ceil", values<Quad>[index0], mr, ma1);
    doTest<Octuple, ceil_, mpfr_ceil>("Octuple ceil", values<Octuple>[index0], mr, ma1);

    doTest<Half, round_, mpfr_round>("Half round", values<Half>[index0], mr, ma1);
    doTest<Float, round_, mpfr_round>("Float round", values<Float>[index0], mr, ma1);
    doTest<Double, round_, mpfr_round>("Double round", values<Double>[index0], mr, ma1);
    doTest<Quad, round_, mpfr_round>("Quad round", values<Quad>[index0], mr, ma1);
    doTest<Octuple, round_, mpfr_round>("Octuple round", values<Octuple>[index0], mr, ma1);

    doTest<Half, rint_, mpfr_rint>("Half rint", values<Half>[index0], mr, ma1);
    doTest<Float, rint_, mpfr_rint>("Float rint", values<Float>[index0], mr, ma1);
    doTest<Double, rint_, mpfr_rint>("Double rint", values<Double>[index0], mr, ma1);
    doTest<Quad, rint_, mpfr_rint>("Quad rint", values<Quad>[index0], mr, ma1);
    doTest<Octuple, rint_, mpfr_rint>("Octuple rint", values<Octuple>[index0], mr, ma1);

    doTest<Half, sin, mpfr_sin>("Half sin", values<Half>[index0], mr, ma1);
    doTest<Float, sin, mpfr_sin>("Float sin", values<Float>[index0], mr, ma1);
    doTest<Double, sin, mpfr_sin>("Double sin", values<Double>[index0], mr, ma1);
    doTest<Quad, sin, mpfr_sin>("Quad sin", values<Quad>[index0], mr, ma1);
    doTest<Octuple, sin, mpfr_sin>("Octuple sin", values<Octuple>[index0], mr, ma1);

    doTest<Half, cos, mpfr_cos>("Half cos", values<Half>[index0], mr, ma1);
    doTest<Float, cos, mpfr_cos>("Float cos", values<Float>[index0], mr, ma1);
    doTest<Double, cos, mpfr_cos>("Double cos", values<Double>[index0], mr, ma1);
    doTest<Quad, cos, mpfr_cos>("Quad cos", values<Quad>[index0], mr, ma1);
    doTest<Octuple, cos, mpfr_cos>("Octuple cos", values<Octuple>[index0], mr, ma1);

    doTest<Half, tan, mpfr_tan>("Half tan", values<Half>[index0], mr, ma1);
    doTest<Float, tan, mpfr_tan>("Float tan", values<Float>[index0], mr, ma1);
    doTest<Double, tan, mpfr_tan>("Double tan", values<Double>[index0], mr, ma1);
    doTest<Quad, tan, mpfr_tan>("Quad tan", values<Quad>[index0], mr, ma1);
    doTest<Octuple, tan, mpfr_tan>("Octuple tan", values<Octuple>[index0], mr, ma1);

    doTest<Half, asin, mpfr_asin>("Half asin", values<Half>[index0], mr, ma1);
    doTest<Float, asin, mpfr_asin>("Float asin", values<Float>[index0], mr, ma1);
    doTest<Double, asin, mpfr_asin>("Double asin", values<Double>[index0], mr, ma1);
    doTest<Quad, asin, mpfr_asin>("Quad asin", values<Quad>[index0], mr, ma1);
    doTest<Octuple, asin, mpfr_asin>("Octuple asin", values<Octuple>[index0], mr, ma1);

    doTest<Half, acos, mpfr_acos>("Half acos", values<Half>[index0], mr, ma1);
    doTest<Float, acos, mpfr_acos>("Float acos", values<Float>[index0], mr, ma1);
    doTest<Double, acos, mpfr_acos>("Double acos", values<Double>[index0], mr, ma1);
    doTest<Quad, acos, mpfr_acos>("Quad acos", values<Quad>[index0], mr, ma1);
    doTest<Octuple, acos, mpfr_acos>("Octuple acos", values<Octuple>[index0], mr, ma1);

    doTest<Half, atan, mpfr_atan>("Half atan", values<Half>[index0], mr, ma1);
    doTest<Float, atan, mpfr_atan>("Float atan", values<Float>[index0], mr, ma1);
    doTest<Double, atan, mpfr_atan>("Double atan", values<Double>[index0], mr, ma1);
    doTest<Quad, atan, mpfr_atan>("Quad atan", values<Quad>[index0], mr, ma1);
    doTest<Octuple, atan, mpfr_atan>("Octuple atan", values<Octuple>[index0], mr, ma1);

    doTest<Half, log, mpfr_log>("Half log", values<Half>[index0], mr, ma1);
    doTest<Float, log, mpfr_log>("Float log", values<Float>[index0], mr, ma1);
    doTest<Double, log, mpfr_log>("Double log", values<Double>[index0], mr, ma1);
    doTest<Quad, log, mpfr_log>("Quad log", values<Quad>[index0], mr, ma1);
    doTest<Octuple, log, mpfr_log>("Octuple log", values<Octuple>[index0], mr, ma1);

    doTest<Half, log2, mpfr_log2>("Half log2", values<Half>[index0], mr, ma1);
    doTest<Float, log2, mpfr_log2>("Float log2", values<Float>[index0], mr, ma1);
    doTest<Double, log2, mpfr_log2>("Double log2", values<Double>[index0], mr, ma1);
    doTest<Quad, log2, mpfr_log2>("Quad log2", values<Quad>[index0], mr, ma1);
    doTest<Octuple, log2, mpfr_log2>("Octuple log2", values<Octuple>[index0], mr, ma1);

    doTest<Half, log10, mpfr_log10>("Half log10", values<Half>[index0], mr, ma1);
    doTest<Float, log10, mpfr_log10>("Float log10", values<Float>[index0], mr, ma1);
    doTest<Double, log10, mpfr_log10>("Double log10", values<Double>[index0], mr, ma1);
    doTest<Quad, log10, mpfr_log10>("Quad log10", values<Quad>[index0], mr, ma1);
    doTest<Octuple, log10, mpfr_log10>("Octuple log10", values<Octuple>[index0], mr, ma1);

    doTest<Half, log1p, mpfr_log1p>("Half log1p", values<Half>[index0], mr, ma1);
    doTest<Float, log1p, mpfr_log1p>("Float log1p", values<Float>[index0], mr, ma1);
    doTest<Double, log1p, mpfr_log1p>("Double log1p", values<Double>[index0], mr, ma1);
    doTest<Quad, log1p, mpfr_log1p>("Quad log1p", values<Quad>[index0], mr, ma1);
    doTest<Octuple, log1p, mpfr_log1p>("Octuple log1p", values<Octuple>[index0], mr, ma1);

    doTest<Half, exp, mpfr_exp>("Half exp", values<Half>[index0], mr, ma1);
    doTest<Float, exp, mpfr_exp>("Float exp", values<Float>[index0], mr, ma1);
    doTest<Double, exp, mpfr_exp>("Double exp", values<Double>[index0], mr, ma1);
    doTest<Quad, exp, mpfr_exp>("Quad exp", values<Quad>[index0], mr, ma1);
    doTest<Octuple, exp, mpfr_exp>("Octuple exp", values<Octuple>[index0], mr, ma1);

    doTest<Half, exp2, mpfr_exp2>("Half exp2", values<Half>[index0], mr, ma1);
    doTest<Float, exp2, mpfr_exp2>("Float exp2", values<Float>[index0], mr, ma1);
    doTest<Double, exp2, mpfr_exp2>("Double exp2", values<Double>[index0], mr, ma1);
    doTest<Quad, exp2, mpfr_exp2>("Quad exp2", values<Quad>[index0], mr, ma1);
    doTest<Octuple, exp2, mpfr_exp2>("Octuple exp2", values<Octuple>[index0], mr, ma1);

    doTest<Half, exp10, mpfr_exp10>("Half exp10", values<Half>[index0], mr, ma1);
    doTest<Float, exp10, mpfr_exp10>("Float exp10", values<Float>[index0], mr, ma1);
    doTest<Double, exp10, mpfr_exp10>("Double exp10", values<Double>[index0], mr, ma1);
    doTest<Quad, exp10, mpfr_exp10>("Quad exp10", values<Quad>[index0], mr, ma1);
    doTest<Octuple, exp10, mpfr_exp10>("Octuple exp10", values<Octuple>[index0], mr, ma1);

    doTest<Half, expm1, mpfr_expm1>("Half expm1", values<Half>[index0], mr, ma1);
    doTest<Float, expm1, mpfr_expm1>("Float expm1", values<Float>[index0], mr, ma1);
    doTest<Double, expm1, mpfr_expm1>("Double expm1", values<Double>[index0], mr, ma1);
    doTest<Quad, expm1, mpfr_expm1>("Quad expm1", values<Quad>[index0], mr, ma1);
    doTest<Octuple, expm1, mpfr_expm1>("Octuple expm1", values<Octuple>[index0], mr, ma1);

    doTest<Half, sinh, mpfr_sinh>("Half sinh", values<Half>[index0], mr, ma1);
    doTest<Float, sinh, mpfr_sinh>("Float sinh", values<Float>[index0], mr, ma1);
    doTest<Double, sinh, mpfr_sinh>("Double sinh", values<Double>[index0], mr, ma1);
    doTest<Quad, sinh, mpfr_sinh>("Quad sinh", values<Quad>[index0], mr, ma1);
    doTest<Octuple, sinh, mpfr_sinh>("Octuple sinh", values<Octuple>[index0], mr, ma1);

    doTest<Half, cosh, mpfr_cosh>("Half cosh", values<Half>[index0], mr, ma1);
    doTest<Float, cosh, mpfr_cosh>("Float cosh", values<Float>[index0], mr, ma1);
    doTest<Double, cosh, mpfr_cosh>("Double cosh", values<Double>[index0], mr, ma1);
    doTest<Quad, cosh, mpfr_cosh>("Quad cosh", values<Quad>[index0], mr, ma1);
    doTest<Octuple, cosh, mpfr_cosh>("Octuple cosh", values<Octuple>[index0], mr, ma1);

    doTest<Half, tanh, mpfr_tanh>("Half tanh", values<Half>[index0], mr, ma1);
    doTest<Float, tanh, mpfr_tanh>("Float tanh", values<Float>[index0], mr, ma1);
    doTest<Double, tanh, mpfr_tanh>("Double tanh", values<Double>[index0], mr, ma1);
    doTest<Quad, tanh, mpfr_tanh>("Quad tanh", values<Quad>[index0], mr, ma1);
    doTest<Octuple, tanh, mpfr_tanh>("Octuple tanh", values<Octuple>[index0], mr, ma1);

    doTest<Half, asinh, mpfr_asinh>("Half asinh", values<Half>[index0], mr, ma1);
    doTest<Float, asinh, mpfr_asinh>("Float asinh", values<Float>[index0], mr, ma1);
    doTest<Double, asinh, mpfr_asinh>("Double asinh", values<Double>[index0], mr, ma1);
    doTest<Quad, asinh, mpfr_asinh>("Quad asinh", values<Quad>[index0], mr, ma1);
    doTest<Octuple, asinh, mpfr_asinh>("Octuple asinh", values<Octuple>[index0], mr, ma1);

    doTest<Half, acosh, mpfr_acosh>("Half acosh", values<Half>[index0], mr, ma1);
    doTest<Float, acosh, mpfr_acosh>("Float acosh", values<Float>[index0], mr, ma1);
    doTest<Double, acosh, mpfr_acosh>("Double acosh", values<Double>[index0], mr, ma1);
    doTest<Quad, acosh, mpfr_acosh>("Quad acosh", values<Quad>[index0], mr, ma1);
    doTest<Octuple, acosh, mpfr_acosh>("Octuple acosh", values<Octuple>[index0], mr, ma1);

    doTest<Half, atanh, mpfr_atanh>("Half atanh", values<Half>[index0], mr, ma1);
    doTest<Float, atanh, mpfr_atanh>("Float atanh", values<Float>[index0], mr, ma1);
    doTest<Double, atanh, mpfr_atanh>("Double atanh", values<Double>[index0], mr, ma1);
    doTest<Quad, atanh, mpfr_atanh>("Quad atanh", values<Quad>[index0], mr, ma1);
    doTest<Octuple, atanh, mpfr_atanh>("Octuple atanh", values<Octuple>[index0], mr, ma1);

    for(int index1 = 0;index1 < N;index1++) {
      doTest<Half, copysign_, mpfr_copysign>("Half copysign", values<Half>[index0], values<Half>[index1], mr, ma1, ma2, true);
      doTest<Float, copysign_, mpfr_copysign>("Float copysign", values<Float>[index0], values<Float>[index1], mr, ma1, ma2, true);
      doTest<Double, copysign_, mpfr_copysign>("Double copysign", values<Double>[index0], values<Double>[index1], mr, ma1, ma2, true);
      doTest<Quad, copysign_, mpfr_copysign>("Quad copysign", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2, true);
      doTest<Octuple, copysign_, mpfr_copysign>("Octuple copysign", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2, true);

      doTest<Half, fdim_, mpfr_dim>("Half fdim", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, fdim_, mpfr_dim>("Float fdim", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, fdim_, mpfr_dim>("Double fdim", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, fdim_, mpfr_dim>("Quad fdim", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, fdim_, mpfr_dim>("Octuple fdim", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, add_, mpfr_add>("Half add", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, add_, mpfr_add>("Float add", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, add_, mpfr_add>("Double add", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, add_, mpfr_add>("Quad add", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, add_, mpfr_add>("Octuple add", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, sub_, mpfr_sub>("Half sub", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, sub_, mpfr_sub>("Float sub", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, sub_, mpfr_sub>("Double sub", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, sub_, mpfr_sub>("Quad sub", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, sub_, mpfr_sub>("Octuple sub", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, mul_, mpfr_mul>("Half mul", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, mul_, mpfr_mul>("Float mul", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, mul_, mpfr_mul>("Double mul", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, mul_, mpfr_mul>("Quad mul", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, mul_, mpfr_mul>("Octuple mul", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, div_, mpfr_div>("Half div", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, div_, mpfr_div>("Float div", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, div_, mpfr_div>("Double div", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, div_, mpfr_div>("Quad div", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, div_, mpfr_div>("Octuple div", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, hypot_, mpfr_hypot>("Half hypot", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, hypot_, mpfr_hypot>("Float hypot", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, hypot_, mpfr_hypot>("Double hypot", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, hypot_, mpfr_hypot>("Quad hypot", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, hypot_, mpfr_hypot>("Octuple hypot", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, atan2, mpfr_atan2>("Half atan2", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, atan2, mpfr_atan2>("Float atan2", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, atan2, mpfr_atan2>("Double atan2", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, atan2, mpfr_atan2>("Quad atan2", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, atan2, mpfr_atan2>("Octuple atan2", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, pow, mpfr_pow>("Half pow", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, pow, mpfr_pow>("Float pow", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, pow, mpfr_pow>("Double pow", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, pow, mpfr_pow>("Quad pow", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, pow, mpfr_pow>("Octuple pow", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, fmod, mpfr_fmod>("Half fmod", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, fmod, mpfr_fmod>("Float fmod", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, fmod, mpfr_fmod>("Double fmod", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, fmod, mpfr_fmod>("Quad fmod", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, fmod, mpfr_fmod>("Octuple fmod", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      doTest<Half, remainder, mpfr_remainder>("Half remainder", values<Half>[index0], values<Half>[index1], mr, ma1, ma2);
      doTest<Float, remainder, mpfr_remainder>("Float remainder", values<Float>[index0], values<Float>[index1], mr, ma1, ma2);
      doTest<Double, remainder, mpfr_remainder>("Double remainder", values<Double>[index0], values<Double>[index1], mr, ma1, ma2);
      doTest<Quad, remainder, mpfr_remainder>("Quad remainder", values<Quad>[index0], values<Quad>[index1], mr, ma1, ma2);
      doTest<Octuple, remainder, mpfr_remainder>("Octuple remainder", values<Octuple>[index0], values<Octuple>[index1], mr, ma1, ma2);

      for(int index2 = 0;index2 < N;index2++) {
	doTest<Half, fma_, mpfr_fma>("Half fma", values<Half>[index0], values<Half>[index1], values<Half>[index2], mr, ma1, ma2, ma3);
	doTest<Float, fma_, mpfr_fma>("Float fma", values<Float>[index0], values<Float>[index1], values<Float>[index2], mr, ma1, ma2, ma3);
	doTest<Double, fma_, mpfr_fma>("Double fma", values<Double>[index0], values<Double>[index1], values<Double>[index2], mr, ma1, ma2, ma3);
	doTest<Quad, fma_, mpfr_fma>("Quad fma", values<Quad>[index0], values<Quad>[index1], values<Quad>[index2], mr, ma1, ma2, ma3);
	doTest<Octuple, fma_, mpfr_fma>("Octuple fma", values<Octuple>[index0], values<Octuple>[index1], values<Octuple>[index2], mr, ma1, ma2, ma3);
      }
    }
  }

  mpfr_clears(mr, ma1, ma2, ma3, NULL);

  cout << "OK" << endl;

  exit(0);
}
