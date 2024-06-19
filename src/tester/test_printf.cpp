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

static const int test_widths[] = { -1, 0, 1, 2, 3, 4, 6, 8, 12, 16, 20, 32 };
static const int test_precs[] = { 0, 1, 2, 3, 4, 8, 12, 14 };

bool xisdigit(int c) { return '0' <= c && c <= '9'; }
bool xisxdigit(int c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
int xtolower(int c) { if ('A' <= c && c <= 'Z') return c - 'A' + 'a'; else return c; }

int xstrcmp(const char *p, const char *q, int prec) {
  for(;;) {
    if (*p == '\0' && *q == '\0') return 0;
    if (xtolower(*p) == 'e' || xtolower(*p) == 'p') prec = 10000;
    if (xisxdigit(*p) && xisxdigit(*q)) {
      if (prec > 0 && *p != *q) return *p > *q ? 1 : -1;
      prec--;
    } else if (*p != *q) {
      return *p > *q ? 1 : -1;
    }
    p++; q++;
  }
}

void doTest(const char *fmt, ...) {
  static char strt[1024], strc[1024];

  va_list ap;

  va_start(ap, fmt);
  int rett = tlfloat_vsnprintf(strt, sizeof(strt), fmt, ap);
  va_end(ap);

  va_start(ap, fmt);
  int retc = vsnprintf(strc, sizeof(strc), fmt, ap);
  va_end(ap);

  if (rett != retc || strcmp(strt, strc) != 0) {
    printf("%s : c=[%s] t=[%s] retc=%d rett=%d\n", fmt, strc, strt, retc, rett);
    exit(-1);
  }
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
	      char fmt[120], corr0[120], corr1[120], test[120];
	      int lc0, lc1, lt;

	      snprintf(fmt, 100, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      lc0 = snprintf(corr0, 100, fmt, val);
	      lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

	      snprintf(fmt, 100, "%%%s%s%s%s%s_%d%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       nbits,
		       types[i]);
	      lt = tlfloat_snprintf(test, 8, fmt, T(val));
	      lt = tlfloat_snprintf(test, 100, fmt, T(val));

	      if( (lt != lc0 && lt != lc1) ||
		  (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		  (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
		       strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		exit(-1);
	      }

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];

		snprintf(fmt, 100, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		lc0 = snprintf(corr0, 100, fmt, val);
		lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

		snprintf(fmt, 100, "%%%s%s%s%s%s%d._%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, nbits, types[i]);
		lt = tlfloat_snprintf(test, 8, fmt, T(val));
		lt = tlfloat_snprintf(test, 100, fmt, T(val));

		if( (lt != lc0 && lt != lc1) ||
		    (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		    (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		  printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			 strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		  exit(-1);
		}

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s*.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		lc0 = snprintf(corr0, 100, fmt, width, val);
		lc1 = snprintf(corr1, 100, fmt, width, strtod(corr0, nullptr));

		snprintf(fmt, 100, "%%%s%s%s%s%s*._%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 nbits, types[i]);
		lt = tlfloat_snprintf(test, 8, fmt, width, T(val));
		lt = tlfloat_snprintf(test, 100, fmt, width, T(val));

		if( (lt != lc0 && lt != lc1) ||
		    (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		    (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		  printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			 strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		  exit(-1);
		}
	      }

	      for(unsigned p=0;p<sizeof(test_precs)/sizeof(int);p++) {
		int prec = test_precs[p];
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];

		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  lc0 = snprintf(corr0, 100, fmt, val);
		  lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d_%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, nbits, types[i]);
		  lt = tlfloat_snprintf(test, 8, fmt, T(val));
		  lt = tlfloat_snprintf(test, 100, fmt, T(val));

		  if( (lt != lc0 && lt != lc1) ||
		      (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		      (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		    printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			   strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		    exit(-1);
		  }

		  //

		  snprintf(fmt, 100, "%%%s%s%s%s%s*.*%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   types[i]);
		  lc0 = snprintf(corr0, 100, fmt, width, prec, val);
		  lc1 = snprintf(corr1, 100, fmt, width, prec, strtod(corr0, nullptr));

		  snprintf(fmt, 100, "%%%s%s%s%s%s*.*_%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   nbits, types[i]);
		  lt = tlfloat_snprintf(test, 8, fmt, width, prec, T(val));
		  lt = tlfloat_snprintf(test, 100, fmt, width, prec, T(val));

		  if( (lt != lc0 && lt != lc1) ||
		      (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		      (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		    printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			   strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		    exit(-1);
		  }
		}

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		lc0 = snprintf(corr0, 100, fmt, val);
		lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d_%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, nbits, types[i]);
		lt = tlfloat_snprintf(test, 8, fmt, T(val));
		lt = tlfloat_snprintf(test, 100, fmt, T(val));

		if( (lt != lc0 && lt != lc1) ||
		    (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		    (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		  printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			 strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		  exit(-1);
		}

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s.*%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		lc0 = snprintf(corr0, 100, fmt, prec, val);
		lc1 = snprintf(corr1, 100, fmt, prec, strtod(corr0, nullptr));

		snprintf(fmt, 100, "%%%s%s%s%s%s.*_%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 nbits, types[i]);
		lt = tlfloat_snprintf(test, 8, fmt, prec, T(val));
		lt = tlfloat_snprintf(test, 100, fmt, prec, T(val));

		if( (lt != lc0 && lt != lc1) ||
		    (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		    (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		  printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			 strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		  exit(-1);
		}
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
  const int base[] = { 10, 10, 16, 8 };
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], corr[120], test[120];

	      snprintf(fmt, 100, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      snprintf(corr, 100, fmt, val);

	      snprintf(fmt, 100, "%%%s%s%s%s%s_32%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      tlfloat_snprintf(test, 8, fmt, val);
	      tlfloat_snprintf(test, 100, fmt, val);

	      if(strcmp(test,corr) != 0) {
		printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		exit(-1);
	      }

	      {
		const char *tp = test;
		char *cp = test;
		if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		  printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			 (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		  exit(-1);
		}
	      }
	      {
		const char *tp = test;
		char *cp = test;
		if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		  printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			 (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		  exit(-1);
		}
	      }

	      //

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];

		snprintf(fmt, 100, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		snprintf(corr, 100, fmt, val);

		snprintf(fmt, 100, "%%%s%s%s%s%s%d._32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		tlfloat_snprintf(test, 8, fmt, val);
		tlfloat_snprintf(test, 100, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}

		{
		  const char *tp = test;
		  char *cp = test;
		  if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			   (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}
		{
		  const char *tp = test;
		  char *cp = test;
		  if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			   (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s*.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		snprintf(corr, 100, fmt, width, val);

		snprintf(fmt, 100, "%%%s%s%s%s%s*._32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		tlfloat_snprintf(test, 8, fmt, width, val);
		tlfloat_snprintf(test, 100, fmt, width, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}

		{
		  const char *tp = test;
		  char *cp = test;
		  if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			   (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}
		{
		  const char *tp = test;
		  char *cp = test;
		  if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			   (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}
	      }

	      for(int prec=4;prec<=12;prec += 1) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];

		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  snprintf(corr, 100, fmt, val);

		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d_32%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types[i]);
		  tlfloat_snprintf(test, 8, fmt, val);
		  tlfloat_snprintf(test, 100, fmt, val);

		  if(strcmp(test,corr) != 0) {
		    printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		    exit(-1);
		  }

		  {
		    const char *tp = test;
		    char *cp = test;
		    if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		      printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			     (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		      exit(-1);
		    }
		  }
		  {
		    const char *tp = test;
		    char *cp = test;
		    if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		      printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			     (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		      exit(-1);
		    }
		  }

		  //

		  snprintf(fmt, 100, "%%%s%s%s%s%s*.*%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   types[i]);
		  snprintf(corr, 100, fmt, width, prec, val);

		  snprintf(fmt, 100, "%%%s%s%s%s%s*.*_32%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    types[i]);
		  tlfloat_snprintf(test, 8, fmt, width, prec, val);
		  tlfloat_snprintf(test, 100, fmt, width, prec, val);

		  if(strcmp(test,corr) != 0) {
		    printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		    exit(-1);
		  }

		  {
		    const char *tp = test;
		    char *cp = test;
		    if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		      printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			     (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		      exit(-1);
		    }
		  }
		  {
		    const char *tp = test;
		    char *cp = test;
		    if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		      printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			     (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		      exit(-1);
		    }
		  }
		}

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		snprintf(corr, 100, fmt, val);

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d_32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		tlfloat_snprintf(test, 8, fmt, val);
		tlfloat_snprintf(test, 100, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}

		{
		  const char *tp = test;
		  char *cp = test;
		  if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			   (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}
		{
		  const char *tp = test;
		  char *cp = test;
		  if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			   (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}

		//

		snprintf(fmt, 100, "%%%s%s%s%s%s.*%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		snprintf(corr, 100, fmt, prec, val);

		snprintf(fmt, 100, "%%%s%s%s%s%s.*_32%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 types[i]);
		tlfloat_snprintf(test, 8, fmt, prec, val);
		tlfloat_snprintf(test, 100, fmt, prec, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}

		{
		  const char *tp = test;
		  char *cp = test;
		  if ((unsigned long long)BigUInt<7>(test, &tp, base[i]) != strtoull(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0U t=%llu(%d) c=%llu(%d)\n", test,
			   (unsigned long long)BigUInt<7>(test, nullptr, base[i]), int(tp-test), strtoull(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}
		{
		  const char *tp = test;
		  char *cp = test;
		  if ((long long)BigInt<7>(test, &tp, base[i]) != strtoll(test, &cp, base[i]) || tp != cp) {
		    printf("%s : 0S t=%lld(%d) c=%lld(%d)\n", test,
			   (long long)BigInt<7>(test, nullptr, base[i]), int(tp-test), strtoll(test, nullptr, base[i]), int(cp-test));
		    exit(-1);
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

void testem64(int64_t val) {
  const char *types[] = { "lld", "llu", "llx", "llo" };
  const char *types2[] = { "d", "u", "x", "o" };
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], corr[120], test[120];

	      snprintf(fmt, 90, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      snprintf(corr, 98, fmt, val);

	      snprintf(fmt, 90, "%%%s%s%s%s%s_64%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types2[i]);
	      tlfloat_snprintf(test, 8, fmt, val);
	      tlfloat_snprintf(test, 98, fmt, val);

	      if(strcmp(test,corr) != 0) {
		printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
	      }

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];
		snprintf(fmt, 90, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		snprintf(corr, 98, fmt, val);

		snprintf(fmt, 90, "%%%s%s%s%s%s%d._64%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types2[i]);
		tlfloat_snprintf(test, 8, fmt, val);
		tlfloat_snprintf(test, 98, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}
	      }

	      for(int prec=0;prec<=12;prec += 2) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];
		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  snprintf(corr, 98, fmt, val);

		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%d_64%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types2[i]);
		  tlfloat_snprintf(test, 8, fmt, val);
		  tlfloat_snprintf(test, 98, fmt, val);

		  if(strcmp(test,corr) != 0) {
		    printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		    exit(-1);
		  }
		}

		snprintf(fmt, 90, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		snprintf(corr, 98, fmt, val);

		snprintf(fmt, 90, "%%%s%s%s%s%s.%d_64%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types2[i]);
		tlfloat_snprintf(test, 8, fmt, val);
		tlfloat_snprintf(test, 98, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

void testem128(int64_t val) {
  const char *types[] = { "lld", "llu", "llx", "llo" };
  const char *types2[] = { "d", "u", "x", "o" };
  for(int i=0;i<4;i++) {
    if (i != 0 && val < 0) continue;

    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], corr[120], test[120];

	      snprintf(fmt, 90, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      snprintf(corr, 98, fmt, val);

	      snprintf(fmt, 90, "%%%s%s%s%s%s_128%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types2[i]);
	      tlfloat_snprintf(test, 8, fmt, BigInt<7>(val));
	      tlfloat_snprintf(test, 98, fmt, BigInt<7>(val));

	      if(strcmp(test,corr) != 0) {
		printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
	      }

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
	      snprintf(fmt, 90, "%%%s%s%s%s%sQ%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types2[i]);
	      tlfloat_snprintf(test, 8, fmt, __int128_t(val));
	      tlfloat_snprintf(test, 98, fmt, __int128_t(val));

	      if(strcmp(test,corr) != 0) {
		printf("%s(__int128_t) : c=[%s] t=[%s]\n", fmt, corr, test);
	      }
#endif

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];
		snprintf(fmt, 90, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		snprintf(corr, 98, fmt, val);

		snprintf(fmt, 90, "%%%s%s%s%s%s%d._128%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types2[i]);
		tlfloat_snprintf(test, 8, fmt, BigInt<7>(val));
		tlfloat_snprintf(test, 98, fmt, BigInt<7>(val));

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
		snprintf(fmt, 90, "%%%s%s%s%s%s%d.Q%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types2[i]);
		tlfloat_snprintf(test, 8, fmt, __int128_t(val));
		tlfloat_snprintf(test, 98, fmt, __int128_t(val));

		if(strcmp(test,corr) != 0) {
		  printf("%s(__int128_t) : c=[%s] t=[%s]\n", fmt, corr, test);
		}
#endif
	      }

	      for(int prec=0;prec<=12;prec += 2) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];
		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  snprintf(corr, 98, fmt, val);

		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%d_128%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types2[i]);
		  tlfloat_snprintf(test, 8, fmt, BigInt<7>(val));
		  tlfloat_snprintf(test, 98, fmt, BigInt<7>(val));

		  if(strcmp(test,corr) != 0) {
		    printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		    exit(-1);
		  }

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%dQ%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types2[i]);
		  tlfloat_snprintf(test, 8, fmt, __int128_t(val));
		  tlfloat_snprintf(test, 98, fmt, __int128_t(val));

		  if(strcmp(test,corr) != 0) {
		    printf("%s(__int128_t) : c=[%s] t=[%s]\n", fmt, corr, test);
		  }
#endif
		}

		snprintf(fmt, 90, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		snprintf(corr, 98, fmt, val);

		snprintf(fmt, 90, "%%%s%s%s%s%s.%d_128%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types2[i]);
		tlfloat_snprintf(test, 8, fmt, BigInt<7>(val));
		tlfloat_snprintf(test, 98, fmt, BigInt<7>(val));

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
		snprintf(fmt, 90, "%%%s%s%s%s%s.%dQ%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types2[i]);
		tlfloat_snprintf(test, 8, fmt, __int128_t(val));
		tlfloat_snprintf(test, 98, fmt, __int128_t(val));

		if(strcmp(test,corr) != 0) {
		  printf("%s(__int128_t) : c=[%s] t=[%s]\n", fmt, corr, test);
		}
#endif
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
  auto rng = createPreferredRNG();
  int var;
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
  doTest("head %d %Lf %LF %Le %LE %Lg %LG %La %LA %d tail",
	 123, 0.11L, 0.21L, 0.31L, 0.41L, 0.51L, 0.61L, 0.71L, 0.81L, 321);
  doTest("head %d %Lf %LF %Le %LE %Lg %LG %La %LA %d tail",
	 -123, -0.11L, -0.21L, -0.31L, -0.41L, -0.51L, -0.61L, -0.71L, -0.81L, -321);
  doTest("head %d %c %s %p %p %d tail", 123, 111, "string", NULL, &var, 321);
  doTest("head %d %c %s %p %p %d tail", -123, 111, "string", NULL, &var, -321);

  testem32(0);
  for(int i=0;i<31;i++) {
    testem32(+1 << i);
    testem32(-1 << i);
  }

  cout << "int32_t OK" << endl;

  testem64(0);
  for(int i=0;i<63;i++) {
    testem64(+1LL << i);
    testem64(-1LL << i);
  }

  cout << "int64_t OK" << endl;

  testem128(0);
  for(int i=0;i<63;i++) {
    testem128(+1LL << i);
    testem128(-1LL << i);
  }

  cout << "128bit int OK" << endl;

  testem<Float>(NAN, 5);
  testem<Float>(+0.0, 5);
  testem<Float>(-0.0, 5);
  testem<Float>(+INFINITY, 5);
  testem<Float>(-INFINITY, 5);
  testem<Float>(+1.0, 5);
  testem<Float>(-1.0, 5);

  cout << "Float OK" << endl;

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

  cout << "Double OK" << endl;

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

  for(int i=0;i<1000;i++) {
    static char str[1024];
    Quad qc = rndQ(rng);
    tlfloat_snprintf(str, 8, "%.40_128g", qc);
    tlfloat_snprintf(str, sizeof(str), "%.40_128g", qc);
    Quad qt = tlfloat_strtoq(str, nullptr);
    if (!cmpq(qc, qt, 1)) {
      printf("Quad snprintf : %s\n", str);
      tlfloat_printf("qc = %.40_128g\n", qc);
      tlfloat_printf("qt = %.40_128g\n", qt);
      exit(-1);
    }
  }

  cout << "Quad OK" << endl;

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

  for(int i=0;i<1000;i++) {
    static char str[1024];
    Octuple oc = rndo(rng);
    tlfloat_snprintf(str, 8, "%.80_256g", oc);
    tlfloat_snprintf(str, sizeof(str), "%.80_256g", oc);
    Octuple ot = tlfloat_strtoo(str, nullptr);
    if (!cmpo(oc, ot, 1)) {
      printf("Octuple snprintf : %s\n", str);
      tlfloat_printf("oc = %.80_256g\n", oc);
      tlfloat_printf("ot = %.80_256g\n", ot);
      exit(-1);
    }
  }

  cout << "Octuple OK" << endl;

  cout << "OK" << endl;

  exit(0);
}
