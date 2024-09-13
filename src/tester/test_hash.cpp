#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "tlfloat/tlmath.hpp"
#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;

int N = 10000;

auto sha256 = PSHA2_256_Internal();

template<typename T>
T rnd(shared_ptr<RNG> rng) {
  for(;;) {
    T f;
    rng->nextBytesW((unsigned char *)&f, sizeof(f));
    if (finite(f)) return f;
  }
}

//

template<typename T>
using Func1 = T (*)(const T&);

template<typename T, Func1<T> func>
void computeHash(const char *mes, T *r, T *x, FILE *fp=nullptr, int mode=0) {
  for (int i = 0; i < N; i++) {
    T f = func(x[i]);
    if (mode == 0) {
      sha256.appendWord((void *)&f.m, sizeof(f.m));
    } else if (mode == 1) {
      fwrite_((void *)&x[i].m, sizeof(x[i].m), 1, fp);
      fwrite_((void *)&f.m, sizeof(f.m), 1, fp);
    } else if (mode == 2) {
      T t = 0;
      if (fread((void *)&t, sizeof(t), 1, fp) != 1 || t.m != x[i].m) {
	cout << mes << " : arg does not match" << endl;
	cout << "arg file : " << toHexString(t.m) << " : " << to_string(t, 75) << endl;
	cout << "arg host : " << toHexString(x[i].m) << " : " << to_string(x[i], 75) << endl;
      }
      if (fread((void *)&t, sizeof(t), 1, fp) != 1 || t.m != f.m) {
	cout << mes << endl;
	cout << "arg1   : " << to_string(x[i], 75) << endl;
	cout << "file   : " << toHexString(t.m) << " : " << to_string(t, 75) << endl;
	cout << "host   : " << toHexString(f.m) << " : " << to_string(f, 75) << endl;
      }
    }
  }
}

template<typename T>
using Func2 = T (*)(const T&, const T&);

template<typename T, Func2<T> func>
void computeHash(const char *mes, T *r, T *x, T *y, FILE *fp=nullptr, int mode=0) {
  for (int i = 0; i < N; i++) {
    T f = func(x[i], y[i]);
    if (mode == 0) {
      sha256.appendWord((void *)&f.m, sizeof(f.m));
    } else if (mode == 1) {
      fwrite_((void *)&f.m, sizeof(f.m), 1, fp);
    } else if (mode == 2) {
      T t = 0;
      if (fread((void *)&t, sizeof(t), 1, fp) != 1 || t.m != f.m) {
	cout << mes << endl;
	cout << "arg1   : " << to_string(x[i], 75) << endl;
	cout << "arg2   : " << to_string(y[i], 75) << endl;
	cout << "file   : " << toHexString(t.m) << " : " << to_string(t, 75) << endl;
	cout << "host   : " << toHexString(f.m) << " : " << to_string(f, 75) << endl;
      }
    }
  }
}

template<typename T>
using Func2i = xpair<T, int64_t> (*)(const T&, const T&);

template<typename T, Func2i<T> func>
void computeHash(const char *mes, T *r, T *x, T *y, FILE *fp=nullptr, int mode=0) {
  for (int i = 0; i < N; i++) {
    xpair<T, int64_t> p = func(x[i], y[i]);
    if (mode == 0) {
      sha256.appendWord((void *)&p.first.m, sizeof(p.first.m));
      sha256.appendWord((void *)&p.second, sizeof(p.second));
    } else if (mode == 1) {
      fwrite_((void *)&p.first.m, sizeof(p.first.m), 1, fp);
      fwrite_((void *)&p.second, sizeof(p.second), 1, fp);
    } else if (mode == 2) {
      T t = 0;
      size_t s0 = fread((void *)&t, sizeof(t), 1, fp);
      int64_t q = 0;
      size_t s1 = fread((void *)&q, sizeof(q), 1, fp);
      if (s0 != 1 || s1 != 1 || t.m != p.first.m || q != p.second) {
	cout << mes << endl;
	cout << "arg1   : " << to_string(x[i], 75) << endl;
	cout << "arg2   : " << to_string(y[i], 75) << endl;
	cout << "file   : " << toHexString(t.m) << " : " << to_string(t, 75) << ", " << q << endl;
	cout << "host   : " << toHexString(p.first.m) << " : " << to_string(p.first, 75) << ", " << p.second << endl;
      }
    }
  }
}

template<typename T>
using Func3 = T (*)(const T&, const T&, const T&);

template<typename T, Func3<T> func>
void computeHash(const char *mes, T *r, T *x, T *y, T *z, FILE *fp=nullptr, int mode=0) {
  for (int i = 0; i < N; i++) {
    T f = func(x[i], y[i], z[i]);
    if (mode == 0) {
      sha256.appendWord((void *)&f.m, sizeof(f.m));
    } else if (mode == 1) {
      fwrite_((void *)&f.m, sizeof(f.m), 1, fp);
    } else if (mode == 2) {
      T t = 0;
      if (fread((void *)&t, sizeof(t), 1, fp) != 1 || t.m != f.m) {
	cout << mes << endl;
	cout << "arg1   : " << to_string(x[i], 75) << endl;
	cout << "arg2   : " << to_string(y[i], 75) << endl;
	cout << "arg3   : " << to_string(z[i], 75) << endl;
	cout << "file   : " << toHexString(t.m) << " : " << to_string(t, 75) << endl;
	cout << "host   : " << toHexString(f.m) << " : " << to_string(f, 75) << endl;
      }
    }
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
template<typename T> static constexpr T lgamma_(const T &a1) { return lgamma(a1, nullptr); }

//

int main(int argc, char **argv) {
  FILE *fp = nullptr;
  int mode = 0;
  if (argc == 3 && strcmp(argv[1], "r") == 0) {
    fp = fopen(argv[2], "rb");
    if (!fp) {
      cerr << "Could not open " << argv[2] << endl;
      exit(-1);
    }
    mode = 2;
  } else if (argc == 3 && strcmp(argv[1], "w") == 0) {
    fp = fopen(argv[2], "wb");
    if (!fp) {
      cerr << "Could not open " << argv[2] << endl;
      exit(-1);
    }
    mode = 1;
  }

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

  computeHash<Octuple, fabs_>("Octuple fabs", ro, xo, fp, mode);
  computeHash<Quad, fabs_>("Quad fabs", rq, xq, fp, mode);
  computeHash<Double, fabs_>("Double fabs", rd, xd, fp, mode);
  computeHash<Float, fabs_>("Float fabs", rf, xf, fp, mode);
  computeHash<Half, fabs_>("Half fabs", rh, xh, fp, mode);

  computeHash<Octuple, copysign_>("Octuple copysign", ro, xo, yo, fp, mode);
  computeHash<Quad, copysign_>("Quad copysign", rq, xq, yq, fp, mode);
  computeHash<Double, copysign_>("Double copysign", rd, xd, yd, fp, mode);
  computeHash<Float, copysign_>("Float copysign", rf, xf, yf, fp, mode);
  computeHash<Half, copysign_>("Half copysign", rh, xh, yh, fp, mode);

  computeHash<Octuple, fmax_>("Octuple fmax", ro, xo, yo, fp, mode);
  computeHash<Quad, fmax_>("Quad fmax", rq, xq, yq, fp, mode);
  computeHash<Double, fmax_>("Double fmax", rd, xd, yd, fp, mode);
  computeHash<Float, fmax_>("Float fmax", rf, xf, yf, fp, mode);
  computeHash<Half, fmax_>("Half fmax", rh, xh, yh, fp, mode);

  computeHash<Octuple, fmin_>("Octuple fmin", ro, xo, yo, fp, mode);
  computeHash<Quad, fmin_>("Quad fmin", rq, xq, yq, fp, mode);
  computeHash<Double, fmin_>("Double fmin", rd, xd, yd, fp, mode);
  computeHash<Float, fmin_>("Float fmin", rf, xf, yf, fp, mode);
  computeHash<Half, fmin_>("Half fmin", rh, xh, yh, fp, mode);

  computeHash<Octuple, fdim_>("Octuple fdim", ro, xo, yo, fp, mode);
  computeHash<Quad, fdim_>("Quad fdim", rq, xq, yq, fp, mode);
  computeHash<Double, fdim_>("Double fdim", rd, xd, yd, fp, mode);
  computeHash<Float, fdim_>("Float fdim", rf, xf, yf, fp, mode);
  computeHash<Half, fdim_>("Half fdim", rh, xh, yh, fp, mode);

  computeHash<Octuple, add_>("Octuple add_", ro, xo, yo, fp, mode);
  computeHash<Quad, add_>("Quad add_", rq, xq, yq, fp, mode);
  computeHash<Double, add_>("Double add_", rd, xd, yd, fp, mode);
  computeHash<Float, add_>("Float add_", rf, xf, yf, fp, mode);
  computeHash<Half, add_>("Half add_", rh, xh, yh, fp, mode);

  computeHash<Octuple, mul_>("Octuple mul_", ro, xo, yo, fp, mode);
  computeHash<Quad, mul_>("Quad mul_", rq, xq, yq, fp, mode);
  computeHash<Double, mul_>("Double mul_", rd, xd, yd, fp, mode);
  computeHash<Float, mul_>("Float mul_", rf, xf, yf, fp, mode);
  computeHash<Half, mul_>("Half mul_", rh, xh, yh, fp, mode);

  computeHash<Octuple, div_>("Octuple div_", ro, xo, yo, fp, mode);
  computeHash<Quad, div_>("Quad div_", rq, xq, yq, fp, mode);
  computeHash<Double, div_>("Double div_", rd, xd, yd, fp, mode);
  computeHash<Float, div_>("Float div_", rf, xf, yf, fp, mode);
  computeHash<Half, div_>("Half div_", rh, xh, yh, fp, mode);

  computeHash<Octuple, fma_>("Octuple fma", ro, xo, yo, zo, fp, mode);
  computeHash<Quad, fma_>("Quad fma", rq, xq, yq, zq, fp, mode);
  computeHash<Double, fma_>("Double fma", rd, xd, yd, zd, fp, mode);
  computeHash<Float, fma_>("Float fma", rf, xf, yf, zf, fp, mode);
  computeHash<Half, fma_>("Half fma", rh, xh, yh, zh, fp, mode);

  computeHash<Octuple, sqrt_>("Octuple sqrt", ro, xo, fp, mode);
  computeHash<Quad, sqrt_>("Quad sqrt", rq, xq, fp, mode);
  computeHash<Double, sqrt_>("Double sqrt", rd, xd, fp, mode);
  computeHash<Float, sqrt_>("Float sqrt", rf, xf, fp, mode);
  computeHash<Half, sqrt_>("Half sqrt", rh, xh, fp, mode);

  computeHash<Octuple, hypot_>("Octuple hypot", ro, xo, yo, fp, mode);
  computeHash<Quad, hypot_>("Quad hypot", rq, xq, yq, fp, mode);
  computeHash<Double, hypot_>("Double hypot", rd, xd, yd, fp, mode);
  computeHash<Float, hypot_>("Float hypot", rf, xf, yf, fp, mode);
  computeHash<Half, hypot_>("Half hypot", rh, xh, yh, fp, mode);

  computeHash<Octuple, trunc_>("Octuple trunc", ro, xo, fp, mode);
  computeHash<Quad, trunc_>("Quad trunc", rq, xq, fp, mode);
  computeHash<Double, trunc_>("Double trunc", rd, xd, fp, mode);
  computeHash<Float, trunc_>("Float trunc", rf, xf, fp, mode);
  computeHash<Half, trunc_>("Half trunc", rh, xh, fp, mode);

  computeHash<Octuple, floor_>("Octuple floor", ro, xo, fp, mode);
  computeHash<Quad, floor_>("Quad floor", rq, xq, fp, mode);
  computeHash<Double, floor_>("Double floor", rd, xd, fp, mode);
  computeHash<Float, floor_>("Float floor", rf, xf, fp, mode);
  computeHash<Half, floor_>("Half floor", rh, xh, fp, mode);

  computeHash<Octuple, ceil_>("Octuple ceil", ro, xo, fp, mode);
  computeHash<Quad, ceil_>("Quad ceil", rq, xq, fp, mode);
  computeHash<Double, ceil_>("Double ceil", rd, xd, fp, mode);
  computeHash<Float, ceil_>("Float ceil", rf, xf, fp, mode);
  computeHash<Half, ceil_>("Half ceil", rh, xh, fp, mode);

  computeHash<Octuple, round_>("Octuple round", ro, xo, fp, mode);
  computeHash<Quad, round_>("Quad round", rq, xq, fp, mode);
  computeHash<Double, round_>("Double round", rd, xd, fp, mode);
  computeHash<Float, round_>("Float round", rf, xf, fp, mode);
  computeHash<Half, round_>("Half round", rh, xh, fp, mode);

  computeHash<Octuple, rint_>("Octuple rint", ro, xo, fp, mode);
  computeHash<Quad, rint_>("Quad rint", rq, xq, fp, mode);
  computeHash<Double, rint_>("Double rint", rd, xd, fp, mode);
  computeHash<Float, rint_>("Float rint", rf, xf, fp, mode);
  computeHash<Half, rint_>("Half rint", rh, xh, fp, mode);

  computeHash<Octuple, nextafter_>("Octuple nextafter", ro, xo, yo, fp, mode);
  computeHash<Quad, nextafter_>("Quad nextafter", rq, xq, yq, fp, mode);
  computeHash<Double, nextafter_>("Double nextafter", rd, xd, yd, fp, mode);
  computeHash<Float, nextafter_>("Float nextafter", rf, xf, yf, fp, mode);
  computeHash<Half, nextafter_>("Half nextafter", rh, xh, yh, fp, mode);

  computeHash<Octuple, sin>("Octuple sin", ro, xo, fp, mode);
  computeHash<Quad, sin>("Quad sin", rq, xq, fp, mode);
  computeHash<Double, sin>("Double sin", rd, xd, fp, mode);
  computeHash<Float, sin>("Float sin", rf, xf, fp, mode);
  computeHash<Half, sin>("Half sin", rh, xh, fp, mode);

  computeHash<Octuple, cos>("Octuple cos", ro, xo, fp, mode);
  computeHash<Quad, cos>("Quad cos", rq, xq, fp, mode);
  computeHash<Double, cos>("Double cos", rd, xd, fp, mode);
  computeHash<Float, cos>("Float cos", rf, xf, fp, mode);
  computeHash<Half, cos>("Half cos", rh, xh, fp, mode);

  computeHash<Octuple, tan>("Octuple tan", ro, xo, fp, mode);
  computeHash<Quad, tan>("Quad tan", rq, xq, fp, mode);
  computeHash<Double, tan>("Double tan", rd, xd, fp, mode);
  computeHash<Float, tan>("Float tan", rf, xf, fp, mode);
  computeHash<Half, tan>("Half tan", rh, xh, fp, mode);

  computeHash<Octuple, sinpi>("Octuple sinpi", ro, xo, fp, mode);
  computeHash<Quad, sinpi>("Quad sinpi", rq, xq, fp, mode);
  computeHash<Double, sinpi>("Double sinpi", rd, xd, fp, mode);
  computeHash<Float, sinpi>("Float sinpi", rf, xf, fp, mode);
  computeHash<Half, sinpi>("Half sinpi", rh, xh, fp, mode);

  computeHash<Octuple, cospi>("Octuple cospi", ro, xo, fp, mode);
  computeHash<Quad, cospi>("Quad cospi", rq, xq, fp, mode);
  computeHash<Double, cospi>("Double cospi", rd, xd, fp, mode);
  computeHash<Float, cospi>("Float cospi", rf, xf, fp, mode);
  computeHash<Half, cospi>("Half cospi", rh, xh, fp, mode);

  computeHash<Octuple, tanpi>("Octuple tanpi", ro, xo, fp, mode);
  computeHash<Quad, tanpi>("Quad tanpi", rq, xq, fp, mode);
  computeHash<Double, tanpi>("Double tanpi", rd, xd, fp, mode);
  computeHash<Float, tanpi>("Float tanpi", rf, xf, fp, mode);
  computeHash<Half, tanpi>("Half tanpi", rh, xh, fp, mode);

  computeHash<Octuple, atan2>("Octuple atan2", ro, xo, yo, fp, mode);
  computeHash<Quad, atan2>("Quad atan2", rq, xq, yq, fp, mode);
  computeHash<Double, atan2>("Double atan2", rd, xd, yd, fp, mode);
  computeHash<Float, atan2>("Float atan2", rf, xf, yf, fp, mode);
  computeHash<Half, atan2>("Half atan2", rh, xh, yh, fp, mode);

  computeHash<Octuple, asin>("Octuple asin", ro, xo, fp, mode);
  computeHash<Quad, asin>("Quad asin", rq, xq, fp, mode);
  computeHash<Double, asin>("Double asin", rd, xd, fp, mode);
  computeHash<Float, asin>("Float asin", rf, xf, fp, mode);
  computeHash<Half, asin>("Half asin", rh, xh, fp, mode);

  computeHash<Octuple, acos>("Octuple acos", ro, xo, fp, mode);
  computeHash<Quad, acos>("Quad acos", rq, xq, fp, mode);
  computeHash<Double, acos>("Double acos", rd, xd, fp, mode);
  computeHash<Float, acos>("Float acos", rf, xf, fp, mode);
  computeHash<Half, acos>("Half acos", rh, xh, fp, mode);

  computeHash<Octuple, atan>("Octuple atan", ro, xo, fp, mode);
  computeHash<Quad, atan>("Quad atan", rq, xq, fp, mode);
  computeHash<Double, atan>("Double atan", rd, xd, fp, mode);
  computeHash<Float, atan>("Float atan", rf, xf, fp, mode);
  computeHash<Half, atan>("Half atan", rh, xh, fp, mode);

  computeHash<Octuple, log>("Octuple log", ro, xo, fp, mode);
  computeHash<Quad, log>("Quad log", rq, xq, fp, mode);
  computeHash<Double, log>("Double log", rd, xd, fp, mode);
  computeHash<Float, log>("Float log", rf, xf, fp, mode);
  computeHash<Half, log>("Half log", rh, xh, fp, mode);

  computeHash<Octuple, log2>("Octuple log2", ro, xo, fp, mode);
  computeHash<Quad, log2>("Quad log2", rq, xq, fp, mode);
  computeHash<Double, log2>("Double log2", rd, xd, fp, mode);
  computeHash<Float, log2>("Float log2", rf, xf, fp, mode);
  computeHash<Half, log2>("Half log2", rh, xh, fp, mode);

  computeHash<Octuple, log10>("Octuple log10", ro, xo, fp, mode);
  computeHash<Quad, log10>("Quad log10", rq, xq, fp, mode);
  computeHash<Double, log10>("Double log10", rd, xd, fp, mode);
  computeHash<Float, log10>("Float log10", rf, xf, fp, mode);
  computeHash<Half, log10>("Half log10", rh, xh, fp, mode);

  computeHash<Octuple, log1p>("Octuple log1p", ro, xo, fp, mode);
  computeHash<Quad, log1p>("Quad log1p", rq, xq, fp, mode);
  computeHash<Double, log1p>("Double log1p", rd, xd, fp, mode);
  computeHash<Float, log1p>("Float log1p", rf, xf, fp, mode);
  computeHash<Half, log1p>("Half log1p", rh, xh, fp, mode);

  computeHash<Octuple, exp>("Octuple exp", ro, xo, fp, mode);
  computeHash<Quad, exp>("Quad exp", rq, xq, fp, mode);
  computeHash<Double, exp>("Double exp", rd, xd, fp, mode);
  computeHash<Float, exp>("Float exp", rf, xf, fp, mode);
  computeHash<Half, exp>("Half exp", rh, xh, fp, mode);

  computeHash<Octuple, exp2>("Octuple exp2", ro, xo, fp, mode);
  computeHash<Quad, exp2>("Quad exp2", rq, xq, fp, mode);
  computeHash<Double, exp2>("Double exp2", rd, xd, fp, mode);
  computeHash<Float, exp2>("Float exp2", rf, xf, fp, mode);
  computeHash<Half, exp2>("Half exp2", rh, xh, fp, mode);

  computeHash<Octuple, exp10>("Octuple exp10", ro, xo, fp, mode);
  computeHash<Quad, exp10>("Quad exp10", rq, xq, fp, mode);
  computeHash<Double, exp10>("Double exp10", rd, xd, fp, mode);
  computeHash<Float, exp10>("Float exp10", rf, xf, fp, mode);
  computeHash<Half, exp10>("Half exp10", rh, xh, fp, mode);

  computeHash<Octuple, expm1>("Octuple expm1", ro, xo, fp, mode);
  computeHash<Quad, expm1>("Quad expm1", rq, xq, fp, mode);
  computeHash<Double, expm1>("Double expm1", rd, xd, fp, mode);
  computeHash<Float, expm1>("Float expm1", rf, xf, fp, mode);
  computeHash<Half, expm1>("Half expm1", rh, xh, fp, mode);

  computeHash<Octuple, pow>("Octuple pow", ro, xo, yo, fp, mode);
  computeHash<Quad, pow>("Quad pow", rq, xq, yq, fp, mode);
  computeHash<Double, pow>("Double pow", rd, xd, yd, fp, mode);
  computeHash<Float, pow>("Float pow", rf, xf, yf, fp, mode);
  computeHash<Half, pow>("Half pow", rh, xh, yh, fp, mode);

  computeHash<Octuple, cbrt>("Octuple cbrt", ro, xo, fp, mode);
  computeHash<Quad, cbrt>("Quad cbrt", rq, xq, fp, mode);
  computeHash<Double, cbrt>("Double cbrt", rd, xd, fp, mode);
  computeHash<Float, cbrt>("Float cbrt", rf, xf, fp, mode);
  computeHash<Half, cbrt>("Half cbrt", rh, xh, fp, mode);

  computeHash<Octuple, sinh>("Octuple sinh", ro, xo, fp, mode);
  computeHash<Quad, sinh>("Quad sinh", rq, xq, fp, mode);
  computeHash<Double, sinh>("Double sinh", rd, xd, fp, mode);
  computeHash<Float, sinh>("Float sinh", rf, xf, fp, mode);
  computeHash<Half, sinh>("Half sinh", rh, xh, fp, mode);

  computeHash<Octuple, cosh>("Octuple cosh", ro, xo, fp, mode);
  computeHash<Quad, cosh>("Quad cosh", rq, xq, fp, mode);
  computeHash<Double, cosh>("Double cosh", rd, xd, fp, mode);
  computeHash<Float, cosh>("Float cosh", rf, xf, fp, mode);
  computeHash<Half, cosh>("Half cosh", rh, xh, fp, mode);

  computeHash<Octuple, tanh>("Octuple tanh", ro, xo, fp, mode);
  computeHash<Quad, tanh>("Quad tanh", rq, xq, fp, mode);
  computeHash<Double, tanh>("Double tanh", rd, xd, fp, mode);
  computeHash<Float, tanh>("Float tanh", rf, xf, fp, mode);
  computeHash<Half, tanh>("Half tanh", rh, xh, fp, mode);

  computeHash<Octuple, asinh>("Octuple asinh", ro, xo, fp, mode);
  computeHash<Quad, asinh>("Quad asinh", rq, xq, fp, mode);
  computeHash<Double, asinh>("Double asinh", rd, xd, fp, mode);
  computeHash<Float, asinh>("Float asinh", rf, xf, fp, mode);
  computeHash<Half, asinh>("Half asinh", rh, xh, fp, mode);

  computeHash<Octuple, acosh>("Octuple acosh", ro, xo, fp, mode);
  computeHash<Quad, acosh>("Quad acosh", rq, xq, fp, mode);
  computeHash<Double, acosh>("Double acosh", rd, xd, fp, mode);
  computeHash<Float, acosh>("Float acosh", rf, xf, fp, mode);
  computeHash<Half, acosh>("Half acosh", rh, xh, fp, mode);

  computeHash<Octuple, atanh>("Octuple atanh", ro, xo, fp, mode);
  computeHash<Quad, atanh>("Quad atanh", rq, xq, fp, mode);
  computeHash<Double, atanh>("Double atanh", rd, xd, fp, mode);
  computeHash<Float, atanh>("Float atanh", rf, xf, fp, mode);
  computeHash<Half, atanh>("Half atanh", rh, xh, fp, mode);

  computeHash<Octuple, erf>("Octuple erf", ro, xo, fp, mode);
  computeHash<Quad, erf>("Quad erf", rq, xq, fp, mode);
  computeHash<Double, erf>("Double erf", rd, xd, fp, mode);
  computeHash<Float, erf>("Float erf", rf, xf, fp, mode);
  computeHash<Half, erf>("Half erf", rh, xh, fp, mode);

  computeHash<Octuple, erfc>("Octuple erfc", ro, xo, fp, mode);
  computeHash<Quad, erfc>("Quad erfc", rq, xq, fp, mode);
  computeHash<Double, erfc>("Double erfc", rd, xd, fp, mode);
  computeHash<Float, erfc>("Float erfc", rf, xf, fp, mode);
  computeHash<Half, erfc>("Half erfc", rh, xh, fp, mode);

  computeHash<Octuple, tgamma>("Octuple tgamma", ro, xo, fp, mode);
  computeHash<Quad, tgamma>("Quad tgamma", rq, xq, fp, mode);
  computeHash<Double, tgamma>("Double tgamma", rd, xd, fp, mode);
  computeHash<Float, tgamma>("Float tgamma", rf, xf, fp, mode);
  computeHash<Half, tgamma>("Half tgamma", rh, xh, fp, mode);

  computeHash<Octuple, lgamma_>("Octuple lgamma", ro, xo, fp, mode);
  computeHash<Quad, lgamma_>("Quad lgamma", rq, xq, fp, mode);
  computeHash<Double, lgamma_>("Double lgamma", rd, xd, fp, mode);
  computeHash<Float, lgamma_>("Float lgamma", rf, xf, fp, mode);
  computeHash<Half, lgamma_>("Half lgamma", rh, xh, fp, mode);

  computeHash<Octuple, fmod>("Octuple fmod", ro, xo, yo, fp, mode);
  computeHash<Quad, fmod>("Quad fmod", rq, xq, yq, fp, mode);
  computeHash<Double, fmod>("Double fmod", rd, xd, yd, fp, mode);
  computeHash<Float, fmod>("Float fmod", rf, xf, yf, fp, mode);
  computeHash<Half, fmod>("Half fmod", rh, xh, yh, fp, mode);

  computeHash<Octuple, remainder>("Octuple remainder", ro, xo, yo, fp, mode);
  computeHash<Quad, remainder>("Quad remainder", rq, xq, yq, fp, mode);
  computeHash<Double, remainder>("Double remainder", rd, xd, yd, fp, mode);
  computeHash<Float, remainder>("Float remainder", rf, xf, yf, fp, mode);
  computeHash<Half, remainder>("Half remainder", rh, xh, yh, fp, mode);

  computeHash<Octuple, remquo>("Octuple remquo", ro, xo, yo, fp, mode);
  computeHash<Quad, remquo>("Quad remquo", rq, xq, yq, fp, mode);
  computeHash<Double, remquo>("Double remquo", rd, xd, yd, fp, mode);
  computeHash<Float, remquo>("Float remquo", rf, xf, yf, fp, mode);
  computeHash<Half, remquo>("Half remquo", rh, xh, yh, fp, mode);

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

  if (mode == 0) {
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
  }

  if (fp) fclose(fp);

  return 0;
}
