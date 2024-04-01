// nvcc -O3 --maxrregcount 128 --expt-relaxed-constexpr --std c++20 test_tlfloat_cuda.cu crypt.cpp sha2.cpp

#include <iostream>
#include <chrono>
#include <cmath>
#include <cassert>

#include "tlmath.hpp"
#include "auxiliary.hpp"

using namespace std;
using namespace tlfloat;

int cudaMultiProcessorCount = -1;
int N = 0, blockSize = 0;
int nLoop = 1 << 4, nThread = 1 << 8, nBlock = 0;

#define cudaErrorCheck(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true) {
  if (code != cudaSuccess)  {
    fprintf(stderr,"%s(%d) : GPUassert: %s\n", file, line, cudaGetErrorString(code));
    if (abort) exit(code);
  }
}

int64_t getTime() {
  return chrono::duration_cast<chrono::microseconds>
    (chrono::high_resolution_clock::now() - chrono::system_clock::from_time_t(0)).count();
}

template<typename T>
T rnd(shared_ptr<RNG> rng) {
  for(;;) {
    T f;
    rng->nextBytes((unsigned char *)&f, sizeof(f));
    f = frexp(f, nullptr);
    if (finite(f)) return f;
  }
}

void *cudaMallocManaged_(size_t size) {
  void *ptr;
  cudaErrorCheck( cudaMallocManaged(&ptr, size) );
  return ptr;
}

//

template<typename T>
using Func1 = T (*)(const T&);

template<typename T, Func1<T> func>
__global__ void kern(int n, T *r, T *x) {
  int index = threadIdx.x, stride = blockDim.x;
  for (int i = index; i < n; i += stride) r[i] = func(x[i]);
}

template<typename T, Func1<T> func>
void doTest(const char *mes, T *r, T *x) {
  auto t0 = getTime();
  kern<T, func><<<nBlock, nThread>>>(N, r, x);
  cudaErrorCheck( cudaPeekAtLastError() );
  cudaErrorCheck( cudaDeviceSynchronize() );

  auto t1 = getTime();
  for (int i = 0; i < N; i++) {
    if (r[i].m == func(x[i]).m) continue;
    cout << mes << " : NG" << endl;
    cout << "arg1   : " << to_string(x[i], 75) << endl;
    cout << "device : " << to_string(r[i], 75) << endl;
    cout << "host   : " << to_string(func(x[i]), 75) << endl;
    exit(-1);
  }
  auto t2 = getTime();
  cout << mes << " : OK (D:" << (t1 - t0)/1000.0 << "ms, H:" << (t2 - t1)/1000.0 << "ms)" << endl;
}

//

template<typename T>
using Func2 = T (*)(const T&, const T&);

template<typename T, Func2<T> func>
__global__ void kern(int n, T *r, T *x, T *y) {
  int index = threadIdx.x, stride = blockDim.x;
  for (int i = index; i < n; i += stride) r[i] = func(x[i], y[i]);
}

template<typename T, Func2<T> func>
void doTest(const char *mes, T *r, T *x, T *y) {
  auto t0 = getTime();
  kern<T, func><<<nBlock, nThread>>>(N, r, x, y);
  cudaErrorCheck( cudaPeekAtLastError() );
  cudaErrorCheck( cudaDeviceSynchronize() );

  auto t1 = getTime();
  for (int i = 0; i < N; i++) {
    if (r[i].m == func(x[i], y[i]).m) continue;
    cout << mes << " : NG" << endl;
    cout << "arg1   : " << to_string(x[i], 75) << endl;
    cout << "arg2   : " << to_string(y[i], 75) << endl;
    cout << "device : " << to_string(r[i], 75) << endl;
    cout << "host   : " << to_string(func(x[i], y[i]), 75) << endl;
    exit(-1);
  }
  auto t2 = getTime();
  cout << mes << " : OK (D:" << (t1 - t0)/1000.0 << "ms, H:" << (t2 - t1)/1000.0 << "ms)" << endl;
}

//

template<typename T>
using Func3 = T (*)(const T&, const T&, const T&);

template<typename T, Func3<T> func>
__global__ void kern(int n, T *r, T *x, T *y, T *z) {
  int index = threadIdx.x, stride = blockDim.x;
  for (int i = index; i < n; i += stride) r[i] = func(x[i], y[i], z[i]);
}

template<typename T, Func3<T> func>
void doTest(const char *mes, T *r, T *x, T *y, T *z) {
  auto t0 = getTime();
  kern<T, func><<<nBlock, nThread>>>(N, r, x, y, z);
  cudaErrorCheck( cudaPeekAtLastError() );
  cudaErrorCheck( cudaDeviceSynchronize() );

  auto t1 = getTime();
  for (int i = 0; i < N; i++) {
    if (r[i].m == func(x[i], y[i], z[i]).m) continue;
    cout << mes << " : NG" << endl;
    cout << "arg1   : " << to_string(x[i], 75) << endl;
    cout << "arg2   : " << to_string(y[i], 75) << endl;
    cout << "arg3   : " << to_string(z[i], 75) << endl;
    cout << "device : " << to_string(r[i], 75) << endl;
    cout << "host   : " << to_string(func(x[i], y[i], z[i]), 75) << endl;
    exit(-1);
  }
  auto t2 = getTime();
  cout << mes << " : OK (D:" << (t1 - t0)/1000.0 << "ms, H:" << (t2 - t1)/1000.0 << "ms)" << endl;
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

int main(int argc, char **argv) {
  {
    int device;
    cudaErrorCheck(cudaGetDevice(&device));

    cudaDeviceProp devProp;
    cudaErrorCheck(cudaGetDeviceProperties(&devProp, device));

    cudaMultiProcessorCount = devProp.multiProcessorCount;
    nBlock = devProp.multiProcessorCount;
  }

  if (argc >= 2) nThread = atoi(argv[1]);
  if (argc >= 3) nLoop = atoi(argv[2]);

  N = nLoop * nThread * nBlock;

  cout << "N = " << N << ", nLoop = " << nLoop << ", nBlock = " << nBlock << ", nThread = " << nThread << endl;

  auto rng = createPreferredRNG();

  Octuple *xo = (Octuple *)cudaMallocManaged_(N*sizeof(Octuple));
  Octuple *yo = (Octuple *)cudaMallocManaged_(N*sizeof(Octuple));
  Octuple *zo = (Octuple *)cudaMallocManaged_(N*sizeof(Octuple));
  Octuple *ro = (Octuple *)cudaMallocManaged_(N*sizeof(Octuple));

  Quad *xq = (Quad *)cudaMallocManaged_(N*sizeof(Quad));
  Quad *yq = (Quad *)cudaMallocManaged_(N*sizeof(Quad));
  Quad *zq = (Quad *)cudaMallocManaged_(N*sizeof(Quad));
  Quad *rq = (Quad *)cudaMallocManaged_(N*sizeof(Quad));

  Double *xd = (Double *)cudaMallocManaged_(N*sizeof(Double));
  Double *yd = (Double *)cudaMallocManaged_(N*sizeof(Double));
  Double *zd = (Double *)cudaMallocManaged_(N*sizeof(Double));
  Double *rd = (Double *)cudaMallocManaged_(N*sizeof(Double));

  Float *xf = (Float *)cudaMallocManaged_(N*sizeof(Float));
  Float *yf = (Float *)cudaMallocManaged_(N*sizeof(Float));
  Float *zf = (Float *)cudaMallocManaged_(N*sizeof(Float));
  Float *rf = (Float *)cudaMallocManaged_(N*sizeof(Float));

  Half *xh = (Half *)cudaMallocManaged_(N*sizeof(Half));
  Half *yh = (Half *)cudaMallocManaged_(N*sizeof(Half));
  Half *zh = (Half *)cudaMallocManaged_(N*sizeof(Half));
  Half *rh = (Half *)cudaMallocManaged_(N*sizeof(Half));

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

  doTest<Octuple, fabs_>("Octuple fabs", ro, xo);
  doTest<Quad, fabs_>("Quad fabs", rq, xq);
  doTest<Double, fabs_>("Double fabs", rd, xd);
  doTest<Float, fabs_>("Float fabs", rf, xf);
  doTest<Half, fabs_>("Half fabs", rh, xh);

  doTest<Octuple, copysign_>("Octuple copysign", ro, xo, yo);
  doTest<Quad, copysign_>("Quad copysign", rq, xq, yq);
  doTest<Double, copysign_>("Double copysign", rd, xd, yd);
  doTest<Float, copysign_>("Float copysign", rf, xf, yf);
  doTest<Half, copysign_>("Half copysign", rh, xh, yh);

  doTest<Octuple, fmax_>("Octuple fmax", ro, xo, yo);
  doTest<Quad, fmax_>("Quad fmax", rq, xq, yq);
  doTest<Double, fmax_>("Double fmax", rd, xd, yd);
  doTest<Float, fmax_>("Float fmax", rf, xf, yf);
  doTest<Half, fmax_>("Half fmax", rh, xh, yh);

  doTest<Octuple, fmin_>("Octuple fmin", ro, xo, yo);
  doTest<Quad, fmin_>("Quad fmin", rq, xq, yq);
  doTest<Double, fmin_>("Double fmin", rd, xd, yd);
  doTest<Float, fmin_>("Float fmin", rf, xf, yf);
  doTest<Half, fmin_>("Half fmin", rh, xh, yh);

  doTest<Octuple, fdim_>("Octuple fdim", ro, xo, yo);
  doTest<Quad, fdim_>("Quad fdim", rq, xq, yq);
  doTest<Double, fdim_>("Double fdim", rd, xd, yd);
  doTest<Float, fdim_>("Float fdim", rf, xf, yf);
  doTest<Half, fdim_>("Half fdim", rh, xh, yh);

  doTest<Octuple, add_>("Octuple add_", ro, xo, yo);
  doTest<Quad, add_>("Quad add_", rq, xq, yq);
  doTest<Double, add_>("Double add_", rd, xd, yd);
  doTest<Float, add_>("Float add_", rf, xf, yf);
  doTest<Half, add_>("Half add_", rh, xh, yh);

  doTest<Octuple, mul_>("Octuple mul_", ro, xo, yo);
  doTest<Quad, mul_>("Quad mul_", rq, xq, yq);
  doTest<Double, mul_>("Double mul_", rd, xd, yd);
  doTest<Float, mul_>("Float mul_", rf, xf, yf);
  doTest<Half, mul_>("Half mul_", rh, xh, yh);

  doTest<Octuple, div_>("Octuple div_", ro, xo, yo);
  doTest<Quad, div_>("Quad div_", rq, xq, yq);
  doTest<Double, div_>("Double div_", rd, xd, yd);
  doTest<Float, div_>("Float div_", rf, xf, yf);
  doTest<Half, div_>("Half div_", rh, xh, yh);

  doTest<Octuple, fma_>("Octuple fma", ro, xo, yo, zo);
  doTest<Quad, fma_>("Quad fma", rq, xq, yq, zq);
  doTest<Double, fma_>("Double fma", rd, xd, yd, zd);
  doTest<Float, fma_>("Float fma", rf, xf, yf, zf);
  doTest<Half, fma_>("Half fma", rh, xh, yh, zh);

  doTest<Octuple, sqrt_>("Octuple sqrt", ro, xo);
  doTest<Quad, sqrt_>("Quad sqrt", rq, xq);
  doTest<Double, sqrt_>("Double sqrt", rd, xd);
  doTest<Float, sqrt_>("Float sqrt", rf, xf);
  doTest<Half, sqrt_>("Half sqrt", rh, xh);

  doTest<Octuple, hypot_>("Octuple hypot", ro, xo, yo);
  doTest<Quad, hypot_>("Quad hypot", rq, xq, yq);
  doTest<Double, hypot_>("Double hypot", rd, xd, yd);
  doTest<Float, hypot_>("Float hypot", rf, xf, yf);
  doTest<Half, hypot_>("Half hypot", rh, xh, yh);

  doTest<Octuple, trunc_>("Octuple trunc", ro, xo);
  doTest<Quad, trunc_>("Quad trunc", rq, xq);
  doTest<Double, trunc_>("Double trunc", rd, xd);
  doTest<Float, trunc_>("Float trunc", rf, xf);
  doTest<Half, trunc_>("Half trunc", rh, xh);

  doTest<Octuple, floor_>("Octuple floor", ro, xo);
  doTest<Quad, floor_>("Quad floor", rq, xq);
  doTest<Double, floor_>("Double floor", rd, xd);
  doTest<Float, floor_>("Float floor", rf, xf);
  doTest<Half, floor_>("Half floor", rh, xh);

  doTest<Octuple, ceil_>("Octuple ceil", ro, xo);
  doTest<Quad, ceil_>("Quad ceil", rq, xq);
  doTest<Double, ceil_>("Double ceil", rd, xd);
  doTest<Float, ceil_>("Float ceil", rf, xf);
  doTest<Half, ceil_>("Half ceil", rh, xh);

  doTest<Octuple, round_>("Octuple round", ro, xo);
  doTest<Quad, round_>("Quad round", rq, xq);
  doTest<Double, round_>("Double round", rd, xd);
  doTest<Float, round_>("Float round", rf, xf);
  doTest<Half, round_>("Half round", rh, xh);

  doTest<Octuple, rint_>("Octuple rint", ro, xo);
  doTest<Quad, rint_>("Quad rint", rq, xq);
  doTest<Double, rint_>("Double rint", rd, xd);
  doTest<Float, rint_>("Float rint", rf, xf);
  doTest<Half, rint_>("Half rint", rh, xh);

  doTest<Octuple, nextafter_>("Octuple nextafter", ro, xo, yo);
  doTest<Quad, nextafter_>("Quad nextafter", rq, xq, yq);
  doTest<Double, nextafter_>("Double nextafter", rd, xd, yd);
  doTest<Float, nextafter_>("Float nextafter", rf, xf, yf);
  doTest<Half, nextafter_>("Half nextafter", rh, xh, yh);

  doTest<Octuple, sin>("Octuple sin", ro, xo);
  doTest<Quad, sin>("Quad sin", rq, xq);
  doTest<Double, sin>("Double sin", rd, xd);
  doTest<Float, sin>("Float sin", rf, xf);
  doTest<Half, sin>("Half sin", rh, xh);

  doTest<Octuple, cos>("Octuple cos", ro, xo);
  doTest<Quad, cos>("Quad cos", rq, xq);
  doTest<Double, cos>("Double cos", rd, xd);
  doTest<Float, cos>("Float cos", rf, xf);
  doTest<Half, cos>("Half cos", rh, xh);

  doTest<Octuple, tan>("Octuple tan", ro, xo);
  doTest<Quad, tan>("Quad tan", rq, xq);
  doTest<Double, tan>("Double tan", rd, xd);
  doTest<Float, tan>("Float tan", rf, xf);
  doTest<Half, tan>("Half tan", rh, xh);

  doTest<Octuple, atan2>("Octuple atan2", ro, xo, yo);
  doTest<Quad, atan2>("Quad atan2", rq, xq, yq);
  doTest<Double, atan2>("Double atan2", rd, xd, yd);
  doTest<Float, atan2>("Float atan2", rf, xf, yf);
  doTest<Half, atan2>("Half atan2", rh, xh, yh);

  doTest<Octuple, asin>("Octuple asin", ro, xo);
  doTest<Quad, asin>("Quad asin", rq, xq);
  doTest<Double, asin>("Double asin", rd, xd);
  doTest<Float, asin>("Float asin", rf, xf);
  doTest<Half, asin>("Half asin", rh, xh);

  doTest<Octuple, acos>("Octuple acos", ro, xo);
  doTest<Quad, acos>("Quad acos", rq, xq);
  doTest<Double, acos>("Double acos", rd, xd);
  doTest<Float, acos>("Float acos", rf, xf);
  doTest<Half, acos>("Half acos", rh, xh);

  doTest<Octuple, atan>("Octuple atan", ro, xo);
  doTest<Quad, atan>("Quad atan", rq, xq);
  doTest<Double, atan>("Double atan", rd, xd);
  doTest<Float, atan>("Float atan", rf, xf);
  doTest<Half, atan>("Half atan", rh, xh);

  doTest<Octuple, log>("Octuple log", ro, xo);
  doTest<Quad, log>("Quad log", rq, xq);
  doTest<Double, log>("Double log", rd, xd);
  doTest<Float, log>("Float log", rf, xf);
  doTest<Half, log>("Half log", rh, xh);

  doTest<Octuple, log2>("Octuple log2", ro, xo);
  doTest<Quad, log2>("Quad log2", rq, xq);
  doTest<Double, log2>("Double log2", rd, xd);
  doTest<Float, log2>("Float log2", rf, xf);
  doTest<Half, log2>("Half log2", rh, xh);

  doTest<Octuple, log10>("Octuple log10", ro, xo);
  doTest<Quad, log10>("Quad log10", rq, xq);
  doTest<Double, log10>("Double log10", rd, xd);
  doTest<Float, log10>("Float log10", rf, xf);
  doTest<Half, log10>("Half log10", rh, xh);

  doTest<Octuple, log1p>("Octuple log1p", ro, xo);
  doTest<Quad, log1p>("Quad log1p", rq, xq);
  doTest<Double, log1p>("Double log1p", rd, xd);
  doTest<Float, log1p>("Float log1p", rf, xf);
  doTest<Half, log1p>("Half log1p", rh, xh);

  doTest<Octuple, exp>("Octuple exp", ro, xo);
  doTest<Quad, exp>("Quad exp", rq, xq);
  doTest<Double, exp>("Double exp", rd, xd);
  doTest<Float, exp>("Float exp", rf, xf);
  doTest<Half, exp>("Half exp", rh, xh);

  doTest<Octuple, exp2>("Octuple exp2", ro, xo);
  doTest<Quad, exp2>("Quad exp2", rq, xq);
  doTest<Double, exp2>("Double exp2", rd, xd);
  doTest<Float, exp2>("Float exp2", rf, xf);
  doTest<Half, exp2>("Half exp2", rh, xh);

  doTest<Octuple, exp10>("Octuple exp10", ro, xo);
  doTest<Quad, exp10>("Quad exp10", rq, xq);
  doTest<Double, exp10>("Double exp10", rd, xd);
  doTest<Float, exp10>("Float exp10", rf, xf);
  doTest<Half, exp10>("Half exp10", rh, xh);

  doTest<Octuple, expm1>("Octuple expm1", ro, xo);
  doTest<Quad, expm1>("Quad expm1", rq, xq);
  doTest<Double, expm1>("Double expm1", rd, xd);
  doTest<Float, expm1>("Float expm1", rf, xf);
  doTest<Half, expm1>("Half expm1", rh, xh);

  doTest<Octuple, pow>("Octuple pow", ro, xo, yo);
  doTest<Quad, pow>("Quad pow", rq, xq, yq);
  doTest<Double, pow>("Double pow", rd, xd, yd);
  doTest<Float, pow>("Float pow", rf, xf, yf);
  doTest<Half, pow>("Half pow", rh, xh, yh);

  doTest<Octuple, cbrt>("Octuple cbrt", ro, xo);
  doTest<Quad, cbrt>("Quad cbrt", rq, xq);
  doTest<Double, cbrt>("Double cbrt", rd, xd);
  doTest<Float, cbrt>("Float cbrt", rf, xf);
  doTest<Half, cbrt>("Half cbrt", rh, xh);

  doTest<Octuple, sinh>("Octuple sinh", ro, xo);
  doTest<Quad, sinh>("Quad sinh", rq, xq);
  doTest<Double, sinh>("Double sinh", rd, xd);
  doTest<Float, sinh>("Float sinh", rf, xf);
  doTest<Half, sinh>("Half sinh", rh, xh);

  doTest<Octuple, cosh>("Octuple cosh", ro, xo);
  doTest<Quad, cosh>("Quad cosh", rq, xq);
  doTest<Double, cosh>("Double cosh", rd, xd);
  doTest<Float, cosh>("Float cosh", rf, xf);
  doTest<Half, cosh>("Half cosh", rh, xh);

  doTest<Octuple, tanh>("Octuple tanh", ro, xo);
  doTest<Quad, tanh>("Quad tanh", rq, xq);
  doTest<Double, tanh>("Double tanh", rd, xd);
  doTest<Float, tanh>("Float tanh", rf, xf);
  doTest<Half, tanh>("Half tanh", rh, xh);

  doTest<Octuple, asinh>("Octuple asinh", ro, xo);
  doTest<Quad, asinh>("Quad asinh", rq, xq);
  doTest<Double, asinh>("Double asinh", rd, xd);
  doTest<Float, asinh>("Float asinh", rf, xf);
  doTest<Half, asinh>("Half asinh", rh, xh);

  doTest<Octuple, acosh>("Octuple acosh", ro, xo);
  doTest<Quad, acosh>("Quad acosh", rq, xq);
  doTest<Double, acosh>("Double acosh", rd, xd);
  doTest<Float, acosh>("Float acosh", rf, xf);
  doTest<Half, acosh>("Half acosh", rh, xh);

  doTest<Octuple, atanh>("Octuple atanh", ro, xo);
  doTest<Quad, atanh>("Quad atanh", rq, xq);
  doTest<Double, atanh>("Double atanh", rd, xd);
  doTest<Float, atanh>("Float atanh", rf, xf);
  doTest<Half, atanh>("Half atanh", rh, xh);

  doTest<Octuple, fmod>("Octuple fmod", ro, xo, yo);
  doTest<Quad, fmod>("Quad fmod", rq, xq, yq);
  doTest<Double, fmod>("Double fmod", rd, xd, yd);
  doTest<Float, fmod>("Float fmod", rf, xf, yf);
  doTest<Half, fmod>("Half fmod", rh, xh, yh);

  doTest<Octuple, remainder>("Octuple remainder", ro, xo, yo);
  doTest<Quad, remainder>("Quad remainder", rq, xq, yq);
  doTest<Double, remainder>("Double remainder", rd, xd, yd);
  doTest<Float, remainder>("Float remainder", rf, xf, yf);
  doTest<Half, remainder>("Half remainder", rh, xh, yh);

  cudaFree(rh);
  cudaFree(zh);
  cudaFree(yh);
  cudaFree(xh);

  cudaFree(rf);
  cudaFree(zf);
  cudaFree(yf);
  cudaFree(xf);

  cudaFree(rd);
  cudaFree(zd);
  cudaFree(yd);
  cudaFree(xd);

  cudaFree(rq);
  cudaFree(zq);
  cudaFree(yq);
  cudaFree(xq);

  cudaFree(ro);
  cudaFree(zo);
  cudaFree(yo);
  cudaFree(xo);

  return 0;
}
