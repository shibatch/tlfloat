#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <cmath>
#include <signal.h>

#define TLFLOAT_SUPPRESS_WARNINGS

#include "tlfloat/bigint.hpp"
#include "tlfloat/tlfloat.h"

#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;

bool equal(BigInt<7> i0, tlfloat_uint128_t b0) { return equal((tlfloat_int128_t)b0, i0); }
bool equal(BigUInt<7> u0, tlfloat_int128_t b0) { return equal((tlfloat_uint128_t)b0, u0); }

bool success = true;

void e(const char *mes) {
  cerr << mes << endl << endl;
  success = false;
}

void checks(uint64_t high0, uint64_t low0, uint64_t high1, uint64_t low1, double d) {
#if 0
  printf("checks 0x%016llx%016llx 0x%016llx%016llx %.16g\n",
	 (unsigned long long)high0, (unsigned long long)low0,
	 (unsigned long long)high1, (unsigned long long)low1, d);
#endif

  union converter {
  public:
    converter() {}
    uint64_t u[2];
    BigInt<7> bi;
    tlfloat_int128_t i128;
  };

  converter cnv;

  cnv.u[0] = low0; cnv.u[1] = high0;
  BigInt<7> i0 = cnv.bi;
  tlfloat_int128_t b0 = cnv.i128;

  cnv.u[0] = low1; cnv.u[1] = high1;
  BigInt<7> i1 = cnv.bi;
  tlfloat_int128_t b1 = cnv.i128;

  if (!equal(i0, b0)) e("checks : cast with union");
  if (!equal((BigInt<7>)b0, b0)) e("checks : cast BigInt<7> <- tlfloat_int128_t");
  if (!equal(i0, (tlfloat_int128_t)i0)) e("checks : cast tlfloat_int128_t <- BigInt<7>");

  if (double(i0) != double(b0)) {
    cerr << i0 << endl;
    e("checks : operator double");
  }
  if (double(i1) != double(b1)) {
    cerr << i1 << endl;
    e("checks : operator double");
  }
  if (!equal(tlfloat_int128_t(d), BigInt<7>(d))) {
    fprintf(stderr, "%.20g\n", d);
    e("checks : cast from double");
  }

  if ((tlfloat_int128_t)tlfloat_quad((tlfloat_int128_t)low0) != low0) {
    cerr << i0 << endl;
    e("checks : cast tlfloat_quad");
  }
  if ((tlfloat_int128_t)tlfloat_octuple(b0) != b0) {
    cerr << i0 << endl;
    e("checks : cast tlfloat_octuple");
  }

  //

  {
    char str[256];
    tlfloat_snprintf(str, sizeof(str), "%Qd", b0);
    if (b0 != tlfloat_strtoi128(str, NULL, 10)) e("Qd to/from string signed base 10");
    if (b0 != tlfloat_strtoi128(str, NULL,  0)) e("Qd to/from string signed base 0");
  }

  if ((i0 == i1) != (b0 == b1)) e("==");
  if ((i1 == i0) != (b1 == b0)) e("==");
  if ((i0 == i0) != (b0 == b0)) e("==");
  if ((i1 == i1) != (b1 == b1)) e("==");

  if (!equal(i0++, b0++) || !equal(i1++, b1++)) e("++ suffix");
  if (!equal(++i0, ++b0) || !equal(++i1, ++b1)) e("++ prefix");

  if (!equal(i0--, b0--) || !equal(i1--, b1--)) e("-- suffix");
  if (!equal(--i0, --b0) || !equal(--i1, --b1)) e("-- prefix");

  if (!equal(-i0, -b0) || !equal(-i1, -b1)) e("- unary");
  if (!equal(+i0, +b0) || !equal(+i1, +b1)) e("+ unary");

  if ((long long)(i0) != (long long)(b0)) e("(long long)");
  if (long(i0) != long(b0)) e("(long)");
  if (int(i0) != int(b0)) e("(int)");
  if (short(i0) != short(b0)) e("(short)");
  if (char(i0) != char(b0)) e("(short)");

  if ((unsigned long long)(i0) != (unsigned long long)(b0)) e("(unsigned long long) 1s");
  if ((unsigned long)(i0) != (unsigned long)(b0)) e("(unsigned long) 1s");
  if ((unsigned int)(i0) != (unsigned int)(b0)) e("(unsigned int) 1s");
  if ((unsigned short)(i0) != (unsigned short)(b0)) e("(unsigned short) 1s");
  if ((unsigned char)(i0) != (unsigned char)(b0)) e("(unsigned char) 1s");
  if ((bool)(i0) != (bool)(b0)) e("(bool)");

  if (!equal(BigInt<7>((long long)(i0)), tlfloat_int128_t((long long)(b0)))) e("(long long) 2s");
  if (!equal(BigInt<7>((long)(i0)), tlfloat_int128_t((long)(b0)))) e("(long) 2s");
  if (!equal(BigInt<7>((int)(i0)), tlfloat_int128_t((int)(b0)))) e("(int) 2s");
  if (!equal(BigInt<7>((short)(i0)), tlfloat_int128_t((short)(b0)))) e("(short) 2s");
  if (!equal(BigInt<7>((char)(i0)), tlfloat_int128_t((char)(b0)))) e("(char) 2s");

  if (!equal(BigInt<7>((unsigned long long)(i0)), tlfloat_int128_t((unsigned long long)(b0)))) e("(unsigned long long) 2s");
  if (!equal(BigInt<7>((unsigned long)(i0)), tlfloat_int128_t((unsigned long)(b0)))) e("(unsigned long) 2s");
  if (!equal(BigInt<7>((unsigned)(i0)), tlfloat_int128_t((unsigned)(b0)))) e("(unsigned) 2s");
  if (!equal(BigInt<7>((unsigned short)(i0)), tlfloat_int128_t((unsigned short)(b0)))) e("(unsigned short) 2s");
  if (!equal(BigInt<7>((unsigned char)(i0)), tlfloat_int128_t((unsigned char)(b0)))) e("(unsigned char) 2s");
  if (!equal(BigInt<7>((bool)(i0)), tlfloat_int128_t((bool)(b0)))) e("(bool) 2s");

  if ((i0 < i1) != (b0 < b1)) e("<");
  if ((i1 < i0) != (b1 < b0)) e("<");
  if ((i0 < i0) != (b0 < b0)) e("<");
  if ((i1 < i1) != (b1 < b1)) e("<");

  if (!equal(i0 + i1, b0 + b1)) e("+");
  if (!equal(i1 + i0, b1 + b0)) e("+");

  if (!equal(i0 - i1, b0 - b1)) e("-");
  if (!equal(i1 - i0, b1 - b0)) e("-");

  if (!equal(i0 * i1, b0 * b1)) e("*");
  if (!equal(i1 * i0, b1 * b0)) e("*");

  if (!equal(i0 / i1, b0 / b1)) e("/");
  if (!equal(i1 / i0, b1 / b0)) e("/");

  if (!equal(i0 % i1, b0 % b1)) e("%");
  if (!equal(i1 % i0, b1 % b0)) e("%");

  if (!equal(i0 << 3, b0 << 3)) e("<<");
  if (!equal((i0 <<= 3), (b0 <<= 3))) e("<<=");

  if (!equal(i0 >> 3, b0 >> 3)) e(">>");
  if (!equal((i0 >>= 3), (b0 >>= 3))) e(">>=");

  if (!equal(i0 += 3, b0 += 3)) e("+=");
  if (!equal(i0 -= 3ULL, b0 -= 3ULL)) e("-=");
  if (!equal(i0 *= 3, b0 *= 3)) e("*=");
  if (!equal(i0 /= 3, b0 /= 3)) e("/=");

  if (!equal(i1 += i0, b1 += b0)) e("+=");
  if (!equal(i1 -= i0, b1 -= tlfloat_int128_t_(b0))) e("-=");
  if (!equal(i1 *= i0, b1 *= tlfloat_uint128_t(b0))) e("*=");
  if (!equal(i1 /= i0, b1 /= b0)) e("/=");
}

void checku(uint64_t high0, uint64_t low0, uint64_t high1, uint64_t low1, double d) {
#if 0
  printf("checku 0x%016llx%016llx 0x%016llx%016llx %.16g\n",
	 (unsigned long long)high0, (unsigned long long)low0,
	 (unsigned long long)high1, (unsigned long long)low1, d);
#endif

  union converter {
  public:
    converter() {}
    int64_t u[2];
    BigUInt<7> bi;
    tlfloat_uint128_t i128;
  };

  converter cnv;

  cnv.u[0] = low0; cnv.u[1] = high0;
  BigUInt<7> i0 = cnv.bi;
  tlfloat_uint128_t b0 = cnv.i128;

  cnv.u[0] = low1; cnv.u[1] = high1;
  BigUInt<7> i1 = cnv.bi;
  tlfloat_uint128_t b1 = cnv.i128;

  if (!equal(i0, b0)) e("checku : cast with union");
  if (!equal((BigUInt<7>)b0, b0)) e("checku : cast BigUInt<7> <- tlfloat_uint128_t");
  if (!equal(i0, (tlfloat_uint128_t)i0)) e("checku : cast tlfloat_uint128_t <- BigUInt<7>");

  if (double(i0) != double(b0)) {
    cerr << i0 << endl;
    e("checku : operator double");
  }
  if (double(i1) != double(b1)) {
    cerr << i1 << endl;
    e("checku : operator double");
  }
  if (!equal(tlfloat_uint128_t(d), BigUInt<7>(d))) {
    fprintf(stderr, "%.20g\n", d);
    e("checku : cast from double");
  }

  if ((tlfloat_uint128_t)tlfloat_quad((tlfloat_uint128_t)low0) != low0) {
    cerr << i0 << endl;
    e("checku : cast tlfloat_quad");
  }
  if ((tlfloat_uint128_t)tlfloat_octuple(b0) != b0) {
    cerr << i0 << endl;
    e("checku : cast tlfloat_octuple");
  }

  //

  {
    char str[256];

    tlfloat_snprintf(str, sizeof(str), "%Qu", b0);
    if (b0 != tlfloat_strtou128(str, NULL, 10)) e("Qu to/from string unsigned base 10");
    if (b0 != tlfloat_strtou128(str, NULL,  0)) e("Qu to/from string unsigned base 10");

    tlfloat_snprintf(str, sizeof(str), "%Qo", b0);
    if (b0 != tlfloat_strtou128(str, NULL,  8)) e("Qo to/from string unsigned base 8");
    tlfloat_snprintf(str, sizeof(str), "0%Qo", b0);
    if (b0 != tlfloat_strtou128(str, NULL,  0)) e("Qo to/from string unsigned base 0");

    tlfloat_snprintf(str, sizeof(str), "%Qx", b0);
    if (b0 != tlfloat_strtou128(str, NULL, 16)) e("Qx to/from string unsigned base 16");
    tlfloat_snprintf(str, sizeof(str), "0x%Qx", b0);
    if (b0 != tlfloat_strtou128(str, NULL,  0)) e("Qx to/from string unsigned base 0");
  }

  if ((i0 == i1) != (b0 == b1)) e("==");
  if ((i1 == i0) != (b1 == b0)) e("==");
  if ((i0 == i0) != (b0 == b0)) e("==");
  if ((i1 == i1) != (b1 == b1)) e("==");

  if (!equal(i0++, b0++) || !equal(i1++, b1++)) e("++ suffix");
  if (!equal(++i0, ++b0) || !equal(++i1, ++b1)) e("++ prefix");

  if (!equal(i0--, b0--) || !equal(i1--, b1--)) e("-- suffix");
  if (!equal(--i0, --b0) || !equal(--i1, --b1)) e("-- prefix");

  if (!equal(-i0, -b0) || !equal(-i1, -b1)) e("- unary");
  if (!equal(+i0, +b0) || !equal(+i1, +b1)) e("+ unary");

  if ((long long)(i0) != (long long)(b0)) e("(long long)");
  if (long(i0) != long(b0)) e("(long)");
  if (int(i0) != int(b0)) e("(int)");
  if (short(i0) != short(b0)) e("(short)");
  if (char(i0) != char(b0)) e("(short)");

  if ((unsigned long long)(i0) != (unsigned long long)(b0)) e("(unsigned long long) 1u");
  if ((unsigned long)(i0) != (unsigned long)(b0)) e("(unsigned long) 1u");
  if ((unsigned int)(i0) != (unsigned int)(b0)) e("(unsigned int) 1u");
  if ((unsigned short)(i0) != (unsigned short)(b0)) e("(unsigned short) 1u");
  if ((unsigned char)(i0) != (unsigned char)(b0)) e("(unsigned char) 1u");
  if ((bool)(i0) != (bool)(b0)) e("(bool)");

  if (!equal(BigUInt<7>((long long)(i0)), tlfloat_int128_t((long long)(b0)))) e("(long long)3u");
  if (!equal(BigUInt<7>((long)(i0)), tlfloat_int128_t((long)(b0)))) e("(long)3u");
  if (!equal(BigUInt<7>((int)(i0)), tlfloat_int128_t((int)(b0)))) e("(int)3u");
  if (!equal(BigUInt<7>((short)(i0)), tlfloat_int128_t((short)(b0)))) e("(short)3u");
  if (!equal(BigUInt<7>((char)(i0)), tlfloat_int128_t((char)(b0)))) e("(char)3u");

  if (!equal(BigUInt<7>((unsigned long long)(i0)), tlfloat_uint128_t((unsigned long long)(b0)))) e("(unsigned long long)4u");
  if (!equal(BigUInt<7>((unsigned long)(i0)), tlfloat_uint128_t((unsigned long)(b0)))) e("(unsigned long)4u");
  if (!equal(BigUInt<7>((unsigned)(i0)), tlfloat_uint128_t((unsigned)(b0)))) e("(unsigned)4u");
  if (!equal(BigUInt<7>((unsigned short)(i0)), tlfloat_uint128_t((unsigned short)(b0)))) e("(unsigned short)4u");
  if (!equal(BigUInt<7>((unsigned char)(i0)), tlfloat_uint128_t((unsigned char)(b0)))) e("(unsigned char)4u");
  if (!equal(BigUInt<7>((bool)(i0)), tlfloat_int128_t((bool)(b0)))) e("(bool)4u");

  if ((i0 < i1) != (b0 < b1)) e("<");
  if ((i1 < i0) != (b1 < b0)) e("<");
  if ((i0 < i0) != (b0 < b0)) e("<");
  if ((i1 < i1) != (b1 < b1)) e("<");

  if (!equal(i0 + i1, b0 + b1)) e("+");
  if (!equal(i1 + i0, b1 + b0)) e("+");

  if (!equal(i0 - i1, b0 - b1)) e("-");
  if (!equal(i1 - i0, b1 - b0)) e("-");

  if (!equal(i0 * i1, b0 * b1)) e("*");
  if (!equal(i1 * i0, b1 * b0)) e("*");

  if (!equal(i0 / i1, b0 / b1)) e("/");
  if (!equal(i1 / i0, b1 / b0)) e("/");

  if (!equal(i0 % i1, b0 % b1)) e("%");
  if (!equal(i1 % i0, b1 % b0)) e("%");

  if (!equal(i0 << 3, b0 << 3)) e("<<");
  if (!equal((i0 <<= 3), (b0 <<= 3))) e("<<=");

  if (!equal(i0 >> 3, b0 >> 3)) e(">>");
  if (!equal((i0 >>= 3), (b0 >>= 3))) e(">>=");

  if (!equal(i0 += 3, b0 += 3)) e("+=");
  if (!equal(i0 -= 3LL, b0 -= 3LL)) e("-=");
  if (!equal(i0 *= 3, b0 *= 3)) e("*=");
  if (!equal(i0 /= 3, b0 /= 3)) e("/=");

  if (!equal(i1 += i0, b1 += b0)) e("+=");
  if (!equal(i1 -= i0, b1 -= tlfloat_uint128_t_(b0))) e("-=");
  if (!equal(i1 *= i0, b1 *= tlfloat_int128_t(b0))) e("*=");
  if (!equal(i1 /= i0, b1 /= b0)) e("/=");
}

int main(int argc, char **argv) {
  int n = 10000;

  if (argc != 1) n = atoi(argv[1]);

  shared_ptr<RNG> rng = createPreferredRNG();

  for(int i=0;i<n;i++) {
    double d0 = ldexp(rng->nextDouble(), rng->nextLT(256)-128);
    checku(rng->next64(), rng->next64(), rng->next64(), rng->next64(), d0);

    d0 = rng->nextBool() ? -d0 : d0;
    checks(rng->next64(), rng->next64(), rng->next64(), rng->next64(), d0);
  }

  if (!success) {
    cout << "NG" << endl;
    return -1;
  }

  cout << "OK" << endl;

  return 0;
}
