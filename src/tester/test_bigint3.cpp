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

#define SUPPRESS_WARNINGS

#include "tlfloat/bigint.hpp"
#include "auxiliary.hpp"

using namespace std;
using namespace tlfloat;

typedef __int128_t INT128;
typedef __uint128_t UINT128;

void print(char *const buf, UINT128 u) {
  if (u == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
  char *p = buf;
  while(u != 0) {
    *p++ = "0123456789" [u % 10];
    u /= 10;
  }
  for(char *q = buf, *r = p-1;q < r;q++, r--) {
    char c = *q; *q = *r; *r = c;
  }
  *p = '\0';
}

string to_string(UINT128 value) {
  vector<char> s((1 << 7)/3 + 2);
  print(s.data(), value);
  return string(s.data());
}

string to_string(INT128 value) {
  vector<char> s((1 << 7)/3 + 2);
  print(s.data(), value >= 0 ? value : -value);
  return (value >= 0 ? "" : "-") + string(s.data());
}

ostream& operator<<(ostream &os, INT128 value) { return os << to_string(value); }
ostream& operator<<(ostream &os, UINT128 value) { return os << to_string(value); }

bool equal(__int128_t b0, BigInt<7> i0) {
  bool b = int64_t(b0 >> 64) == int64_t(i0 >> 64) &&
    int64_t(b0 & ~uint64_t(0)) == int64_t(i0 & BigUInt<7>(~uint64_t(0)));
  if (!b) {
    cerr << "int128 : " << b0 << endl;
    cerr << "bigint : " << i0 << endl;
  }
  return b;
}

bool equal(BigInt<7> i0, __int128_t b0) { return equal(b0, i0); }

bool success = true;

void e(const char *mes) {
  cerr << mes << endl;
  success = false;
}

void checks(uint64_t high0, uint64_t low0, uint64_t high1, uint64_t low1, double d) {
  union converter {
  public:
    converter() {}
    uint64_t u[2];
    BigInt<7> bi;
    __int128_t i128;
  };

  converter cnv;

  cnv.u[0] = low0; cnv.u[1] = high0;
  BigInt<7> i0 = cnv.bi;
  __int128_t b0 = cnv.i128;

  cnv.u[0] = low1; cnv.u[1] = high1;
  BigInt<7> i1 = cnv.bi;
  __int128_t b1 = cnv.i128;

  //cerr << "i0 = " << i0 << endl;
  //cerr << "i1 = " << i1 << endl;

  if (double(i0) != double(b0)) {
    cerr << i0 << endl;
    e("checks : operator double");
  }
  if (double(i1) != double(b1)) {
    cerr << i1 << endl;
    e("checks : operator double");
  }
  if (!equal(__int128_t(d), BigInt<7>(d))) {
    fprintf(stderr, "%.20g\n", d);
    e("checks : cast from double");
  }

  //

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

  if ((unsigned long long)(i0) != (unsigned long long)(b0)) e("(unsigned long long)");
  if ((unsigned long)(i0) != (unsigned long)(b0)) e("(unsigned long)");
  if ((unsigned int)(i0) != (unsigned int)(b0)) e("(unsigned int)");
  if ((unsigned short)(i0) != (unsigned short)(b0)) e("(unsigned short)");
  if ((unsigned char)(i0) != (unsigned char)(b0)) e("(unsigned char)");
  if ((bool)(i0) != (bool)(b0)) e("(bool)");

  if (!equal(BigInt<7>((long long)(i0)), __int128_t((long long)(b0)))) e("(long long)2");
  if (!equal(BigInt<7>((long)(i0)), __int128_t((long)(b0)))) e("(long)2");
  if (!equal(BigInt<7>((int)(i0)), __int128_t((int)(b0)))) e("(int)2");
  if (!equal(BigInt<7>((short)(i0)), __int128_t((short)(b0)))) e("(short)2");
  if (!equal(BigInt<7>((char)(i0)), __int128_t((char)(b0)))) e("(char)2");

  if (!equal(BigInt<7>((unsigned long long)(i0)), __int128_t((unsigned long long)(b0)))) e("(unsigned long long)2");
  if (!equal(BigInt<7>((unsigned long)(i0)), __int128_t((unsigned long)(b0)))) e("(unsigned long)2");
  if (!equal(BigInt<7>((unsigned)(i0)), __int128_t((unsigned)(b0)))) e("(unsigned)2");
  if (!equal(BigInt<7>((unsigned short)(i0)), __int128_t((unsigned short)(b0)))) e("(unsigned short)2");
  if (!equal(BigInt<7>((unsigned char)(i0)), __int128_t((unsigned char)(b0)))) e("(unsigned char)2");
  if (!equal(BigInt<7>((bool)(i0)), __int128_t((bool)(b0)))) e("(bool)2");

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
}

void checku(uint64_t high0, uint64_t low0, uint64_t high1, uint64_t low1, double d) {
  union converter {
  public:
    converter() {}
    int64_t u[2];
    BigUInt<7> bi;
    __uint128_t i128;
  };

  converter cnv;

  cnv.u[0] = low0; cnv.u[1] = high0;
  BigUInt<7> i0 = cnv.bi;
  __uint128_t b0 = cnv.i128;

  cnv.u[0] = low1; cnv.u[1] = high1;
  BigUInt<7> i1 = cnv.bi;
  __uint128_t b1 = cnv.i128;

  //cerr << "i0 = " << i0 << endl;
  //cerr << "i1 = " << i1 << endl;

  if (double(i0) != double(b0)) {
    cerr << i0 << endl;
    e("checku : operator double");
  }
  if (double(i1) != double(b1)) {
    cerr << i1 << endl;
    e("checku : operator double");
  }
  if (!equal(__uint128_t(d), BigUInt<7>(d))) {
    fprintf(stderr, "%.20g\n", d);
    e("checku : cast from double");
  }

  //

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

  if ((unsigned long long)(i0) != (unsigned long long)(b0)) e("(unsigned long long)");
  if ((unsigned long)(i0) != (unsigned long)(b0)) e("(unsigned long)");
  if ((unsigned int)(i0) != (unsigned int)(b0)) e("(unsigned int)");
  if ((unsigned short)(i0) != (unsigned short)(b0)) e("(unsigned short)");
  if ((unsigned char)(i0) != (unsigned char)(b0)) e("(unsigned char)");
  if ((bool)(i0) != (bool)(b0)) e("(bool)");

  if (!equal(BigUInt<7>((long long)(i0)), __int128_t((long long)(b0)))) e("(long long)3");
  if (!equal(BigUInt<7>((long)(i0)), __int128_t((long)(b0)))) e("(long)3");
  if (!equal(BigUInt<7>((int)(i0)), __int128_t((int)(b0)))) e("(int)3");
  if (!equal(BigUInt<7>((short)(i0)), __int128_t((short)(b0)))) e("(short)3");
  if (!equal(BigUInt<7>((char)(i0)), __int128_t((char)(b0)))) e("(char)3");

  if (!equal(BigUInt<7>((unsigned long long)(i0)), __int128_t((unsigned long long)(b0)))) e("(unsigned long long)4");
  if (!equal(BigUInt<7>((unsigned long)(i0)), __int128_t((unsigned long)(b0)))) e("(unsigned long)4");
  if (!equal(BigUInt<7>((unsigned)(i0)), __int128_t((unsigned)(b0)))) e("(unsigned)4");
  if (!equal(BigUInt<7>((unsigned short)(i0)), __int128_t((unsigned short)(b0)))) e("(unsigned short)4");
  if (!equal(BigUInt<7>((unsigned char)(i0)), __int128_t((unsigned char)(b0)))) e("(unsigned char)4");
  if (!equal(BigUInt<7>((bool)(i0)), __int128_t((bool)(b0)))) e("(bool)4");

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
}

int main(int argc, char **argv) {
  int n = 1000;

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
