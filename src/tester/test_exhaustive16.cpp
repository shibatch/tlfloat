#include <iostream>
#define MPFR_WANT_FLOAT128
#include <mpfr.h>
#include <arm_fp16.h>
#include <signal.h>

#define TLFLOAT_SUPPRESS_WARNINGS

#include "tlfloat/tlmath.hpp"
#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

#define TEST_SQRT
#define TEST_TRIG
#define TEST_ROUND
#define TEST_ARITH
#define TEST_COMPARE
#define TEST_INVTRIG
#define TEST_EXP
#define TEST_LOG
#define TEST_FMA
#define TEST_POW
#define TEST_CBRT
#define TEST_HYP
#define TEST_MODREM

static void sighandler(int signum) {
  cerr << "Caught SIGILL" << endl;
  exit(0);
}

int main(int argc, char **argv) {
  signal(SIGILL, sighandler);

  typedef UnpackedFloat<uint16_t, uint32_t, 5, 10> uhalf;
  typedef TLFloat<uhalf> Half;

  #pragma omp parallel for
  for(uint32_t u32=0;u32 < 0x10000;u32++) {
    if ((u32 & 0xff) == 0) {
      printf("u = %04x\r", u32);
      fflush(stdout);
    }
    uint16_t u = u32;
    __fp16 fu;
    memcpy((void *)&fu, (void *)&u, sizeof(fu));

#ifdef TEST_SQRT
    {
      __fp16 fw = vsqrth_f16(fu);
      __fp16 fx = __fp16(sqrt(Half(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("sqrt u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	exit(-1);
      }
    }
#endif

    {
      mpfr_set_default_prec(32);

      mpfr_t mx, my, mz;
      mpfr_inits(mx, my, mz, NULL);

      Half x = Half(fu);

#ifdef TEST_TRIG
      {
	Half r = sin(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_sin(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf sin\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = cos(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cos(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf cos\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = tan(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_tan(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf tan\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_INVTRIG
      {
	Half r = asin(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_asin(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf asin\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = acos(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_acos(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf acos\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = atan(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_atan(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf atan\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_EXP
      {
	Half r = exp(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_exp(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf exp\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = expm1(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_expm1(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf expm1\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = exp2(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_exp2(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf exp2\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = exp10(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_exp10(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf exp10\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_LOG
      {
	Half r = log(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf log\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = log1p(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log1p(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf log1p\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = log2(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log2(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf log2\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = log10(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log10(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf log10\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_CBRT
      {
	Half r = cbrt(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cbrt(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf cbrt\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_HYP
      {
	Half r = sinh(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_sinh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf sinh\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = cosh(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cosh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf cosh\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = tanh(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_tanh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf tanh\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = asinh(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_asinh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf asinh\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = acosh(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_acosh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf acosh\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	Half r = atanh(x);
	uhalf xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_atanh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	if (ulp > 0.7) {
	  printf("\nhalf atanh\n");
	  cout << "x = " << (__fp16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

      mpfr_clears(mx, my, mz, NULL);
    }

#ifdef TEST_ROUND
    {
      __fp16 fw = vrndh_f16(fu);
      __fp16 fx = __fp16(trunc(uhalf(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("trunc u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	exit(-1);
      }
    }

    {
      __fp16 fw = vrndmh_f16(fu);
      __fp16 fx = __fp16(floor(uhalf(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("floor u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	exit(-1);
      }
    }

    {
      __fp16 fw = vrndph_f16(fu);
      __fp16 fx = __fp16(ceil(uhalf(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("ceil u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	exit(-1);
      }
    }

    {
      __fp16 fw = vrndah_f16(fu);
      __fp16 fx = __fp16(round(uhalf(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("round u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	exit(-1);
      }
    }

    {
      __fp16 fw = vrndnh_f16(fu);
      __fp16 fx = __fp16(rint(uhalf(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("rint u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	exit(-1);
      }
    }
#endif

    for(uint32_t v32=0;v32 < 0x10000;v32++) {
      uint16_t v = v32;
      __fp16 fv;
      memcpy((void *)&fv, (void *)&v, sizeof(fv));

#ifdef TEST_ARITH
      {
	__fp16 fw = vaddh_f16(fu, fv);
	__fp16 fx = __fp16(uhalf::addsub(uhalf(fu), uhalf(fv), false));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan(fw) && isnan(fx)) && w != x) {
	  printf("u = %04x, v = %04x, w = %04x, x = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	  cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	  exit(-1);
	}
      }

      {
	__fp16 fw = vsubh_f16(fu, fv);
	__fp16 fx = __fp16(uhalf::addsub(uhalf(fu), uhalf(fv), true));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan(fw) && isnan(fx)) && w != x) {
	  printf("u = %04x, v = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	  cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	  exit(-1);
	}
      }

      {
	__fp16 fw = vmulh_f16(fu, fv);
	__fp16 fx = __fp16(uhalf::mul(uhalf(fu), uhalf(fv)));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan(fw) && isnan(fx)) && w != x) {
	  printf("u = %04x, v = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	  cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	  exit(-1);
	}
      }

      {
	__fp16 fw = vdivh_f16(fu, fv);
	__fp16 fx = __fp16(uhalf::div(uhalf(fu), uhalf(fv)));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan(fw) && isnan(fx)) && w != x) {
	  printf("u = %04x, v = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	  cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_COMPARE
      {
	bool bc = vceqh_f16(fu, fv);
	bool bt = uhalf(fu) == uhalf(fv);

	if (bc != bt) {
	  printf("== : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = vcleh_f16(fu, fv);
	bool bt = uhalf(fu) <= uhalf(fv);

	if (bc != bt) {
	  printf("<= : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = vcgeh_f16(fu, fv);
	bool bt = uhalf(fu) >= uhalf(fv);

	if (bc != bt) {
	  printf(">= : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = vclth_f16(fu, fv);
	bool bt = uhalf(fu) < uhalf(fv);

	if (bc != bt) {
	  printf("< : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = vcgth_f16(fu, fv);
	bool bt = uhalf(fu) > uhalf(fv);

	if (bc != bt) {
	  printf("> : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_INVTRIG
      {
	mpfr_set_default_prec(32);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

	Half x = Half(fu);
	Half y = Half(fv);

	{
	  Half r = atan2(y, x);
	  uhalf xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_atan2(mx, my, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	  if (ulp > 0.7) {
	    printf("\nhalf atan2\n");
	    cout << "y = " << (__fp16)y << endl;
	    cout << "x = " << (__fp16)x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	mpfr_clears(mx, my, mz, NULL);
      }
#endif

#ifdef TEST_POW
      {
	mpfr_set_default_prec(32);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

	Half x = Half(fu);
	Half y = Half(fv);

	{
	  Half r = pow(x, y);
	  uhalf xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_pow(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	  if (ulp > 0.7) {
	    printf("\nhalf pow\n");
	    cout << "x = " << (__fp16)x << endl;
	    cout << "y = " << (__fp16)y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	mpfr_clears(mx, my, mz, NULL);
      }
#endif

#ifdef TEST_MODREM
      {
	mpfr_set_default_prec(32);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

	Half x = Half(fu);
	Half y = Half(fv);

	{
	  Half r = fmod(x, y);
	  uhalf xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_fmod(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	  if (ulp > 0.5) {
	    printf("\nhalf fmod\n");
	    cout << "x = " << (__fp16)x << endl;
	    cout << "y = " << (__fp16)y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	mpfr_clears(mx, my, mz, NULL);
      }

      {
	mpfr_set_default_prec(32);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

	Half x = Half(fu);
	Half y = Half(fv);

	{
	  Half r = remainder(x, y);
	  uhalf xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_remainder(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, uhalf::floatdenormmin(), uhalf::floatmax());
	  if (ulp > 0.5) {
	    printf("\nhalf remainder\n");
	    cout << "x = " << (__fp16)x << endl;
	    cout << "y = " << (__fp16)y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (__fp16)r << " : " << to_string_d(Half(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	mpfr_clears(mx, my, mz, NULL);
      }
#endif

#ifdef TEST_FMA
      __fp16 fz[] = {
	6e-8, -12e-8, 18e-8, -24e-8, 30e-8, -36e-8,
	1e-5, -1e-4, 1e-3, -1e-2, 65504, -32768,
	+0.0, -0.0, (__fp16)+INFINITY, (__fp16)-INFINITY
      };
      for(unsigned i=0;i<sizeof(fz)/sizeof(__fp16);i++) {
	//__fp16 fw = vfmah_f16(fz[i], fu, fv);
	__fp16 fw = fma(double(fz[i]), double(fu), double(fv));
	__fp16 fx = __fp16(uhalf::fma(uhalf(fz[i]), uhalf(fu), uhalf(fv)));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan(fw) && isnan(fx)) && w != x) {
	  printf("i = %u, u = %04x, v = %04x, y = %04x, z = %04x\n", i, (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  cout << "c: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	  cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	  exit(-1);
	}
      }
#endif
    }
  }

  cout << "OK" << endl;
  return 0;
}
