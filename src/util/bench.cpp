#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <cstring>
#include <tlfloat/tlfloatconfig.hpp>

using namespace std;

#if defined(CONFIG_TLFLOAT_QUAD)

#include <tlfloat/tlmath.hpp>
#define CONFIG "tlfloat quad"
using namespace tlfloat;
typedef Quad real;

#define FMA	fma
#define SQRT	sqrt
#define RINT    rint
#define SIN	sin
#define ATAN	atan
#define EXP	exp
#define LOG	log
#define POW	pow

#elif defined(CONFIG_TLFLOAT_OCTUPLE)

#include <tlfloat/tlmath.hpp>
#define CONFIG "tlfloat octuple"
using namespace tlfloat;
typedef Octuple real;

#define FMA	fma
#define SQRT	sqrt
#define RINT    rint
#define SIN	sin
#define ATAN	atan
#define EXP	exp
#define LOG	log
#define POW	pow

#elif defined(CONFIG_TLFLOAT_QUAD_CAPI)

#include <tlfloat/tlfloat.h>
#define CONFIG "tlfloat quad C API"
typedef tlfloat_quad real;

#define FMA	tlfloat_fmaq
#define SQRT	tlfloat_sqrtq
#define RINT    tlfloat_rintq
#define SIN	tlfloat_sinq
#define ATAN	tlfloat_atanq
#define EXP	tlfloat_expq
#define LOG	tlfloat_logq
#define POW	tlfloat_powq

#elif defined(CONFIG_TLFLOAT_OCTUPLE_CAPI)

#include <tlfloat/tlfloat.h>
#define CONFIG "tlfloat octuple C API"
typedef tlfloat_octuple real;

#define FMA	tlfloat_fmao
#define SQRT	tlfloat_sqrto
#define RINT    tlfloat_rinto
#define SIN	tlfloat_sino
#define ATAN	tlfloat_atano
#define EXP	tlfloat_expo
#define LOG	tlfloat_logo
#define POW	tlfloat_powo

#elif defined(CONFIG_LIBQUADMATH)

#define CONFIG "Libquadmath"
#include <quadmath.h>
typedef __float128 real;

#define FMA	fmaq
#define SQRT	sqrtq
#define RINT    rintq
#define SIN	sinq
#define ATAN	atanq
#define EXP	expq
#define LOG	logq
#define POW	powq

#elif defined(CONFIG_LONGDOUBLE)

#define CONFIG "Native long double"
#include <math.h>
typedef long double real;

#define FMA	fmal
#define SQRT	sqrtl
#define RINT    rintl
#define SIN	sinl
#define ATAN	atanl
#define EXP	expl
#define LOG	logl
#define POW	powl

#else

#define CONFIG "Native double"
#include <math.h>
typedef double real;

#define FMA	fma
#define SQRT	sqrt
#define RINT    rint
#define SIN	sin
#define ATAN	atan
#define EXP	exp
#define LOG	log
#define POW	pow
#endif

const int K = 256;

real W[K], X[K], Y[K], Z[K], H[K];
bool B[K];

static void funcAddSub() {
  for(int i=0;i<K;i+=2) {
    W[i+0] = X[i+0] + Y[i+0];
    W[i+1] = X[i+1] - Y[i+1];
  }
}

static void funcMul() {
  for(int i=0;i<K;i++) W[i] = X[i] * Y[i];
}

static void funcDiv() {
  for(int i=0;i<K;i++) W[i] = X[i] / Y[i];
}

static void funcCastDouble() {
  for(int i=0;i<K;i++) W[i] = (double)H[i];
}

static void funcCompare() {
  for(int i=0;i<K;i+=2) {
    B[i+0] = X[i+0] == Y[i+0];
    B[i+1] = X[i+1] >= Y[i+1];
  }
}

static void funcFMA() {
  for(int i=0;i<K;i++) W[i] = FMA(X[i], Y[i], Z[i]);
}

static void funcSqrt() {
  for(int i=0;i<K;i++) W[i] = SQRT(H[i]);
}

static void funcRint() {
  for(int i=0;i<K;i++) W[i] = RINT(H[i]);
}

static void funcSin() {
  for(int i=0;i<K;i++) W[i] = SIN(X[i]);
}

static void funcAtan() {
  for(int i=0;i<K;i++) W[i] = ATAN(H[i]);
}

static void funcExp() {
  for(int i=0;i<K;i++) W[i] = EXP(X[i]);
}

static void funcLog() {
  for(int i=0;i<K;i++) W[i] = LOG(H[i]);
}

static void funcPow() {
  for(int i=0;i<K;i++) W[i] = POW(X[i], Y[i]);
}

void donothing();

static inline int64_t timeus() {
  return chrono::duration_cast<chrono::microseconds>
    (chrono::system_clock::now() - chrono::system_clock::from_time_t(0)).count();
}

void measure(const char* mes, void (*func)(void), int opPerCall, int64_t sec_us) {
  int64_t N = 10, t0, t1, t2, t3;

  for(;;) {
    t0 = timeus();
    for(int64_t i=0;i<N;i++) { (*func)(); donothing(); }
    t1 = timeus();
    if (t1 - t0 > 100000) break;
    N *= 2;
  }

  const int64_t M = N * sec_us / (t1 - t0);

  this_thread::sleep_for(chrono::microseconds(sec_us));

  t2 = timeus();
  for(int64_t i=0;i<M;i++) { (*func)(); donothing(); }
  t3 = timeus();

  printf("%s : %g Mops/second\n", mes, M * opPerCall / double(t3 - t2));
}

int main(int argc, char **argv) {
  int64_t sec_us = 1000000;
  if (argc >= 2) sec_us = int64_t(atof(argv[1]) * 1000000);

  for(int i=0;i<K;i++) {
    X[i] = (33 + i * 0.001) * ((i & 4) ? -1 : 1) / real(10);
    Y[i] = (22 + i * 0.001) * ((i & 2) ? -1 : 1) / real(10);
    Z[i] = (11 + i * 0.001) * ((i & 1) ? -1 : 1) / real(10);
    H[i] = (real(1.234567890123456e+10) + real(9.8765432109876543e+5) * i) / real(10);
  }

  time_t t = time(NULL);
  printf("Date                 : %s", ctime(&t));
  printf("TLFloat version      : %d.%d.%d\n", TLFLOAT_VERSION_MAJOR, TLFLOAT_VERSION_MINOR, TLFLOAT_VERSION_PATCH);
  printf("Config               : %s\n", CONFIG);
  printf("Measurement time     : %g sec\n", sec_us / 1000000.0);

  measure("Addition            ", funcAddSub     , K, sec_us);
  measure("Multiplication      ", funcMul        , K, sec_us);
  measure("Division            ", funcDiv        , K, sec_us);
  measure("Cast to/from double ", funcCastDouble , K, sec_us);
  measure("Compare             ", funcCompare    , K, sec_us);
  measure("FMA                 ", funcFMA        , K, sec_us);
  measure("Square root         ", funcSqrt       , K, sec_us);
  measure("Rint                ", funcRint       , K, sec_us);
  measure("Sin                 ", funcSin        , K, sec_us);
  measure("Atan                ", funcAtan       , K, sec_us);
  measure("Exp                 ", funcExp        , K, sec_us);
  measure("Log                 ", funcLog        , K, sec_us);
  measure("Pow                 ", funcPow        , K, sec_us);

  exit(0);
}
