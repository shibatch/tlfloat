#include <iostream>
#include <cstdint>

#if defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#include <quadmath.h>
#endif

#define MPFR_WANT_FLOAT128
#include <mpfr.h>

#define TLFLOAT_SUPPRESS_WARNINGS

#include "tlfloat/tlmath.hpp"
#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

typedef UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;

#define TEST_SQRT
#define TEST_TRIG
#define TEST_INVTRIG
#define TEST_EXP
#define TEST_LOG
#define TEST_CBRT
#define TEST_HYP

int main(int argc, char **argv) {
#ifdef TEST_PH
  int p = argc > 1 ? atoi(argv[1]) : 1;
  double maxulp_ph = 0;
#endif
  double maxulp_sqrt_ = 0, maxulp_sqrt = 0.5;
  double maxulp_sin = 0.5, maxulp_cos = 0.5, maxulp_tan = 0.5;
  double maxulp_asin = 0.5, maxulp_acos = 0.5, maxulp_atan = 0.5;
  double maxulp_exp = 0.5, maxulp_expm1 = 0.5;
  double maxulp_exp2 = 0.5, maxulp_exp10 = 0.5;
  double maxulp_log = 0.5, maxulp_log1p = 0.5;
  double maxulp_log2 = 0.5, maxulp_log10 = 0.5;
  double maxulp_cbrt = 0.5;
  double maxulp_sinh = 0.5, maxulp_cosh = 0.5, maxulp_tanh = 0.5;
  double maxulp_asinh = 0.5, maxulp_acosh = 0.5, maxulp_atanh = 0.5;

  #pragma omp parallel for
  for(uint64_t u64=0;u64 < 0x100000000ULL;u64++) {
    if ((u64 & 0xffffff) == 0) {
      printf("u = %08llx\r", (long long unsigned)u64);
      fflush(stdout);
    }
    uint32_t u = u64;
    float x;
    memcpy((void *)&x, (void *)&u, sizeof(x));

#ifdef TEST_SQRT
    if (x >= 0 && finite(x)) {
      mpfr_set_default_prec(32);
      mpfr_t mx;
      mpfr_inits(mx, NULL);

      auto xfr = sqrt_(Float(x).getUnpacked().cast((const xfloat *)nullptr));
      float r = (float)Float(xfr.cast((const ufloat *)nullptr));

      mpfr_set_d(mx, x, GMP_RNDN);
      mpfr_sqrt(mx, mx, GMP_RNDN);
      float c = mpfr_get_d(mx, GMP_RNDN);
      double ulp = countULP(xfr, mx, ufloat::zero(), ufloat::inf());
      if (ulp > maxulp_sqrt_) {
	maxulp_sqrt_ = ulp;
	printf("\nsqrt_\n");
	printf("ulp = %g\n", ulp);
	printf("x = %.8g\n", x);
	printf("t = %.8g\n", r);
	printf("c = %.8g\n\n", c);
	fflush(stdout);
      }

      mpfr_clears(mx, NULL);
    }

    {
      mpfr_set_default_prec(32);
      mpfr_t mx;
      mpfr_inits(mx, NULL);

      auto xfr = sqrt(Float(x)).getUnpacked().cast((const ufloat *)nullptr);
      float r = (float)Float(xfr.cast((const ufloat *)nullptr));

      mpfr_set_d(mx, x, GMP_RNDN);
      mpfr_sqrt(mx, mx, GMP_RNDN);
      float c = mpfr_get_d(mx, GMP_RNDN);
      double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
      if (ulp > maxulp_sqrt) {
	maxulp_sqrt = ulp;
	printf("\nsqrt\n");
	printf("ulp = %g\n", ulp);
	printf("x = %.8g\n", x);
	printf("t = %.8g\n", r);
	printf("c = %.8g\n\n", c);
	fflush(stdout);
      }

      mpfr_clears(mx, NULL);
    }
#endif

    {
      mpfr_set_default_prec(32);
      mpfr_t mx;
      mpfr_inits(mx, NULL);

#ifdef TEST_TRIG
      {
	float r = (float)sin(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_sin(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_sin) {
	  maxulp_sin = ulp;
	  printf("\nsin\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)cos(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_cos(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_cos) {
	  maxulp_cos = ulp;
	  printf("\ncos\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)tan(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_tan(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_tan) {
	  maxulp_tan = ulp;
	  printf("\ntan\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }
#endif

#ifdef TEST_INVTRIG
      {
	float r = (float)atan(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_atan(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_atan) {
	  maxulp_atan = ulp;
	  printf("\natan\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)asin(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_asin(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_asin) {
	  maxulp_asin = ulp;
	  printf("\nasin\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)acos(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_acos(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_acos) {
	  maxulp_acos = ulp;
	  printf("\nacos\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }
#endif

#ifdef TEST_EXP
      {
	float r = (float)exp(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_exp(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_exp) {
	  maxulp_exp = ulp;
	  printf("\nexp\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)expm1(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_expm1(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_expm1) {
	  maxulp_expm1 = ulp;
	  printf("\nexpm1\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)exp2(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_exp2(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_exp2) {
	  maxulp_exp2 = ulp;
	  printf("\nexp2\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)exp10(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_exp10(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_exp10) {
	  maxulp_exp10 = ulp;
	  printf("\nexp10\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }
#endif

#ifdef TEST_LOG
      {
	float r = (float)log(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_log(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_log) {
	  maxulp_log = ulp;
	  printf("\nlog\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)log1p(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_log1p(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_log1p) {
	  maxulp_log1p = ulp;
	  printf("\nlog1p\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)log2(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_log2(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_log2) {
	  maxulp_log2 = ulp;
	  printf("\nlog2\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)log10(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_log10(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_log10) {
	  maxulp_log10 = ulp;
	  printf("\nlog10\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }
#endif

#ifdef TEST_CBRT
      {
	float r = (float)cbrt(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_cbrt(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_cbrt) {
	  maxulp_cbrt = ulp;
	  printf("\ncbrt\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }
#endif

#ifdef TEST_HYP
      {
	float r = (float)sinh(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_sinh(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_sinh) {
	  maxulp_sinh = ulp;
	  printf("\nsinh\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)cosh(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_cosh(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_cosh) {
	  maxulp_cosh = ulp;
	  printf("\ncosh\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)tanh(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_tanh(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_tanh) {
	  maxulp_tanh = ulp;
	  printf("\ntanh\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)asinh(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_asinh(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_asinh) {
	  maxulp_asinh = ulp;
	  printf("\nasinh\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)acosh(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_acosh(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_acosh) {
	  maxulp_acosh = ulp;
	  printf("\nacosh\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }

      {
	float r = (float)atanh(Float(x));
	ufloat xfr = Float(r).getUnpacked();

	mpfr_set_d(mx, x, GMP_RNDN);
	mpfr_atanh(mx, mx, GMP_RNDN);
	float c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xfr, mx, ufloat::floatdenormmin(), ufloat::floatmax());
	if (ulp > maxulp_atanh) {
	  maxulp_atanh = ulp;
	  printf("\natanh\n");
	  printf("ulp = %g\n", ulp);
	  printf("x = %.8g\n", x);
	  printf("t = %.8g\n", r);
	  printf("c = %.8g\n\n", c);
	  fflush(stdout);
	}
      }
#endif

      mpfr_clears(mx, NULL);
    }
  }
}
