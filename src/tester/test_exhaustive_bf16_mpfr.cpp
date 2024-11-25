#include <iostream>
#include <atomic>
#include <signal.h>

#include <detect.h>

#if defined(TLFLOAT_ENABLE_MPFR_WANT_FLOAT128)
#define MPFR_WANT_FLOAT128
#endif

#include <mpfr.h>

#include "suppress.hpp"

#include "tlfloat/tlmath.hpp"
#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

#define TEST_SQRT
#define TEST_TRIG
#define TEST_ROUND
#define TEST_INVTRIG
#define TEST_EXP
#define TEST_LOG
#define TEST_FMA
#define TEST_POW
#define TEST_CBRT
#define TEST_HYP
#define TEST_ERF
#define TEST_GAMMA
#define TEST_MODREM

int main(int argc, char **argv) {
  typedef UnpackedFloat<uint16_t, uint32_t, 8, 7> ubf16;
  typedef TLFloat<ubf16> BFloat16;

  atomic_int progress;

  #pragma omp parallel for
  for(uint32_t u32=0;u32 < 0x10000;u32++) {
    if ((u32 & 0xff) == 0xff) {
      progress++;
      int k = progress;
      printf(" %d / 256\r", k);
      fflush(stdout);
    }
    uint16_t u = ((u32 << 8) | (u32 >> 8)) & 0xffff;
    BFloat16 fu;
    memcpy((void *)&fu, (void *)&u, sizeof(fu));

    {
      mpfr_set_default_prec(32);

      mpfr_t mx, my, mz;
      mpfr_inits(mx, my, mz, NULL);

      BFloat16 x = BFloat16(fu);

#ifdef TEST_SQRT
      {
	BFloat16 r = sqrt(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_sqrt(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.5) {
	  printf("\nbf16 sqrt\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_TRIG
      {
	BFloat16 r = sin(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_sin(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 sin\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = cos(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cos(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 cos\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = tan(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_tan(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 tan\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

#ifdef TLFLOAT_ENABLE_MPFR_SINPI
      {
	BFloat16 r = sinpi(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_sinpi(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 sinpi\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = cospi(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cospi(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 cospi\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = tanpi(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_tanpi(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 tanpi\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif
#endif

#ifdef TEST_INVTRIG
      {
	BFloat16 r = asin(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_asin(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 asin\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = acos(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_acos(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 acos\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = atan(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_atan(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 atan\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_EXP
      {
	BFloat16 r = exp(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_exp(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 exp\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = expm1(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_expm1(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 expm1\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = exp2(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_exp2(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 exp2\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = exp10(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_exp10(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 exp10\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_LOG
      {
	BFloat16 r = log(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 log\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = log1p(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log1p(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 log1p\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = log2(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log2(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 log2\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = log10(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_log10(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 log10\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_CBRT
      {
	BFloat16 r = cbrt(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cbrt(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 cbrt\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_HYP
      {
	BFloat16 r = sinh(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_sinh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 sinh\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = cosh(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_cosh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 cosh\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = tanh(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_tanh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 tanh\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = asinh(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_asinh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 asinh\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = acosh(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_acosh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 acosh\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = atanh(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_atanh(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 atanh\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_ERF
      {
	BFloat16 r = erf(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_erf(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 erf\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = erfc(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_erfc(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 erfc\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_GAMMA
      {
	BFloat16 r = tgamma(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	mpfr_gamma(mx, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 tgamma\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	  printf("ulp = %g\n", ulp);
	  cout << "NG" << endl;
	  exit(-1);
	}
      }

      {
	BFloat16 r = lgamma(x);
	ubf16 xcr = r.getUnpacked();

	mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	int msign = 0;
	mpfr_lgamma(mx, &msign, mx, GMP_RNDN);
	double c = mpfr_get_d(mx, GMP_RNDN);
	double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	if (ulp > 0.7) {
	  printf("\nbf16 lgamma\n");
	  cout << "x = " << (BFloat16)x << endl;
	  cout << "c = " << c << endl;
	  cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
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
      BFloat16 fw = trunc((double)fu);
      BFloat16 fx = BFloat16(trunc(ubf16(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("trunc u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(ubf16(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(ubf16(fx)) << endl;
	exit(-1);
      }
    }

    {
      BFloat16 fw = floor((double)fu);
      BFloat16 fx = BFloat16(floor(ubf16(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("floor u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(ubf16(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(ubf16(fx)) << endl;
	exit(-1);
      }
    }

    {
      BFloat16 fw = ceil((double)fu);
      BFloat16 fx = BFloat16(ceil(ubf16(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("ceil u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(ubf16(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(ubf16(fx)) << endl;
	exit(-1);
      }
    }

    {
      BFloat16 fw = round((double)fu);
      BFloat16 fx = BFloat16(round(ubf16(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("round u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(ubf16(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(ubf16(fx)) << endl;
	exit(-1);
      }
    }

    {
      BFloat16 fw = rint((double)fu);
      BFloat16 fx = BFloat16(rint(ubf16(fu)));
      uint16_t w, x;
      memcpy((void *)&w, (void *)&fw, sizeof(w));
      memcpy((void *)&x, (void *)&fx, sizeof(x));

      if (!(isnan(fw) && isnan(fx)) && w != x) {
	printf("rint u = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)w, (unsigned)x);
	cout << "u: " << fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << fw << " : " << to_string_d(ubf16(fw)) << endl;
	cout << "t: " << fx << " : " << to_string_d(ubf16(fx)) << endl;
	exit(-1);
      }
    }
#endif

    for(uint32_t v32=0;v32 < 0x10000;v32++) {
      uint16_t v = ((v32 << 8) | (v32 >> 8)) & 0xffff;
      BFloat16 fv;
      memcpy((void *)&fv, (void *)&v, sizeof(fv));

#ifdef TEST_INVTRIG
      {
	mpfr_set_default_prec(32);

	mpfr_t mx, my, mz;
	mpfr_inits(mx, my, mz, NULL);

	BFloat16 x = BFloat16(fu);
	BFloat16 y = BFloat16(fv);

	{
	  BFloat16 r = atan2(y, x);
	  ubf16 xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_atan2(mx, my, mx, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	  if (ulp > 0.7) {
	    printf("\nbf16 atan2\n");
	    cout << "y = " << (BFloat16)y << endl;
	    cout << "x = " << (BFloat16)x << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
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

	BFloat16 x = BFloat16(fu);
	BFloat16 y = BFloat16(fv);

	{
	  BFloat16 r = pow(x, y);
	  ubf16 xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_pow(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	  if (ulp > 0.7) {
	    printf("\nbf16 pow\n");
	    cout << "x = " << (BFloat16)x << endl;
	    cout << "y = " << (BFloat16)y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
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

	BFloat16 x = BFloat16(fu);
	BFloat16 y = BFloat16(fv);

	{
	  BFloat16 r = fmod(x, y);
	  ubf16 xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_fmod(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	  if (ulp > 0.5) {
	    printf("\nbf16 fmod\n");
	    cout << "x = " << (BFloat16)x << endl;
	    cout << "y = " << (BFloat16)y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
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

	BFloat16 x = BFloat16(fu);
	BFloat16 y = BFloat16(fv);

	{
	  BFloat16 r = remainder(x, y);
	  ubf16 xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_remainder(mx, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	  if (ulp > 0.5) {
	    printf("\nbf16 remainder\n");
	    cout << "x = " << (BFloat16)x << endl;
	    cout << "y = " << (BFloat16)y << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
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

	BFloat16 x = BFloat16(fu);
	BFloat16 y = BFloat16(fv);

	{
	  auto p = remquo(x, y);
	  BFloat16 r = p.first;
	  ubf16 xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  long int mq = 0;
	  mpfr_remquo(mx, &mq, mx, my, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	  if (ulp > 0.5 || mq != p.second) {
	    printf("\nbf16 remquo\n");
	    cout << "x = " << (BFloat16)x << endl;
	    cout << "y = " << (BFloat16)y << endl;
	    cout << "c = " << c << ", " << mq << endl;
	    cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << ", " << p.second << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	mpfr_clears(mx, my, mz, NULL);
      }
#endif

#ifdef TEST_FMA
      {
	mpfr_set_default_prec(32);

	mpfr_t mw, mx, my, mz;
	mpfr_inits(mw, mx, my, mz, NULL);

	BFloat16 x = BFloat16(fu);
	BFloat16 y = BFloat16(fv);

	BFloat16 fz[] = {
	  6e-8, -12e-8, 18e-8, -24e-8, 30e-8, -36e-8,
	  1e-5, -1e-4, 1e-3, -1e-2, 65504, -32768,
	  +0.0, -0.0, (BFloat16)+INFINITY, (BFloat16)-INFINITY
	};
	for(unsigned i=0;i<sizeof(fz)/sizeof(BFloat16);i++) {
	  BFloat16 z = BFloat16(fz[i]);

	  BFloat16 r = fma(x, y, z);
	  ubf16 xcr = r.getUnpacked();

	  mpfr_set_unpacked(mx, x.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(my, y.getUnpacked(), GMP_RNDN);
	  mpfr_set_unpacked(mz, z.getUnpacked(), GMP_RNDN);
	  mpfr_fma(mx, mx, my, mz, GMP_RNDN);
	  double c = mpfr_get_d(mx, GMP_RNDN);
	  double ulp = countULP(xcr, mx, ubf16::flt_true_min(), ubf16::flt_max());
	  if (ulp > 0.5) {
	    printf("\nbf16 fma\n");
	    cout << "x = " << (BFloat16)x << endl;
	    cout << "y = " << (BFloat16)y << endl;
	    cout << "z = " << (BFloat16)z << endl;
	    cout << "c = " << c << endl;
	    cout << "r = " << (BFloat16)r << " : " << to_string_d(BFloat16(r).getUnpacked()) << endl;
	    printf("ulp = %g\n", ulp);
	    cout << "NG" << endl;
	    exit(-1);
	  }
	}

	mpfr_clears(mw, mx, my, mz, NULL);
      }
#endif
    }
  }

  cout << endl << "OK" << endl;
  return 0;
}
