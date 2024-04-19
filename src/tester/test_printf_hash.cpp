#include <iostream>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cctype>

#include "tlfloat/tlfloat.hpp"
#include "tlfloat/tlfloat.h"
#include "testerutil.hpp"

using namespace tlfloat;

auto sha256 = PSHA2_256_Internal();

static const int test_widths[] = { -1, 0, 1, 2, 3, 4, 6, 8, 12, 16, 20, 32 };
static const int test_precs[] = { 0, 1, 2, 3, 4, 8, 12, 14 };

void doTest(const char *fmt, ...) {
  static char strt[1024];

  va_list ap;

  va_start(ap, fmt);
  int rett = tlfloat_vsnprintf(strt, sizeof(strt), fmt, ap);
  va_end(ap);

  sha256.append((void *)&rett, sizeof(rett));
  sha256.append((void *)strt, strlen(strt)+1);
}

template<typename T>
void testem(double val, int cmpprec=10) {
  const char *types[] = { "e", "f", "g", "a" };
  int nbits = sizeof(T) * 8;
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], test[120];
	      int lt;

	      snprintf(fmt, 100, "%%%s%s%s%s%s_%d%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       nbits,
		       types[i]);
	      lt = tlfloat_snprintf(test, 100, fmt, T(val));

	      sha256.append((void *)&lt, sizeof(lt));
	      sha256.append((void *)test, strlen(test)+1);

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];

		snprintf(fmt, 100, "%%%s%s%s%s%s%d._%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, nbits, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, T(val));

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s*._%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 nbits, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, width, T(val));

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);
	      }

	      for(unsigned p=0;p<sizeof(test_precs)/sizeof(int);p++) {
		int prec = test_precs[p];
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];

		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d_%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, nbits, types[i]);
		  lt = tlfloat_snprintf(test, 100, fmt, T(val));

		  sha256.append((void *)&lt, sizeof(lt));
		  sha256.append((void *)test, strlen(test)+1);

		  //

		  snprintf(fmt, 100, "%%%s%s%s%s%s*.*_%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   nbits, types[i]);
		  lt = tlfloat_snprintf(test, 100, fmt, width, prec, T(val));

		  sha256.append((void *)&lt, sizeof(lt));
		  sha256.append((void *)test, strlen(test)+1);
		}

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d_%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, nbits, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, T(val));

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s.*_%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 nbits, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, prec, T(val));

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);
	      }
	    }
	  }
	}
      }
    }
  }
}

void testem32(int32_t val) {
  const char *types[] = { "d", "u", "x", "o" };
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], test[120];
	      int lt;

	      snprintf(fmt, 100, "%%%s%s%s%s%s_32%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      lt = tlfloat_snprintf(test, 100, fmt, val);

	      sha256.append((void *)&lt, sizeof(lt));
	      sha256.append((void *)test, strlen(test)+1);

	      //

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];

		snprintf(fmt, 100, "%%%s%s%s%s%s%d._32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, val);

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s*._32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, width, val);

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);
	      }

	      for(int prec=4;prec<=12;prec += 1) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];

		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d_32%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types[i]);
		  lt = tlfloat_snprintf(test, 100, fmt, val);

		  sha256.append((void *)&lt, sizeof(lt));
		  sha256.append((void *)test, strlen(test)+1);

		  //

		  snprintf(fmt, 100, "%%%s%s%s%s%s*.*_32%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    types[i]);
		  lt = tlfloat_snprintf(test, 100, fmt, width, prec, val);

		  sha256.append((void *)&lt, sizeof(lt));
		  sha256.append((void *)test, strlen(test)+1);
		}

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d_32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, val);

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s.*_32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, prec, val);

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);
	      }
	    }
	  }
	}
      }
    }
  }
}

void testem64(int64_t val) {
  const char *types2[] = { "d", "u", "x", "o" };
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], test[120];
	      int lt;

	      snprintf(fmt, 90, "%%%s%s%s%s%s_64%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types2[i]);
	      lt = tlfloat_snprintf(test, 98, fmt, val);

	      sha256.append((void *)&lt, sizeof(lt));
	      sha256.append((void *)test, strlen(test)+1);

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];
		snprintf(fmt, 90, "%%%s%s%s%s%s%d._64%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types2[i]);
		lt = tlfloat_snprintf(test, 98, fmt, val);

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);
	      }

	      for(int prec=0;prec<=12;prec += 2) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];

		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%d_64%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types2[i]);
		  lt = tlfloat_snprintf(test, 98, fmt, val);

		  sha256.append((void *)&lt, sizeof(lt));
		  sha256.append((void *)test, strlen(test)+1);
		}

		snprintf(fmt, 90, "%%%s%s%s%s%s.%d_64%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types2[i]);
		lt = tlfloat_snprintf(test, 98, fmt, val);

		sha256.append((void *)&lt, sizeof(lt));
		sha256.append((void *)test, strlen(test)+1);
	      }
	    }
	  }
	}
      }
    }
  }
}

using namespace std;

int main(int argc, char **argv) {
  doTest("head %d [%*g] [%.*g] [%*.*g] %d tail", 123, 8, 100.1234567, 7, 101.1234567, 11, 9, 102.1234567, 321);
  doTest("head %d [%*d] [%.*d] [%*.*d] %d tail", 123, 8, 100, 7, 101, 11, 9, 102, 321);
  doTest("head %d [%*s] [%.*s] [%*.*s] %d tail", 123, 8, "abc", 7, "def", 11, 9, "ghi", 321);
  doTest("head %.8d %hhd %hd %d %ld %lld %jd %zd %td %.4d tail",
	 123, (signed char)1, (short int)2, (int)3, (long int)4, (long long int)5, (intmax_t)6, (size_t)7, (ptrdiff_t) 8, 321);
  doTest("head %.8d %hhd %hd %d %ld %lld %jd %zd %td %.4d tail",
	 -123, (signed char)-1, (short int)-2, (int)-3, (long int)-4, (long long int)-5, (intmax_t)-6, (size_t)7, (ptrdiff_t)-8, -321);
  doTest("head %10.8d %hhi %hi %i %li %lli %ji %zi %ti %8.5d tail",
	 123, (signed char)1, (short int)2, (int)3, (long int)4, (long long int)5, (intmax_t)6, (size_t)7, (ptrdiff_t) 8, 321);
  doTest("head %10.8d %hhi %hi %i %li %lli %ji %zi %ti %8.5d tail",
	 -123, (signed char)-1, (short int)-2, (int)-3, (long int)-4, (long long int)-5, (intmax_t)-6, (size_t)7, (ptrdiff_t)-8, -321);
  doTest("head %-10d %hhx %hx %x %lx %llx %jx %zx %tx %-10.9d tail",
	 123, (unsigned char)1, (short unsigned)2, (unsigned)3, (long unsigned)4, (long long unsigned)5, (uintmax_t)6, (size_t)7, (ptrdiff_t) 8, 321);
  doTest("head %+10d %hhX %hX %X %lX %llX %jX %zX %tX %+10.9d tail",
	 123, (unsigned char)1, (short unsigned)2, (unsigned)3, (long unsigned)4, (long long unsigned)5, (uintmax_t)6, (size_t)7, (ptrdiff_t) 8, 321);
  doTest("head %d %hhu %hu %u %lu %llu %ju %zu %tu %d tail",
	 123, (unsigned char)1, (short unsigned)2, (unsigned)3, (long unsigned)4, (long long unsigned)5, (uintmax_t)6, (size_t)7, (ptrdiff_t) 8, 321);
  doTest("head %d %hho %ho %o %lo %llo %jo %zo %to %d tail",
	 123, (unsigned char)1, (short unsigned)2, (unsigned)3, (long unsigned)4, (long long unsigned)5, (uintmax_t)6, (size_t)7, (ptrdiff_t) 8, 321);
  doTest("head %d %f %F %e %E %g %G %a %A %d tail",
	 123, 0.11, 0.21, 0.31, 0.41, 0.51, 0.61, 0.71, 0.81, 321);
  doTest("head %d %f %F %e %E %g %G %a %A %d tail",
	 -123, -0.11, -0.21, -0.31, -0.41, -0.51, -0.61, -0.71, -0.81, -321);

  testem32(0);
  for(int i=0;i<31;i++) {
    testem32(+1 << i);
    testem32(-1 << i);
  }

  testem64(0);
  for(int i=0;i<63;i++) {
    testem64(+1LL << i);
    testem64(-1LL << i);
  }

  testem<Float>(NAN, 5);
  testem<Float>(+0.0, 5);
  testem<Float>(-0.0, 5);
  testem<Float>(+INFINITY, 5);
  testem<Float>(-INFINITY, 5);
  testem<Float>(+1.0, 5);
  testem<Float>(-1.0, 5);

  testem<Double>(NAN);
  testem<Double>(+0.0);
  testem<Double>(-0.0);
  testem<Double>(+INFINITY);
  testem<Double>(-INFINITY);
  testem<Double>(+1.0);
  testem<Double>(-1.0);
  testem<Double>(+0.1);
  testem<Double>(-0.1);
  testem<Double>(+1e+12);
  testem<Double>(-1e+12);
  testem<Double>(+1.234567890123456789e+12);
  testem<Double>(-1.234567890123456789e+12);
  testem<Double>(+1e-12);
  testem<Double>(-1e-12);
  testem<Double>(+1.234567890123456789e-12);
  testem<Double>(-1.234567890123456789e-12);
  testem<Double>(+1e+120);
  testem<Double>(-1e+120);
  testem<Double>(+1.234567890123456789e+120);
  testem<Double>(-1.234567890123456789e+120);
  testem<Double>(+1e-120);
  testem<Double>(-1e-120);
  testem<Double>(+1.234567890123456789e-120);
  testem<Double>(-1.234567890123456789e-120);

  testem<Quad>(NAN);
  testem<Quad>(+0.0);
  testem<Quad>(-0.0);
  testem<Quad>(+INFINITY);
  testem<Quad>(-INFINITY);
  testem<Quad>(+1.0);
  testem<Quad>(-1.0);
  testem<Quad>(+0.1);
  testem<Quad>(-0.1);
  testem<Quad>(+1e+12);
  testem<Quad>(-1e+12);
  testem<Quad>(+1.234567890123456789e+12);
  testem<Quad>(-1.234567890123456789e+12);
  testem<Quad>(+1e-12);
  testem<Quad>(-1e-12);
  testem<Quad>(+1.234567890123456789e-12);
  testem<Quad>(-1.234567890123456789e-12);
  testem<Quad>(+1e+120);
  testem<Quad>(-1e+120);
  testem<Quad>(+1.234567890123456789e+120);
  testem<Quad>(-1.234567890123456789e+120);
  testem<Quad>(+1e-120);
  testem<Quad>(-1e-120);
  testem<Quad>(+1.234567890123456789e-120);
  testem<Quad>(-1.234567890123456789e-120);

  testem<Octuple>(NAN);
  testem<Octuple>(+0.0);
  testem<Octuple>(-0.0);
  testem<Octuple>(+INFINITY);
  testem<Octuple>(-INFINITY);
  testem<Octuple>(+1.0);
  testem<Octuple>(-1.0);
  testem<Octuple>(+0.1);
  testem<Octuple>(-0.1);
  testem<Octuple>(+1e+12);
  testem<Octuple>(-1e+12);
  testem<Octuple>(+1.234567890123456789e+12);
  testem<Octuple>(-1.234567890123456789e+12);
  testem<Octuple>(+1e-12);
  testem<Octuple>(-1e-12);
  testem<Octuple>(+1.234567890123456789e-12);
  testem<Octuple>(-1.234567890123456789e-12);
  testem<Octuple>(+1e+120);
  testem<Octuple>(-1e+120);
  testem<Octuple>(+1.234567890123456789e+120);
  testem<Octuple>(-1.234567890123456789e+120);
  testem<Octuple>(+1e-120);
  testem<Octuple>(-1e-120);
  testem<Octuple>(+1.234567890123456789e-120);
  testem<Octuple>(-1.234567890123456789e-120);

  //

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

  exit(0);
}
