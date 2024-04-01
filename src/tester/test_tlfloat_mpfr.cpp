#include <iostream>
#include <cstdint>
#include <cfloat>
#define MPFR_WANT_FLOAT128
#include <mpfr.h>

#define SUPPRESS_WARNINGS

#include "tlmath.hpp"
#include "tlfloatutil.hpp"
#include "auxiliary.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

#define TEST_FLOAT
#define TEST_DOUBLE
#define TEST_QUAD
#define TEST_OCTUPLE

#define TEST_SQRT
#define TEST_TRIG
#define TEST_ROUND
#define TEST_ARITH
#define TEST_INVTRIG
#define TEST_EXP
#define TEST_LOG
#define TEST_POW
#define TEST_CBRT
#define TEST_HYP
#define TEST_MODREM
#define TEST_MISC

typedef UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;
typedef UnpackedFloat<uint64_t, BigUInt<7>, 11, 52> udouble;
typedef UnpackedFloat<BigUInt<7>, BigUInt<8>, 15, 112> uquad;
typedef UnpackedFloat<BigUInt<8>, BigUInt<9>, 19, 236> uoctuple;

static constexpr uint64_t piao[] = {0x0105DF531D89CD91ULL, 0x948127044533E63AULL, 0x62633145C06E0E68ULL, 0x6487ED5110B4611AULL};
static constexpr xoctuple xpio(BigUInt<8>(piao), 0, false, false, false, false);

static const Octuple M_PIo_ = (Octuple)xpio.cast((const uoctuple *)nullptr);

static float rndf(shared_ptr<RNG> rng) {
  if (rng->nextLT(32) != 0) {
    for(;;) {
      float f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (finite(f)) return f;
    }
  } else {
    for(;;) {
      float f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (!finitef(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rintf(f) * M_PI / 2;

      int32_t u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += rng->nextLT(5) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(u));

      return f;
    }
  }
}

static double rndd(shared_ptr<RNG> rng) {
  if (rng->nextLT(32) != 0) {
    for(;;) {
      double f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (finite(f)) return f;
    }
  } else {
    for(;;) {
      double f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (!finite(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rint(f) * M_PI / 2;

      int64_t u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += rng->nextLT(5) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(u));

      return f;
    }
  }
}

static quad rndq(shared_ptr<RNG> rng) {
  if (rng->nextLT(32) != 0) {
    for(;;) {
      quad f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (finiteq(f)) return f;
    }
  } else {
    for(;;) {
      quad f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (!finiteq(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rintq(f) * M_PIq_ / 2;

      BigInt<7> u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += rng->nextLT(5) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(u));

      return f;
    }
  }
}

static Octuple rndo(shared_ptr<RNG> rng) {
  if (rng->nextLT(32) != 0) {
    for(;;) {
      Octuple f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (finite(f)) return f;
    }
  } else {
    for(;;) {
      Octuple f;
      rng->nextBytes((unsigned char *)&f, sizeof(f));
      if (!finite(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rint(f) * M_PIo_ / 2;

      BigInt<8> u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += rng->nextLT(5) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(u));

      return f;
    }
  }
}

int main(int argc, char **argv) {
  uint64_t niter = argc > 1 ? strtoull(argv[1], nullptr, 10) : 100000;

  if (niter == 0) {
#if defined(__GNUC__)
    cout << "Build " << __DATE__ << " " << __TIME__ << endl;
#endif
    cout << "Testing continuously" << endl;
  }

  auto rng = createPreferredRNG();

  for(uint64_t i=0;niter == 0 || i<niter;i++) {
    int p = rng->nextLT(7);

    {
      float x = rndf(rng), y = rndf(rng);

      {
	mpfr_set_default_prec(32);

	mpfr_t mx, my;
	mpfr_inits(mx, my, NULL);

#if defined(TEST_FLOAT) && defined(TEST_SQRT)
	{
	  ufloat xfr = sqrt(Float(x)).getUnpacked().cast((const ufloat *)nullptr);
	  float r = (float)Float(xfr.cast((const ufloat *)nullptr));

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_sqrt(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat sqrt\n");
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n", c);
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  ufloat xfr = hypot(Float(x), Float(y)).getUnpacked().cast((const ufloat *)nullptr);
	  float r = (float)Float(xfr.cast((const ufloat *)nullptr));

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_hypot(mx, mx, my, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat hypot\n");
	    printf("x = %.8g\n", x);
	    printf("y = %.8g\n", y);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n", c);
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_TRIG)
	{
	  float r = (float)sin(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_sin(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat sin\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)cos(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_cos(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat cos\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)tan(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_tan(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.56) {
	    printf("\nfloat tan\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_INVTRIG)
	{
	  float r = (float)atan2(Float(y), Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_atan2(mx, my, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat atan2\n");
	    printf("ulp = %g\n", ulp);
	    printf("y = %.8g\n", y);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)asin(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_asin(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat asin\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)acos(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_acos(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat acos\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)atan(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_atan(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat atan\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_EXP)
	{
	  float r = (float)exp(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_exp(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat exp\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)expm1(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_expm1(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat expm1\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)exp2(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_exp2(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat exp2\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)exp10(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_exp10(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat exp10\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_LOG)
	{
	  float r = (float)log(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat log\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)log1p(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log1p(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat log1p\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)log2(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log2(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat log2\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)log10(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log10(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat log10\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_POW)
	{
	  float r = (float)pow(Float(y), Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_pow(mx, my, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat pow\n");
	    printf("ulp = %g\n", ulp);
	    printf("y = %.8g\n", y);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_MODREM)
	{
	  float r = (float)fmod(Float(y), Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_fmod(mx, my, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat fmod\n");
	    printf("ulp = %g\n", ulp);
	    printf("y = %.8g\n", y);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)remainder(Float(y), Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_remainder(mx, my, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat remainder\n");
	    printf("ulp = %g\n", ulp);
	    printf("y = %.8g\n", y);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_CBRT)
	{
	  float r = (float)cbrt(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_cbrt(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat cbrt\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_HYP)
	{
	  float r = (float)sinh(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_sinh(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat sinh\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)cosh(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_cosh(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat cosh\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)tanh(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_tanh(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat tanh\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)asinh(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_asinh(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat asinh\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)acosh(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_acosh(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat acosh\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)atanh(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_atanh(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.55) {
	    printf("\nfloat atanh\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_FLOAT) && defined(TEST_MISC)
	{
	  float r = (float)fabs(Float(x));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_abs(mx, mx, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat fabs\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)copysign(Float(x), Float(y));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_copysign(mx, mx, my, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5 && !isnan(y)) {
	    printf("\nfloat copysign\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("y = %.8g\n", y);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)fmax(Float(x), Float(y));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_max(mx, mx, my, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat fmax\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)fmin(Float(x), Float(y));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_min(mx, mx, my, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat fmin\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  float r = (float)fdim(Float(x), Float(y));
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_dim(mx, mx, my, GMP_RNDN);
	  float c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5) {
	    printf("\nfloat fdim\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Float w;
	  float r = (float)modf(Float(x), &w);
	  ufloat xfr = Float(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_modf(mx, my, mx, GMP_RNDN);
	  double ci = mpfr_get_d(mx, GMP_RNDN);
	  float c = mpfr_get_d(my, GMP_RNDN);
	  double ulp = countULP(xfr, my, ufloat::floatmin(), ufloat::floatmax());
	  if (ulp > 0.5 || (!(isnan(w) && isnan(ci)) && double(w) != ci)) {
	    printf("\nfloat modf\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.8g\n", x);
	    printf("t = %.8g\n", r);
	    printf("c = %.8g\n", c);
	    printf("ci = %.8g\n\n", ci);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

	mpfr_clears(mx, my, NULL);
      }
    }

    //

    {
      double x = rndd(rng), y = rndd(rng);

      {
	mpfr_set_default_prec(64);

	mpfr_t mx, my;
	mpfr_inits(mx, my, NULL);

#if defined(TEST_DOUBLE) && defined(TEST_SQRT)
	{
	  auto xfr = sqrt(Double(x)).getUnpacked().cast((const udouble *)nullptr);
	  double r = (double)Double(xfr.cast((const udouble *)nullptr));

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_sqrt(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble sqrt\n");
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n", c);
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  auto xfr = hypot(Double(x), Double(y)).getUnpacked().cast((const udouble *)nullptr);
	  double r = (double)Double(xfr.cast((const udouble *)nullptr));

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_hypot(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble hypot\n");
	    printf("x = %.20lg\n", x);
	    printf("y = %.20lg\n", y);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n", c);
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_TRIG)
	{
	  double r = (double)sin(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_sin(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.506) {
	    printf("\ndouble sin\n");
	    printf("ulp = %g\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)cos(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_cos(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.506) {
	    printf("\ndouble cos\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)tan(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_tan(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.51) {
	    printf("\ndouble tan\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_INVTRIG)
	{
	  double r = (double)atan2(Double(y), Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_atan2(mx, my, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.51) {
	    printf("\ndouble atan2\n");
	    printf("ulp = %lg\n", ulp);
	    printf("y = %.20lg\n", y);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)asin(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_asin(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.51) {
	    printf("\ndouble asin\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)acos(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_acos(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.51) {
	    printf("\ndouble acos\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)atan(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_atan(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.51) {
	    printf("\ndouble atan\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_EXP)
	{
	  double r = (double)exp(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_exp(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble exp\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)expm1(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_expm1(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble expm1\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)exp2(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_exp2(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble exp2\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)exp10(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_exp10(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble exp10\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_LOG)
	{
	  double r = (double)log(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble log\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)log1p(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log1p(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble log1p\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)log2(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log2(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.506) {
	    printf("\ndouble log2\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)log10(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_log10(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.506) {
	    printf("\ndouble log10\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_POW)
	{
	  double r = (double)pow(Double(y), Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_pow(mx, my, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.51) {
	    printf("\ndouble pow\n");
	    printf("ulp = %lg\n", ulp);
	    printf("y = %.20lg\n", y);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_MODREM)
	{
	  double r = (double)fmod(Double(y), Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_fmod(mx, my, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble fmod\n");
	    printf("ulp = %lg\n", ulp);
	    printf("y = %.20lg\n", y);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)remainder(Double(y), Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_remainder(mx, my, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble remainder\n");
	    printf("ulp = %lg\n", ulp);
	    printf("y = %.20lg\n", y);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_CBRT)
	{
	  double r = (double)cbrt(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_cbrt(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble cbrt\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_HYP)
	{
	  double r = (double)sinh(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_sinh(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble sinh\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)cosh(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_cosh(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble cosh\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)tanh(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_tanh(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble tanh\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)asinh(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_asinh(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble asinh\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)acosh(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_acosh(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble acosh\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)atanh(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_atanh(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.505) {
	    printf("\ndouble atanh\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_DOUBLE) && defined(TEST_MISC)
	{
	  double r = (double)fabs(Double(x));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_abs(mx, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble fabs\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)copysign(Double(x), Double(y));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_copysign(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5 && !isnan(y)) {
	    printf("\ndouble copysign\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}


	{
	  double r = (double)fmax(Double(x), Double(y));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_max(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble fmax\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)fmin(Double(x), Double(y));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_min(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble fmin\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  double r = (double)fdim(Double(x), Double(y));
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_set_d(my, y, GMP_RNDN);
	  mpfr_dim(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5) {
	    printf("\ndouble fdim\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Double w;
	  double r = (double)modf(Double(x), &w);
	  udouble xfr = Double(r).getUnpacked();

	  mpfr_set_d(mx, x, GMP_RNDN);
	  mpfr_modf(mx, my, mx, GMP_RNDN);
	  double ci = mpfr_get_d(mx, GMP_RNDN);
	  double c = mpfr_get_d(my, GMP_RNDN);
	  double ulp = countULP(xfr, my, udouble::floatmin(), udouble::floatmax());
	  if (ulp > 0.5 || (!(isnan(w) && isnan(ci)) && double(w) != ci)) {
	    printf("\ndouble modf\n");
	    printf("ulp = %lg\n", ulp);
	    printf("x = %.20lg\n", x);
	    printf("t = %.20lg\n", r);
	    printf("c = %.20lg\n\n", c);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

	mpfr_clears(mx, my, NULL);
      }
    }

    if ((i & 31) == 0) {
      quad x = rndq(rng), y = rndq(rng), z = rndq(rng);

      {
	mpfr_set_default_prec(128);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

#if defined(TEST_QUAD) && defined(TEST_ARITH)
	{
	  auto xfr = (Quad(x) + Quad(y)).getUnpacked();
	  quad r = (quad)Quad(xfr);

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_add(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad add\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << c << ", " << to_string(mx, 72) << " : " << to_string_d(Quad(c).getUnpacked()) << endl;
	    cout << "r = " << r << ", " << " : " << to_string_d(Quad(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  auto xfr = (Quad(x) * Quad(y)).getUnpacked();
	  quad r = (quad)Quad(xfr);

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_mul(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad mul\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << c << ", " << to_string(mx, 72) << " : " << to_string_d(Quad(c).getUnpacked()) << endl;
	    cout << "r = " << r << " : " << to_string_d(Quad(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  auto xfr = (Quad(x) / Quad(y)).getUnpacked();
	  quad r = (quad)Quad(xfr);

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_div(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad div\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << c << ", " << to_string(mx, 72) << " : " << to_string_d(Quad(c).getUnpacked()) << endl;
	    cout << "r = " << r << " : " << to_string_d(Quad(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  auto xfr = fma(Quad(x), Quad(y), Quad(z)).getUnpacked();
	  quad r = (quad)Quad(xfr);

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_set_float128(mz, z, GMP_RNDN);
	  mpfr_fma(mx, mx, my, mz, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad fma\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "z = " << z << endl;
	    cout << "c = " << c << ", " << to_string(mx, 72) << " : " << to_string_d(Quad(c).getUnpacked()) << endl;
	    cout << "r = " << r << ", " << " : " << to_string_d(Quad(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_SQRT)
	{
	  auto xfr = sqrt(Quad(x)).getUnpacked();
	  quad r = (quad)Quad(xfr);

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_sqrt(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad sqrt\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  auto xfr = hypot(Quad(x), Quad(y)).getUnpacked();
	  quad r = (quad)Quad(xfr);

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_hypot(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad hypot\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_TRIG)
	{
	  quad r = (quad)sin(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_sin(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad sin\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)cos(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_cos(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad cos\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)tan(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_tan(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad tan\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_INVTRIG)
	{
	  quad r = (quad)atan2(Quad(y), Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_atan2(mx, my, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad atan2\n");
	    printf("ulp = %g\n", ulp);
	    cout << "y = " << y << endl;
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)asin(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_asin(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad asin\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)acos(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_acos(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad acos\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)atan(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_atan(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad atan\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_EXP)
	{
	  quad r = (quad)exp(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_exp(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad exp\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)expm1(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_expm1(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad expm1\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)exp2(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_exp2(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad exp2\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)exp10(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_exp10(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad exp10\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_LOG)
	{
	  quad r = (quad)log(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_log(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad log\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)log1p(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_log1p(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad log1p\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)log2(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_log2(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad log2\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)log10(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_log10(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad log10\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_POW)
	{
	  quad r = (quad)pow(Quad(y), Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_pow(mx, my, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad pow\n");
	    printf("ulp = %g\n", ulp);
	    cout << "y = " << y << endl;
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_MODREM)
	{
	  quad r = (quad)fmod(Quad(y), Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_fmod(mx, my, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad fmod\n");
	    printf("ulp = %g\n", ulp);
	    cout << "y = " << y << endl;
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)remainder(Quad(y), Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_remainder(mx, my, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad remainder\n");
	    printf("ulp = %g\n", ulp);
	    cout << "y = " << y << endl;
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_CBRT)
	{
	  quad r = (quad)cbrt(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_cbrt(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad cbrt\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_HYP)
	{
	  quad r = (quad)sinh(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_sinh(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad sinh\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)cosh(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_cosh(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad cosh\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)tanh(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_tanh(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad tanh\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)asinh(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_asinh(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad asinh\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)acosh(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_acosh(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad acosh\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)atanh(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_atanh(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.501) {
	    printf("\nquad atanh\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_QUAD) && defined(TEST_MISC)
	{
	  quad r = (quad)fabs(Quad(x));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_abs(mx, mx, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad fabs\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)copysign(Quad(x), Quad(y));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_copysign(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5 && !isnanq(y)) {
	    printf("\nquad copysign\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)fmax(Quad(x), Quad(y));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_max(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad fmax\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)fmin(Quad(x), Quad(y));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_min(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad fmin\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  quad r = (quad)fdim(Quad(x), Quad(y));
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_set_float128(my, y, GMP_RNDN);
	  mpfr_dim(mx, mx, my, GMP_RNDN);
	  quad c = mpfr_get_float128(mx, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5) {
	    printf("\nquad fdim\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Quad w;
	  quad r = (quad)modf(Quad(x), &w);
	  uquad xfr = Quad(r).getUnpacked();

	  mpfr_set_float128(mx, x, GMP_RNDN);
	  mpfr_modf(mx, my, mx, GMP_RNDN);
	  double ci = mpfr_get_d(mx, GMP_RNDN);
	  quad c = mpfr_get_float128(my, GMP_RNDN);
	  double ulp = countULP(xfr, my, uquad::floatmin(), uquad::floatmax());
	  if (ulp > 0.5 || (!(isnan(w) && isnan(ci)) && double(w) != ci)) {
	    printf("\nquad modf\n");
	    printf("ulp = %g\n", ulp);
	    cout << "x = " << x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << r << endl;
	    printf("ci = %.8g\n", ci);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

	mpfr_clears(mx, my, mz, NULL);
      }
    }

    if ((i & 2047) == 0) {
      Octuple x = rndo(rng);
      Octuple y = rndo(rng);
      Octuple z = rndo(rng);

      {
	mpfr_set_default_prec(256);
	mpfr_set_emin(-100000000);
	mpfr_set_emax( 100000000);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

#if defined(TEST_OCTUPLE) && defined(TEST_ARITH)
	{
	  uoctuple xfr = (Octuple(x) + Octuple(y)).getUnpacked();
	  Octuple r = Octuple(xfr);

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_add(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple add\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  uoctuple xfr = (Octuple(x) * Octuple(y)).getUnpacked();
	  Octuple r = Octuple(xfr);

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_mul(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple mul\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  uoctuple xfr = (Octuple(x) / Octuple(y)).getUnpacked();
	  Octuple r = Octuple(xfr);

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_div(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple div\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}

	{
	  uoctuple xfr = fma(Octuple(x), Octuple(y), Octuple(z)).getUnpacked();
	  Octuple r = Octuple(xfr);

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(mz, z.getUnpacked(), GMP_RNDN);
	  mpfr_fma(mx, mx, my, mz, GMP_RNDN);
	  double ulp = countULP(xfr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple fma\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "z = " << z << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_SQRT)
	{
	  uoctuple xcr = sqrt(Octuple(x)).getUnpacked().cast((const uoctuple *)nullptr);
	  Octuple r = Octuple(xcr);

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_sqrt(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple sqrt\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  uoctuple xcr = hypot(Octuple(x), Octuple(y)).getUnpacked().cast((const uoctuple *)nullptr);
	  Octuple r = Octuple(xcr);

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_hypot(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple hypot\n");
	    cout << "x = " << x << endl;
	    cout << "y = " << y << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_TRIG)
	{
	  Octuple r = sin(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_sin(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple sin\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = cos(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_cos(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple cos\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = tan(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_tan(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple tan\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_INVTRIG)
	{
	  Octuple r = atan2(Octuple(y), Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_atan2(mx, my, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple atan2\n");
	    cout << "y = " << to_string(y, 72) << endl;
	    cout << "x = " << to_string(x, 72) << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = asin(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_asin(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple asin\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = acos(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_acos(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple acos\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = atan(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_atan(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple atan\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_EXP)
	{
	  Octuple r = exp(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_exp(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple exp\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = expm1(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_expm1(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple expm1\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = exp2(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_exp2(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple exp2\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = exp10(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_exp10(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple exp10\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_LOG)
	{
	  Octuple r = log(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_log(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple log\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = log1p(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_log1p(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple log1p\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = log2(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_log2(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple log2\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = log10(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_log10(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple log10\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_POW)
	{
	  Octuple r = pow(Octuple(y), Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_pow(mx, my, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple pow\n");
	    cout << "y = " << to_string(y, 72) << endl;
	    cout << "x = " << to_string(x, 72) << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_MODREM)
	{
	  Octuple r = fmod(Octuple(y), Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_fmod(mx, my, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple fmod\n");
	    cout << "y = " << to_string(y, 72) << endl;
	    cout << "x = " << to_string(x, 72) << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = remainder(Octuple(y), Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_remainder(mx, my, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple remainder\n");
	    cout << "y = " << to_string(y, 72) << endl;
	    cout << "x = " << to_string(x, 72) << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_CBRT)
	{
	  Octuple r = cbrt(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_cbrt(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple cbrt\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_HYP)
	{
	  Octuple r = sinh(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_sinh(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple sinh\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = cosh(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_cosh(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple cosh\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = tanh(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_tanh(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple tanh\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = asinh(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_asinh(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple asinh\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = acosh(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_acosh(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple acosh\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = atanh(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_atanh(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5002) {
	    printf("\noctuple atanh\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

#if defined(TEST_OCTUPLE) && defined(TEST_MISC)
	{
	  Octuple r = fabs(Octuple(x));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_abs(mx, mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple fabs\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = copysign(Octuple(x), Octuple(y));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_copysign(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5 && !isnan(y)) {
	    printf("\noctuple copysign\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = fmax(Octuple(x), Octuple(y));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_max(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple fmax\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = fmin(Octuple(x), Octuple(y));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_min(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple fmin\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple r = fdim(Octuple(x), Octuple(y));
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_dim(mx, mx, my, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5) {
	    printf("\noctuple fdim\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	{
	  Octuple w;
	  Octuple r = modf(Octuple(x), &w);
	  uoctuple xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_modf(mx, my, mx, GMP_RNDN);
	  double ci = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, my, uoctuple::floatmin(), uoctuple::floatmax());
	  if (ulp > 0.5 || (!(isnan(w) && isnan(ci)) && double(w) != ci)) {
	    printf("\noctuple modf\n");
	    cout << "x = " << x << endl;
	    cout << "c = " << to_string(mx, 72) << endl;
	    cout << "r = " << to_string(r , 72) << " : " << to_string_d(Octuple(r).getUnpacked()) << endl;
	    printf("ci = %.8g\n", ci);
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}
#endif

	mpfr_clears(mx, my, mz, NULL);
      }
    }
  }

  cout << "OK" << endl;

  exit(0);
}
