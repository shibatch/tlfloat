#include <iostream>
#include <cassert>

#include "tlmath.hpp"
#include "auxiliary.hpp"

using namespace std;
using namespace tlfloat;

int N = 10000;

auto sha256 = PSHA2_256_Internal();

template<typename T>
T rnd(shared_ptr<RNG> rng) {
  for(;;) {
    T f;
    rng->nextBytes((unsigned char *)&f, sizeof(f));
    f = frexp(f, nullptr);
    if (finite(f)) return f;
  }
}

//

template<typename T>
using Func1 = T (*)(const T&);

template<typename T, Func1<T> func>
void computeHash(const char *mes, T *r, T *x) {
  for (int i = 0; i < N; i++) {
    auto m = func(x[i]).m;
    sha256.append((void *)&m, sizeof(m));
  }
}

template<typename T>
using Func2 = T (*)(const T&, const T&);

template<typename T, Func2<T> func>
void computeHash(const char *mes, T *r, T *x, T *y) {
  for (int i = 0; i < N; i++) {
    auto m = func(x[i], y[i]).m;
    sha256.append((void *)&m, sizeof(m));
  }
}

template<typename T>
using Func3 = T (*)(const T&, const T&, const T&);

template<typename T, Func3<T> func>
void computeHash(const char *mes, T *r, T *x, T *y, T *z) {
  for (int i = 0; i < N; i++) {
    auto m = func(x[i], y[i], z[i]).m;
    sha256.append((void *)&m, sizeof(m));
  }
}

//

template<typename T> static constexpr T fabs_(const T &a1) { return fabs(a1); }
template<typename T> static constexpr T copysign_(const T &a1, const T &a2) { return copysign(a1, a2); }
template<typename T> static constexpr T fmax_(const T &a1, const T &a2) { return fmax(a1, a2); }
template<typename T> static constexpr T fmin_(const T &a1, const T &a2) { return fmin(a1, a2); }
template<typename T> static constexpr T fdim_(const T &a1, const T &a2) { return fdim(a1, a2); }
template<typename T> static constexpr T add_(const T &a1, const T &a2) { return a1 + a2; }
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

//

int main(int argc, char **argv) {
  auto rng = shared_ptr<RNG>(new LCG64(0));

  Octuple *xo = (Octuple *)malloc(N*sizeof(Octuple));
  Octuple *yo = (Octuple *)malloc(N*sizeof(Octuple));
  Octuple *zo = (Octuple *)malloc(N*sizeof(Octuple));
  Octuple *ro = (Octuple *)malloc(N*sizeof(Octuple));

  Quad *xq = (Quad *)malloc(N*sizeof(Quad));
  Quad *yq = (Quad *)malloc(N*sizeof(Quad));
  Quad *zq = (Quad *)malloc(N*sizeof(Quad));
  Quad *rq = (Quad *)malloc(N*sizeof(Quad));

  Double *xd = (Double *)malloc(N*sizeof(Double));
  Double *yd = (Double *)malloc(N*sizeof(Double));
  Double *zd = (Double *)malloc(N*sizeof(Double));
  Double *rd = (Double *)malloc(N*sizeof(Double));

  Float *xf = (Float *)malloc(N*sizeof(Float));
  Float *yf = (Float *)malloc(N*sizeof(Float));
  Float *zf = (Float *)malloc(N*sizeof(Float));
  Float *rf = (Float *)malloc(N*sizeof(Float));

  Half *xh = (Half *)malloc(N*sizeof(Half));
  Half *yh = (Half *)malloc(N*sizeof(Half));
  Half *zh = (Half *)malloc(N*sizeof(Half));
  Half *rh = (Half *)malloc(N*sizeof(Half));

  for (int i = 0; i < N; i++) {
    xo[i] = rnd<Octuple>(rng);
    yo[i] = rnd<Octuple>(rng);
    zo[i] = rnd<Octuple>(rng);

    xq[i] = rnd<Quad>(rng);
    yq[i] = rnd<Quad>(rng);
    zq[i] = rnd<Quad>(rng);

    xd[i] = rnd<Double>(rng);
    yd[i] = rnd<Double>(rng);
    zd[i] = rnd<Double>(rng);

    xf[i] = rnd<Float>(rng);
    yf[i] = rnd<Float>(rng);
    zf[i] = rnd<Float>(rng);

    xh[i] = rnd<Half>(rng);
    yh[i] = rnd<Half>(rng);
    zh[i] = rnd<Half>(rng);
  }

  computeHash<Octuple, fabs_>("Octuple fabs", ro, xo);
  computeHash<Quad, fabs_>("Quad fabs", rq, xq);
  computeHash<Double, fabs_>("Double fabs", rd, xd);
  computeHash<Float, fabs_>("Float fabs", rf, xf);
  computeHash<Half, fabs_>("Half fabs", rh, xh);

  computeHash<Octuple, copysign_>("Octuple copysign", ro, xo, yo);
  computeHash<Quad, copysign_>("Quad copysign", rq, xq, yq);
  computeHash<Double, copysign_>("Double copysign", rd, xd, yd);
  computeHash<Float, copysign_>("Float copysign", rf, xf, yf);
  computeHash<Half, copysign_>("Half copysign", rh, xh, yh);

  computeHash<Octuple, fmax_>("Octuple fmax", ro, xo, yo);
  computeHash<Quad, fmax_>("Quad fmax", rq, xq, yq);
  computeHash<Double, fmax_>("Double fmax", rd, xd, yd);
  computeHash<Float, fmax_>("Float fmax", rf, xf, yf);
  computeHash<Half, fmax_>("Half fmax", rh, xh, yh);

  computeHash<Octuple, fmin_>("Octuple fmin", ro, xo, yo);
  computeHash<Quad, fmin_>("Quad fmin", rq, xq, yq);
  computeHash<Double, fmin_>("Double fmin", rd, xd, yd);
  computeHash<Float, fmin_>("Float fmin", rf, xf, yf);
  computeHash<Half, fmin_>("Half fmin", rh, xh, yh);

  computeHash<Octuple, fdim_>("Octuple fdim", ro, xo, yo);
  computeHash<Quad, fdim_>("Quad fdim", rq, xq, yq);
  computeHash<Double, fdim_>("Double fdim", rd, xd, yd);
  computeHash<Float, fdim_>("Float fdim", rf, xf, yf);
  computeHash<Half, fdim_>("Half fdim", rh, xh, yh);

  computeHash<Octuple, add_>("Octuple add_", ro, xo, yo);
  computeHash<Quad, add_>("Quad add_", rq, xq, yq);
  computeHash<Double, add_>("Double add_", rd, xd, yd);
  computeHash<Float, add_>("Float add_", rf, xf, yf);
  computeHash<Half, add_>("Half add_", rh, xh, yh);

  computeHash<Octuple, mul_>("Octuple mul_", ro, xo, yo);
  computeHash<Quad, mul_>("Quad mul_", rq, xq, yq);
  computeHash<Double, mul_>("Double mul_", rd, xd, yd);
  computeHash<Float, mul_>("Float mul_", rf, xf, yf);
  computeHash<Half, mul_>("Half mul_", rh, xh, yh);

  computeHash<Octuple, div_>("Octuple div_", ro, xo, yo);
  computeHash<Quad, div_>("Quad div_", rq, xq, yq);
  computeHash<Double, div_>("Double div_", rd, xd, yd);
  computeHash<Float, div_>("Float div_", rf, xf, yf);
  computeHash<Half, div_>("Half div_", rh, xh, yh);

  computeHash<Octuple, fma_>("Octuple fma", ro, xo, yo, zo);
  computeHash<Quad, fma_>("Quad fma", rq, xq, yq, zq);
  computeHash<Double, fma_>("Double fma", rd, xd, yd, zd);
  computeHash<Float, fma_>("Float fma", rf, xf, yf, zf);
  computeHash<Half, fma_>("Half fma", rh, xh, yh, zh);

  computeHash<Octuple, sqrt_>("Octuple sqrt", ro, xo);
  computeHash<Quad, sqrt_>("Quad sqrt", rq, xq);
  computeHash<Double, sqrt_>("Double sqrt", rd, xd);
  computeHash<Float, sqrt_>("Float sqrt", rf, xf);
  computeHash<Half, sqrt_>("Half sqrt", rh, xh);

  computeHash<Octuple, hypot_>("Octuple hypot", ro, xo, yo);
  computeHash<Quad, hypot_>("Quad hypot", rq, xq, yq);
  computeHash<Double, hypot_>("Double hypot", rd, xd, yd);
  computeHash<Float, hypot_>("Float hypot", rf, xf, yf);
  computeHash<Half, hypot_>("Half hypot", rh, xh, yh);

  computeHash<Octuple, trunc_>("Octuple trunc", ro, xo);
  computeHash<Quad, trunc_>("Quad trunc", rq, xq);
  computeHash<Double, trunc_>("Double trunc", rd, xd);
  computeHash<Float, trunc_>("Float trunc", rf, xf);
  computeHash<Half, trunc_>("Half trunc", rh, xh);

  computeHash<Octuple, floor_>("Octuple floor", ro, xo);
  computeHash<Quad, floor_>("Quad floor", rq, xq);
  computeHash<Double, floor_>("Double floor", rd, xd);
  computeHash<Float, floor_>("Float floor", rf, xf);
  computeHash<Half, floor_>("Half floor", rh, xh);

  computeHash<Octuple, ceil_>("Octuple ceil", ro, xo);
  computeHash<Quad, ceil_>("Quad ceil", rq, xq);
  computeHash<Double, ceil_>("Double ceil", rd, xd);
  computeHash<Float, ceil_>("Float ceil", rf, xf);
  computeHash<Half, ceil_>("Half ceil", rh, xh);

  computeHash<Octuple, round_>("Octuple round", ro, xo);
  computeHash<Quad, round_>("Quad round", rq, xq);
  computeHash<Double, round_>("Double round", rd, xd);
  computeHash<Float, round_>("Float round", rf, xf);
  computeHash<Half, round_>("Half round", rh, xh);

  computeHash<Octuple, rint_>("Octuple rint", ro, xo);
  computeHash<Quad, rint_>("Quad rint", rq, xq);
  computeHash<Double, rint_>("Double rint", rd, xd);
  computeHash<Float, rint_>("Float rint", rf, xf);
  computeHash<Half, rint_>("Half rint", rh, xh);

  computeHash<Octuple, nextafter_>("Octuple nextafter", ro, xo, yo);
  computeHash<Quad, nextafter_>("Quad nextafter", rq, xq, yq);
  computeHash<Double, nextafter_>("Double nextafter", rd, xd, yd);
  computeHash<Float, nextafter_>("Float nextafter", rf, xf, yf);
  computeHash<Half, nextafter_>("Half nextafter", rh, xh, yh);

  computeHash<Octuple, sin>("Octuple sin", ro, xo);
  computeHash<Quad, sin>("Quad sin", rq, xq);
  computeHash<Double, sin>("Double sin", rd, xd);
  computeHash<Float, sin>("Float sin", rf, xf);
  computeHash<Half, sin>("Half sin", rh, xh);

  computeHash<Octuple, cos>("Octuple cos", ro, xo);
  computeHash<Quad, cos>("Quad cos", rq, xq);
  computeHash<Double, cos>("Double cos", rd, xd);
  computeHash<Float, cos>("Float cos", rf, xf);
  computeHash<Half, cos>("Half cos", rh, xh);

  computeHash<Octuple, tan>("Octuple tan", ro, xo);
  computeHash<Quad, tan>("Quad tan", rq, xq);
  computeHash<Double, tan>("Double tan", rd, xd);
  computeHash<Float, tan>("Float tan", rf, xf);
  computeHash<Half, tan>("Half tan", rh, xh);

  computeHash<Octuple, atan2>("Octuple atan2", ro, xo, yo);
  computeHash<Quad, atan2>("Quad atan2", rq, xq, yq);
  computeHash<Double, atan2>("Double atan2", rd, xd, yd);
  computeHash<Float, atan2>("Float atan2", rf, xf, yf);
  computeHash<Half, atan2>("Half atan2", rh, xh, yh);

  computeHash<Octuple, asin>("Octuple asin", ro, xo);
  computeHash<Quad, asin>("Quad asin", rq, xq);
  computeHash<Double, asin>("Double asin", rd, xd);
  computeHash<Float, asin>("Float asin", rf, xf);
  computeHash<Half, asin>("Half asin", rh, xh);

  computeHash<Octuple, acos>("Octuple acos", ro, xo);
  computeHash<Quad, acos>("Quad acos", rq, xq);
  computeHash<Double, acos>("Double acos", rd, xd);
  computeHash<Float, acos>("Float acos", rf, xf);
  computeHash<Half, acos>("Half acos", rh, xh);

  computeHash<Octuple, atan>("Octuple atan", ro, xo);
  computeHash<Quad, atan>("Quad atan", rq, xq);
  computeHash<Double, atan>("Double atan", rd, xd);
  computeHash<Float, atan>("Float atan", rf, xf);
  computeHash<Half, atan>("Half atan", rh, xh);

  computeHash<Octuple, log>("Octuple log", ro, xo);
  computeHash<Quad, log>("Quad log", rq, xq);
  computeHash<Double, log>("Double log", rd, xd);
  computeHash<Float, log>("Float log", rf, xf);
  computeHash<Half, log>("Half log", rh, xh);

  computeHash<Octuple, log2>("Octuple log2", ro, xo);
  computeHash<Quad, log2>("Quad log2", rq, xq);
  computeHash<Double, log2>("Double log2", rd, xd);
  computeHash<Float, log2>("Float log2", rf, xf);
  computeHash<Half, log2>("Half log2", rh, xh);

  computeHash<Octuple, log10>("Octuple log10", ro, xo);
  computeHash<Quad, log10>("Quad log10", rq, xq);
  computeHash<Double, log10>("Double log10", rd, xd);
  computeHash<Float, log10>("Float log10", rf, xf);
  computeHash<Half, log10>("Half log10", rh, xh);

  computeHash<Octuple, log1p>("Octuple log1p", ro, xo);
  computeHash<Quad, log1p>("Quad log1p", rq, xq);
  computeHash<Double, log1p>("Double log1p", rd, xd);
  computeHash<Float, log1p>("Float log1p", rf, xf);
  computeHash<Half, log1p>("Half log1p", rh, xh);

  computeHash<Octuple, exp>("Octuple exp", ro, xo);
  computeHash<Quad, exp>("Quad exp", rq, xq);
  computeHash<Double, exp>("Double exp", rd, xd);
  computeHash<Float, exp>("Float exp", rf, xf);
  computeHash<Half, exp>("Half exp", rh, xh);

  computeHash<Octuple, exp2>("Octuple exp2", ro, xo);
  computeHash<Quad, exp2>("Quad exp2", rq, xq);
  computeHash<Double, exp2>("Double exp2", rd, xd);
  computeHash<Float, exp2>("Float exp2", rf, xf);
  computeHash<Half, exp2>("Half exp2", rh, xh);

  computeHash<Octuple, exp10>("Octuple exp10", ro, xo);
  computeHash<Quad, exp10>("Quad exp10", rq, xq);
  computeHash<Double, exp10>("Double exp10", rd, xd);
  computeHash<Float, exp10>("Float exp10", rf, xf);
  computeHash<Half, exp10>("Half exp10", rh, xh);

  computeHash<Octuple, expm1>("Octuple expm1", ro, xo);
  computeHash<Quad, expm1>("Quad expm1", rq, xq);
  computeHash<Double, expm1>("Double expm1", rd, xd);
  computeHash<Float, expm1>("Float expm1", rf, xf);
  computeHash<Half, expm1>("Half expm1", rh, xh);

  computeHash<Octuple, pow>("Octuple pow", ro, xo, yo);
  computeHash<Quad, pow>("Quad pow", rq, xq, yq);
  computeHash<Double, pow>("Double pow", rd, xd, yd);
  computeHash<Float, pow>("Float pow", rf, xf, yf);
  computeHash<Half, pow>("Half pow", rh, xh, yh);

  computeHash<Octuple, cbrt>("Octuple cbrt", ro, xo);
  computeHash<Quad, cbrt>("Quad cbrt", rq, xq);
  computeHash<Double, cbrt>("Double cbrt", rd, xd);
  computeHash<Float, cbrt>("Float cbrt", rf, xf);
  computeHash<Half, cbrt>("Half cbrt", rh, xh);

  computeHash<Octuple, sinh>("Octuple sinh", ro, xo);
  computeHash<Quad, sinh>("Quad sinh", rq, xq);
  computeHash<Double, sinh>("Double sinh", rd, xd);
  computeHash<Float, sinh>("Float sinh", rf, xf);
  computeHash<Half, sinh>("Half sinh", rh, xh);

  computeHash<Octuple, cosh>("Octuple cosh", ro, xo);
  computeHash<Quad, cosh>("Quad cosh", rq, xq);
  computeHash<Double, cosh>("Double cosh", rd, xd);
  computeHash<Float, cosh>("Float cosh", rf, xf);
  computeHash<Half, cosh>("Half cosh", rh, xh);

  computeHash<Octuple, tanh>("Octuple tanh", ro, xo);
  computeHash<Quad, tanh>("Quad tanh", rq, xq);
  computeHash<Double, tanh>("Double tanh", rd, xd);
  computeHash<Float, tanh>("Float tanh", rf, xf);
  computeHash<Half, tanh>("Half tanh", rh, xh);

  computeHash<Octuple, asinh>("Octuple asinh", ro, xo);
  computeHash<Quad, asinh>("Quad asinh", rq, xq);
  computeHash<Double, asinh>("Double asinh", rd, xd);
  computeHash<Float, asinh>("Float asinh", rf, xf);
  computeHash<Half, asinh>("Half asinh", rh, xh);

  computeHash<Octuple, acosh>("Octuple acosh", ro, xo);
  computeHash<Quad, acosh>("Quad acosh", rq, xq);
  computeHash<Double, acosh>("Double acosh", rd, xd);
  computeHash<Float, acosh>("Float acosh", rf, xf);
  computeHash<Half, acosh>("Half acosh", rh, xh);

  computeHash<Octuple, atanh>("Octuple atanh", ro, xo);
  computeHash<Quad, atanh>("Quad atanh", rq, xq);
  computeHash<Double, atanh>("Double atanh", rd, xd);
  computeHash<Float, atanh>("Float atanh", rf, xf);
  computeHash<Half, atanh>("Half atanh", rh, xh);

  computeHash<Octuple, fmod>("Octuple fmod", ro, xo, yo);
  computeHash<Quad, fmod>("Quad fmod", rq, xq, yq);
  computeHash<Double, fmod>("Double fmod", rd, xd, yd);
  computeHash<Float, fmod>("Float fmod", rf, xf, yf);
  computeHash<Half, fmod>("Half fmod", rh, xh, yh);

  computeHash<Octuple, remainder>("Octuple remainder", ro, xo, yo);
  computeHash<Quad, remainder>("Quad remainder", rq, xq, yq);
  computeHash<Double, remainder>("Double remainder", rd, xd, yd);
  computeHash<Float, remainder>("Float remainder", rf, xf, yf);
  computeHash<Half, remainder>("Half remainder", rh, xh, yh);

  free(rh);
  free(zh);
  free(yh);
  free(xh);

  free(rf);
  free(zf);
  free(yf);
  free(xf);

  free(rd);
  free(zd);
  free(yd);
  free(xd);

  free(rq);
  free(zq);
  free(yq);
  free(xq);

  free(ro);
  free(zo);
  free(yo);
  free(xo);

  uint8_t h[32];
  sha256.finalize_bytes(&h);

  if (argc == 1) {
    for(int i=0;i<32;i++) printf("%02x", h[i]);
    printf("\n");
  } else {
    unsigned int a[32];
    if (sscanf(argv[1], "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	       &a[ 0], &a[ 1], &a[ 2], &a[ 3], &a[ 4], &a[ 5], &a[ 6], &a[ 7], &a[ 8], &a[ 9], 
	       &a[10], &a[11], &a[12], &a[13], &a[14], &a[15], &a[16], &a[17], &a[18], &a[19], 
	       &a[20], &a[21], &a[22], &a[23], &a[24], &a[25], &a[26], &a[27], &a[28], &a[29], 
	       &a[30], &a[31]) != 32) {
      printf("NG (arg format)\n");
      exit(-1);
    }
    for(int i=0;i<32;i++) {
      if (h[i] != a[i]) {
	printf("NG\n");
	exit(-1);
      }
    }
    printf("OK\n");
  }

  return 0;
}
