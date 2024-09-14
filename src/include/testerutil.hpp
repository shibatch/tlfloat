#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <ctime>

#include "suppress.hpp"

#include <detect.h>

using namespace std;
using namespace tlfloat;

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884
#endif

class RNG {
  uint64_t res0 = 0, res1 = 0;
  uint32_t nBitsInRes = 0;
public:
  virtual uint64_t next64() = 0;
  virtual uint32_t next32() { return (uint32_t)next(32); }

  uint64_t next(uint32_t bits) {
    if (bits == 64) return next64();
    if (bits > nBitsInRes) {
      uint64_t u = next64();
      res0 |= u << nBitsInRes;
      res1 = nBitsInRes == 0 ? 0 : (u >> (64 - nBitsInRes));
      nBitsInRes += 64;
    }
    uint64_t ret = res0 & ((uint64_t(1) << bits) - 1);
    res0 >>= bits;
    res0 |= res1 << (64 - bits);
    res1 >>= bits;
    nBitsInRes -= bits;
    return ret;
  }

  void nextBytes(unsigned char *dst, size_t len) {
    while(len >= 8) {
      uint64_t u = next64();
      for(int i=0;i<8;i++) dst[i] = (u >> (i * 8)) & 0xff;
      dst += 8;
      len -= 8;
    }
    uint64_t u = next(uint32_t(len * 8));
    for(int i=0;i<(int)len;i++) dst[i] = (u >> (i * 8)) & 0xff;
  }

#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
  void nextBytesW(unsigned char *dst, size_t len) { nextBytes(dst, len); }
#else
  void nextBytesW(unsigned char * const dst, const size_t len_) {
    int len = len_, index = 0;
    while(len >= 8) {
      uint64_t u = next64();
      for(int i=0;i<8;i++) dst[len_-1-i-index] = (u >> (i * 8)) & 0xff;
      index += 8;
      len -= 8;
    }
    uint64_t u = next(uint32_t(len * 8));
    for(int i=0;i<(int)len;i++) dst[len_-1-i-index] = (u >> (i * 8)) & 0xff;
  }
#endif

  unsigned clz64(uint64_t u) {
    unsigned z = 0;
    if (u & 0xffffffff00000000ULL) u >>= 32; else z += 32;
    if (u & 0x00000000ffff0000ULL) u >>= 16; else z += 16;
    if (u & 0x000000000000ff00ULL) u >>=  8; else z +=  8;
    if (u & 0x00000000000000f0ULL) u >>=  4; else z +=  4;
    if (u & 0x000000000000000cULL) u >>=  2; else z +=  2;
    if (u & 0x0000000000000002ULL) u >>=  1; else z +=  1;
    if (!u) z++;
    return z;
  }

  uint64_t nextLT(uint64_t bound) {
    if (bound == 0) return 0;

    unsigned b = sizeof(uint64_t)*8 - clz64(bound - 1);
    uint64_t r = next(b), u = uint64_t(1) << b;

    while(r >= bound) {
      r -= bound;
      u -= bound;
      while(u < bound) {
	r = (r << 1) | next(1);
	u *= 2;
      }
    }
    return r;
  }

  double nextDouble() {
    union {
      uint64_t u;
      double d;
    } c;
    c.u = next(52);
    c.u |= 0x3fe0000000000000ULL;
    return c.d;
  }
  bool nextBool() { return next(1); }
  virtual ~RNG() {}
};

class LCG64 : public RNG {
  uint64_t state = 1;
public:
  LCG64(uint64_t seed) {
    state = seed;
    for(int i=0;i<10;i++) next32();
  }

  LCG64() {
    state = chrono::high_resolution_clock::now().time_since_epoch().count();
    for(int i=0;i<10;i++) next32();
  }

  uint32_t next32() {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return uint32_t(state >> 32);
  }
  uint64_t next64() {
    uint32_t u = next32();
    return u | (uint64_t(next32()) << 32);
  }
};

class TLCG64 : public RNG {
  uint64_t state = 1;
public:
  TLCG64() {
    state = chrono::high_resolution_clock::now().time_since_epoch().count();
    for(int i=0;i<10;i++) next32();
  }

  uint32_t next32() {
    uint64_t t = chrono::high_resolution_clock::now().time_since_epoch().count();
    state = state * 6364136223846793005ULL + (t << 1) + 1;
    return uint32_t(state >> 32);
  }
  uint64_t next64() {
    uint32_t u = next32();
    return u | (uint64_t(next32()) << 32);
  }
};

shared_ptr<RNG> createPreferredRNG() {
  return shared_ptr<RNG>(new TLCG64());
}

//

class PSHA2_256_Internal {
  // https://github.com/983/SHA-256
  // This is public domain implementation of SHA256
public:
  static inline uint32_t rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
  }

  static inline uint32_t step1(uint32_t e, uint32_t f, uint32_t g) {
    return (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ ((~ e) & g));
  }

  static inline uint32_t step2(uint32_t a, uint32_t b, uint32_t c) {
    return (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
  }

  static inline void update_w(uint32_t *w, int i, const uint8_t *buffer) {
    int j;
    for(j = 0;j < 16;j++) {
      if (i < 16) {
	w[j] =
	  ((uint32_t)buffer[0] << 24) |
	  ((uint32_t)buffer[1] << 16) |
	  ((uint32_t)buffer[2] <<  8) |
	  ((uint32_t)buffer[3]);
	buffer += 4;
      } else {
	uint32_t a = w[(j + 1) & 15];
	uint32_t b = w[(j + 14) & 15];
	uint32_t s0 = (rotr(a,  7) ^ rotr(a, 18) ^ (a >>  3));
	uint32_t s1 = (rotr(b, 17) ^ rotr(b, 19) ^ (b >> 10));
	w[j] += w[(j + 9) & 15] + s0 + s1;
      }
    }
  }

  uint32_t state[8];
  uint64_t n_bits;
  uint8_t buffer_counter;
  uint8_t buffer[64];

  PSHA2_256_Internal() {
    state[0] = 0x6a09e667;
    state[1] = 0xbb67ae85;
    state[2] = 0x3c6ef372;
    state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
    state[5] = 0x9b05688c;
    state[6] = 0x1f83d9ab;
    state[7] = 0x5be0cd19;
    n_bits = 0;
    buffer_counter = 0;
    for(int i=0;i<64;i++) buffer[i] = 0;
  }

  void block() {
    static const uint32_t k[] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
      0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
      0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
      0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
      0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
      0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
      0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
      0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
      0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
      0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];
    uint32_t f = state[5];
    uint32_t g = state[6];
    uint32_t h = state[7];

    uint32_t w[16];

    for(int i = 0;i < 64;i += 16) {
      update_w(w, i, buffer);

#if defined(__clang__)
#pragma clang loop unroll(full)
#endif
      for(int j = 0;j < 16;j += 4) {
	uint32_t temp;
	temp = h + step1(e, f, g) + k[i + j + 0] + w[j + 0];
	h = temp + d;
	d = temp + step2(a, b, c);
	temp = g + step1(h, e, f) + k[i + j + 1] + w[j + 1];
	g = temp + c;
	c = temp + step2(d, a, b);
	temp = f + step1(g, h, e) + k[i + j + 2] + w[j + 2];
	f = temp + b;
	b = temp + step2(c, d, a);
	temp = e + step1(f, g, h) + k[i + j + 3] + w[j + 3];
	e = temp + a;
	a = temp + step2(b, c, d);
      }
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
  }

  void append_byte(uint8_t byte) {
    buffer[buffer_counter++] = byte;
    n_bits += 8;

    if (buffer_counter == 64) {
      buffer_counter = 0;
      block();
    }
  }

  void append(const void *src, size_t n_bytes) {
    for(size_t i = 0;i < n_bytes;i++) {
      append_byte(((const uint8_t*)src)[i]);
    }
  }

  void appendWord(const void *src, size_t n_bytes) {
#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
    for(size_t i = 0;i < n_bytes;i++) {
      append_byte(((const uint8_t*)src)[i]);
    }
#else
    for(int i = int(n_bytes)-1;i >= 0;i--) {
      append_byte(((const uint8_t*)src)[i]);
    }
#endif
  }

  void finalize() {
    uint64_t nb = n_bits;

    append_byte(0x80);

    while(buffer_counter != 64 - 8) {
      append_byte(0);
    }

    for(int i = 7;i >= 0;i--) {
      uint8_t byte = (nb >> 8 * i) & 0xff;
      append_byte(byte);
    }
  }

  void finalize_bytes(void *dst_bytes32) {
    uint8_t *ptr = (uint8_t*)dst_bytes32;
    finalize();

    for(int i = 0;i < 8;i++) {
      for(int j = 3;j >= 0;j--) {
	*ptr++ = (state[i] >> j * 8) & 0xff;
      }
    }
  }
};

//

#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
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
  s.data()[s.size()-1] = '\0';
  return (value >= 0 ? "" : "-") + string(s.data());
}

ostream& operator<<(ostream &os, INT128 value) { return os << ::to_string(value); }
ostream& operator<<(ostream &os, UINT128 value) { return os << ::to_string(value); }
#endif

//

#ifdef __BIGINT_HPP_INCLUDED__
class CryptUtil {
public:
  template<int N>
  static tlfloat::BigUInt<N> genRand(tlfloat::BigUInt<N> bound, shared_ptr<RNG> rng) {
    tlfloat::BigUInt<N> r = 0;
    if (bound == 0) {
      rng->nextBytesW((unsigned char *)&r, sizeof(r));
    } else {
      unsigned b = (bound - 1).ilogbp1();
      tlfloat::BigUInt<N> u = tlfloat::BigUInt<N>(1) << b;
#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
      rng->nextBytesW((unsigned char *)&r, (b >> 6) << 3);
#else
      rng->nextBytesW((unsigned char *)&r + sizeof(r) - ((b >> 6) << 3), (b >> 6) << 3);
#endif
      r.setWord(b >> 6, rng->next(b & ((1 << 6)-1)));

      while(r >= bound) {
	r -= bound;
	u -= bound;
	while(u < bound) {
	  r <<= 1;
	  r.setWord(0, r.getWord(0) | rng->next(1));
	  u += u;
	}
      }
    }
    return r;
  }

  template<int N>
  static bool millerTest(shared_ptr<RNG> rng, unsigned s, tlfloat::BigUInt<N> d, const tlfloat::BigUInt<N>& n, const tlfloat::BigUInt<N>& recn) {
    tlfloat::BigUInt<N> a = 2 + CryptUtil::genRand<N>(n - 4, rng);
    tlfloat::BigUInt<N> x = a.pow(d, n, recn);
    if (x == 1 || x == n - 1) return true;

    for(unsigned i=0;i<s;i++) {
      x = (x * x).mod(n, recn);
      if (x == 1) return false;
      if (x == n-1) return true;
    }
    return false;
  }

  template<int N>
  static bool checkPrimality(const tlfloat::BigUInt<N>& bu, shared_ptr<RNG> rng, unsigned nloop=64) {
    if (bu == 0 || bu == 1 || bu == 4) return false;
    if (bu == 2 || bu == 3) return true;
 
    tlfloat::BigUInt<N+1> n2 = bu, d = n2 - 1, recn = n2.reciprocal();
    unsigned s = 0;
    while((d.getWord(0) & 1) == 0) { d >>= 1; s++; }
 
    for (unsigned i = 0;i < nloop;i++) if (!CryptUtil::millerTest<N+1>(rng, s, d, n2, recn)) return false;
    return true;
  }

  template<int N>
  static tlfloat::BigUInt<N> genPrime(tlfloat::BigUInt<N> bound, shared_ptr<RNG> rng) {
    tlfloat::BigUInt<N> u = genRand(bound, rng);
    u.setWord(0, u.getWord(0) | 1);
    for(;;) {
      if (checkPrimality(u, rng)) break;
      u += 2;
    }
    return u;
  }
};

#ifdef __TLFLOAT_H_INCLUDED__
#ifdef TLFLOAT_INT128_IS_STRUCT
ostream& operator<<(ostream &os, tlfloat_int128_t value) { return os << tlfloat::to_string(value); }
ostream& operator<<(ostream &os, tlfloat_uint128_t value) { return os << tlfloat::to_string(value); }
#endif

bool equal(tlfloat_int128_t b0, BigInt<7> i0) {
  bool b = int64_t(b0 >> 64) == int64_t(i0 >> 64) &&
    int64_t(b0 & (tlfloat_int128_t)~uint64_t(0)) == int64_t(i0 & BigInt<7>(~uint64_t(0)));
  if (!b) {
    cerr << "tlfloat_int128 : " << b0 << endl;
    cerr << "BigInt<7>      : " << i0 << endl;
  }
  return b;
}

bool equal(tlfloat_uint128_t b0, BigUInt<7> u0) {
  bool b = uint64_t(b0 >> 64) == uint64_t(u0 >> 64) &&
    uint64_t(b0 & ~uint64_t(0)) == uint64_t(u0 & BigUInt<7>(~uint64_t(0)));
  if (!b) {
    cerr << "tlfloat_uint128 : " << b0 << endl;
    cerr << "BigUInt<7>      : " << u0 << endl;
  }
  return b;
}

bool equal(BigInt<7> i0, tlfloat_int128_t b0) { return equal(b0, i0); }
bool equal(BigUInt<7> u0, tlfloat_uint128_t b0) { return equal(b0, u0); }
#elif defined(TLFLOAT_COMPILER_SUPPORTS_INT128)
bool equal(__int128_t b0, BigInt<7> i0) {
  bool b = int64_t(b0 >> 64) == int64_t(i0 >> 64) &&
    int64_t(b0 & ~uint64_t(0)) == int64_t(i0 & BigInt<7>(~uint64_t(0)));
  if (!b) {
    cerr << "int128    : " << b0 << endl;
    cerr << "BigInt<7> : " << i0 << endl;
  }
  return b;
}

bool equal(__uint128_t b0, BigUInt<7> u0) {
  bool b = uint64_t(b0 >> 64) == uint64_t(u0 >> 64) &&
    uint64_t(b0 & ~uint64_t(0)) == uint64_t(u0 & BigUInt<7>(~uint64_t(0)));
  if (!b) {
    cerr << "uint128    : " << b0 << endl;
    cerr << "BigUInt<7> : " << u0 << endl;
  }
  return b;
}

bool equal(BigInt<7> i0, __int128_t b0) { return equal(b0, i0); }
bool equal(BigUInt<7> u0, __uint128_t b0) { return equal(b0, u0); }
#endif

#endif // #ifdef __BIGINT_HPP_INCLUDED__

//

#ifdef QUADMATH_H
#define ENABLE_QUAD
typedef __float128 quad;
#define M_PIq_ M_PIq

std::ostream& operator<<(std::ostream &os, const __float128& f) {
  char buf[128];
  quadmath_snprintf(buf, sizeof(buf), "%.32Qg", f);
  return os << std::string(buf);
}
#endif

#ifdef TLFLOAT_LONGDOUBLE_IS_FLOAT128
#define ENABLE_QUAD
typedef long double quad;
#define fabsq fabsl
#define fmaq fmal
#define sqrtq sqrtl
#define ilogbq ilogbl
#define finiteq finitel
#define isnanq isnanl
#define isinfq isinfl
#define truncq truncl
#define floorq floorl
#define ceilq ceill
#define roundq roundl
#define rintq rintl
#define M_PIq_ M_PIl
#endif

#ifdef _MSC_VER
#define finite_ _finite
#endif

#ifndef finite_
#define finite_ isfinite
#endif

#ifdef __TLFLOAT_HPP_INCLUDED__
typedef tlfloat::detail::UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;
typedef tlfloat::detail::UnpackedFloat<uint64_t, tlfloat::BigUInt<7>, 11, 52> udouble;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 15, 112> uquad;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<8>, tlfloat::BigUInt<9>, 19, 236> uoctuple;

typedef tlfloat::detail::UnpackedFloat<uint32_t, uint64_t, 0, 30> xfloat;
typedef tlfloat::detail::UnpackedFloat<uint64_t, tlfloat::BigUInt<7>, 0, 62> xdouble;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 0, 126> xquad;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<8>, tlfloat::BigUInt<9>, 0, 254> xoctuple;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<9>, tlfloat::BigUInt<10>, 0, 510> xsedecuple;

xfloat   xfpi(0x6487ed51U, 0, false, false, false, false);
xdouble  xdpi(0x6487ed5110b4611aULL, 0, false, false, false, false);
uint64_t piqa[] = {0x62633145c06e0e69, 0x6487ed5110b4611aULL};
xquad xqpi(tlfloat::BigUInt<7>(piqa), 0, false, false, false, false);
uint64_t pioa[] = {0x0105DF531D89CD91ULL, 0x948127044533E63AULL, 0x62633145C06E0E68ULL, 0x6487ED5110B4611AULL};
xoctuple xopi(tlfloat::BigUInt<8>(pioa), 0, false, false, false, false);

static const Octuple M_PIo_ = (Octuple)xopi.cast((const uoctuple *)nullptr);

#ifdef ENABLE_QUAD
static_assert(sizeof(quad) == 16, "quad precision FP not supported");
#else
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 0, 126> xquad;
const tlfloat::Quad M_PIq_ = (tlfloat::Quad)xqpi.cast((tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 15, 112> *)0);
#endif
#endif // #ifdef __TLFLOAT_HPP_INCLUDED__

#ifdef MPFR_VERSION_MAJOR
std::string to_string(const mpfr_t& fr, int digits = 51) {
  mpfr_t t;
  mpfr_inits(t, NULL);
  int sign = mpfr_signbit(fr) ? -1 : 1;
  char *s = (char *)malloc(digits + 20);
  if (mpfr_inf_p(fr)) {
    sprintf(s, "%cinf", sign < 0 ? '-' : '+');
  } else if (mpfr_nan_p(fr)) {
    sprintf(s, "nan");
  } else {
    mpfr_exp_t e;
    s[0] = sign < 0 ? '-' : '+';
    s[1] = '0';
    s[2] = '.';
    mpfr_abs(t, fr, GMP_RNDN);
    mpfr_get_str(s+3, &e, 10, digits, t, GMP_RNDN);
    int ie = e;
    char es[32];
    snprintf(es, 30, "e%c%d", ie >= 0 ? '+' : '-', ie >= 0 ? ie : -ie);
    strncat(s, es, digits+20);
  }

  mpfr_clears(t, NULL);
  std::string str = s;
  free(s);
  return str;
}

std::ostream& operator<<(std::ostream &os, const mpfr_t& fr) {
  return os << to_string(fr);
}

void mpfr_set_inttype(mpfr_t &rop, uint16_t op, const mpfr_rnd_t rnd) {
  mpfr_set_d(rop, op, GMP_RNDN);
}

template<typename inttype>
static void mpfr_set_inttype(mpfr_t &rop, inttype op, const mpfr_rnd_t rnd) {
  mpfr_set_d(rop, 0, GMP_RNDN);
  for(unsigned u=0;u<sizeof(inttype)/sizeof(uint32_t);u++) {
    double d = double(op & 0xffffffffULL);
    if (sizeof(op) > 4) op >>= 32;
    mpfr_add_d(rop, rop, ldexp(d, 32 * u), GMP_RNDN);
  }
}

template<typename Unpacked_t>
static void mpfr_set_unpacked(mpfr_t &rop, const Unpacked_t& ux, const mpfr_rnd_t rnd) {
  if (ux.isnan) {
    mpfr_set_nan(rop);
    return;
  }
  if (ux.isinf) {
    mpfr_set_inf(rop, ux.sign);
    mpfr_setsign(rop, rop, ux.sign, GMP_RNDN);
    return;
  }
  if (ux.iszero) {
    mpfr_set_zero(rop, ux.sign);
    mpfr_setsign(rop, rop, ux.sign, GMP_RNDN);
    return;
  }

  mpfr_set_inttype(rop, ux.mant, rnd);
  mpfr_set_exp(rop, ux.ilogb() + 1);
  mpfr_setsign(rop, rop, ux.sign, GMP_RNDN);
}

int mpfr_ph(mpfr_t &rop, const mpfr_t &op, int e) {
  mpfr_t pi;
  mpfr_inits(pi, NULL);

  mpfr_const_pi(pi, GMP_RNDN);
  mpfr_mul_d(pi, pi, ldexp(1, -e), GMP_RNDN);

  //

  long int q;
  mpfr_remquo(rop, &q, op, pi, GMP_RNDN);

  mpfr_clears(pi, NULL);

  return q;
}

template<typename Unpacked_t>
static int mpfr_ph(mpfr_t &rop, const Unpacked_t& ux, int e) {
  mpfr_t x;
  mpfr_inits(x, NULL);

  mpfr_set_unpacked(x, ux, GMP_RNDN);
  int q = mpfr_ph(rop, x, e);

  mpfr_clears(x, NULL);

  return q & 0xff;
}

#if defined(ENABLE_QUAD) && defined(TLFLOAT_ENABLE_MPFR_WANT_FLOAT128)
quad mpfr_sqrt(quad a) {
  mpfr_t x;
  mpfr_inits(x, NULL);

  mpfr_set_float128(x, a, GMP_RNDN);
  mpfr_sqrt(x, x, GMP_RNDN);
  quad d = mpfr_get_float128(x, GMP_RNDN);

  mpfr_clears(x, NULL);

  return d;
}
#endif

template<typename Unpacked_t, typename Unpacked2_t>
static double countULP(const Unpacked_t& ux, mpfr_t c, const Unpacked2_t& umin, const Unpacked2_t& umax, bool checkSignedZero=false) {
  const int nbmant = umin.nbmant_();
  mpfr_t fra, frb, frc, frd, frmin, frmino2, frmax;
  mpfr_inits(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);

  mpfr_set_unpacked(frmin, umin, GMP_RNDN);
  mpfr_mul_d(frmino2, frmin, 0.5, GMP_RNDN);
  mpfr_set_unpacked(frmax, umax, GMP_RNDN);

  mpfr_abs(fra, c, GMP_RNDN);
  bool ciszero = mpfr_cmp(fra, frmino2) < 0;
  bool cisinf = mpfr_cmp(fra, frmax) > 0;

  if (ciszero && !ux.iszero) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 10000;
  }
  if (checkSignedZero && ciszero && ux.iszero) {
    if (mpfr_signbit(c) != ux.sign) {
      mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
      return 10002;
    }
  }
  if (cisinf && ux.isinf && mpfr_signbit(c) == ux.sign) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 0;
  }
  if (mpfr_nan_p(c) && ux.isnan) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 0;
  }
  if (mpfr_nan_p(c) || ux.isnan) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 10001;
  }

  int e = mpfr_get_exp(c);

  mpfr_set_d(frb, 1, GMP_RNDN);
  mpfr_set_exp(frb, e - nbmant);
  mpfr_max(frb, frb, frmin, GMP_RNDN);

  mpfr_set_unpacked(frd, ux, GMP_RNDN);
  mpfr_sub(fra, frd, c, GMP_RNDN);
  mpfr_div(fra, fra, frb, GMP_RNDN);
  double u = fabs(mpfr_get_d(fra, GMP_RNDN));

  mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);

  return u;
}
#endif // #ifdef MPFR_VERSION_MAJOR

int64_t rndint(shared_ptr<RNG> rng, unsigned maxnb = 5) {
  unsigned nbits = rng->nextLT(maxnb);
  int64_t i = rng->nextLT(1 << nbits) + 1;
  if (rng->next(1)) i = -i;
  return i;
}

float rndf(shared_ptr<RNG> rng) {
  uint64_t r = rng->nextLT(1000);

  if (r == 0) {
    static float a[] = { +0.0f, -0.0f, (float)+INFINITY, (float)-INFINITY, (float)NAN,
      +1.4013e-45f, +2.8026e-45f, +4.2039e-45f, +5.60519e-45f, +7.00649e-4f,
      -1.4013e-45f, -2.8026e-45f, -4.2039e-45f, -5.60519e-45f, -7.00649e-4f,
    };
    return a[rng->nextLT(sizeof(a)/sizeof(float))];
  } else if (r < 32) {
    for(;;) {
      float f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (!isfinite(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rintf(f) * M_PI / 2;

      int32_t u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += int(rng->nextLT(5)) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(f));

      return f;
    }
  } else if (r < 64) {
    for(;;) {
      float f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (isfinite(f)) return rndint(rng) + f;
    }
  } else {
    for(;;) {
      float f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (isfinite(f)) return f;
    }
  }
}

#ifndef TLFLOAT_FP_NAN
#define TLFLOAT_FP_NAN 0
#define TLFLOAT_FP_INFINITE 1
#define TLFLOAT_FP_ZERO 2
#define TLFLOAT_FP_SUBNORMAL 3
#define TLFLOAT_FP_NORMAL 4
#endif

bool eqquo(int64_t t, long int c) {
  if (sizeof(int64_t) == sizeof(long int)) {
    return t == c || (c == (1LL << (sizeof(long int)*8-1)) && t == 0);
  } else {
    return ((t & 7) == (c & 7) && ((t < 0) == (c < 0))) || ((c & 0x7fffffff) == 0);
  }
}

bool cmpfpclass(int sl, int tl) {
  if (sl == FP_NAN && tl == TLFLOAT_FP_NAN) return true;
  if (sl == FP_INFINITE && tl == TLFLOAT_FP_INFINITE) return true;
  if (sl == FP_ZERO && tl == TLFLOAT_FP_ZERO) return true;
  if (sl == FP_SUBNORMAL && tl == TLFLOAT_FP_SUBNORMAL) return true;
  if (sl == FP_NORMAL && tl == TLFLOAT_FP_NORMAL) return true;
  return false;
}

bool cmpf(float x, float y, int t=0) {
  if (isnan(x) && isnan(y)) return true;
  uint32_t u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

  if (t == 0) return u == v;

  int d = int(int32_t(u) - int32_t(v));
  return -t <= d && d <= t;
}

double rndd(shared_ptr<RNG> rng) {
  uint64_t r = rng->nextLT(1000);

  if (r == 0) {
    static double a[] = { +0.0, -0.0, +INFINITY, -INFINITY, NAN,
      +4.94066e-324, +9.88131e-324, +1.4822e-323, +1.97626e-323, +2.47033e-323,
      -4.94066e-324, -9.88131e-324, -1.4822e-323, -1.97626e-323, -2.47033e-323,
    };
    return a[rng->nextLT(sizeof(a)/sizeof(double))];
  } else if (r < 32) {
    for(;;) {
      double f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (!isfinite(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rint(f) * M_PI / 2;

      int64_t u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += int(rng->nextLT(5)) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(f));

      return f;
    }
  } else if (r < 64) {
    for(;;) {
      double f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (isfinite(f)) return rndint(rng) + f;
    }
  } else {
    for(;;) {
      double f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (isfinite(f)) return f;
    }
  }
}

bool cmpd(double x, double y, int t=0) {
  if (isnan(x) && isnan(y)) return true;
  uint64_t u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

  if (t == 0) return u == v;

  int d = int(int64_t(u) - int64_t(v));
  return -t <= d && d <= t;
}

#ifdef __TLFLOAT_HPP_INCLUDED__
Quad rndQ(shared_ptr<RNG> rng) {
  uint64_t r = rng->nextLT(1000);

  if (r == 0) {
    Quad pm = Quad::flt_true_min(), nm = -pm;
    Quad po = 1, no = -1;
    static Quad a[] = { +0.0, -0.0, +INFINITY, -INFINITY, NAN,
      pm, nextafter(pm, po), nextafter(nextafter(pm, po), po),
      nextafter(nextafter(nextafter(pm, po), po), po), 
      nextafter(nextafter(nextafter(nextafter(pm, po), po), po), po),
      nm, nextafter(nm, no), nextafter(nextafter(nm, no), no),
      nextafter(nextafter(nextafter(nm, no), no), no), 
      nextafter(nextafter(nextafter(nextafter(nm, no), no), no), no),
    };
    return a[rng->nextLT(sizeof(a)/sizeof(Quad))];
  } else if (r < 32) {
    for(;;) {
      Quad f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (!finite(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rint(f) * M_PIo_ / 2;

      BigInt<7> u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += int(rng->nextLT(5)) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(f));

      return f;
    }
  } else if (r < 64) {
    for(;;) {
      Quad f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (finite(f)) return rndint(rng) + f;
    }
  } else {
    for(;;) {
      Quad f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (finite(f)) return f;
    }
  }
}

#ifdef ENABLE_QUAD
quad rndq(shared_ptr<RNG> rng) { return (quad)rndQ(rng); }
typedef quad quad_;

bool cmpq(quad x, quad y, int t=0) {
  if (isnanq(x) && isnanq(y)) return true;
  BigUInt<7> u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

  if (t == 0) return u == v;

  int d = int(BigInt<7>(u) - BigInt<7>(v));
  return -t <= d && d <= t;
}
#else
typedef Quad quad_;
quad_ rndq(shared_ptr<RNG> rng) { return rndQ(rng); }
#endif

bool cmpq(Quad x, Quad y, int t=0) {
  if (isnan(x) && isnan(y)) return true;
  BigUInt<7> u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

  if (t == 0) return u == v;

  int d = int(BigInt<7>(u) - BigInt<7>(v));
  return -t <= d && d <= t;
}

Octuple rndo(shared_ptr<RNG> rng) {
  uint64_t r = rng->nextLT(1000);

  if (r == 0) {
    Octuple pm = Octuple::flt_true_min(), nm = -pm;
    Octuple po = 1, no = -1;
    static Octuple a[] = { +0.0, -0.0, +INFINITY, -INFINITY, NAN,
      pm, nextafter(pm, po), nextafter(nextafter(pm, po), po),
      nextafter(nextafter(nextafter(pm, po), po), po), 
      nextafter(nextafter(nextafter(nextafter(pm, po), po), po), po),
      nm, nextafter(nm, no), nextafter(nextafter(nm, no), no),
      nextafter(nextafter(nextafter(nm, no), no), no), 
      nextafter(nextafter(nextafter(nextafter(nm, no), no), no), no),
    };
    return a[rng->nextLT(sizeof(a)/sizeof(Octuple))];
  } else if (r < 32) {
    for(;;) {
      Octuple f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (!finite(f)) continue;
      if (-1 <= f && f <= 1) continue;
      f = rint(f) * M_PIo_ / 2;

      BigInt<8> u;
      memcpy((void *)&u, (void *)&f, sizeof(u));
      u += int(rng->nextLT(5)) - 2;
      memcpy((void *)&f, (void *)&u, sizeof(f));

      return f;
    }
  } else if (r < 64) {
    for(;;) {
      Octuple f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (finite(f)) return rndint(rng) + f;
    }
  } else {
    for(;;) {
      Octuple f;
      rng->nextBytesW((unsigned char *)&f, sizeof(f));
      if (finite(f)) return f;
    }
  }
}

bool cmpo(Octuple x, Octuple y, int t=0) {
  if (isnan(x) && isnan(y)) return true;
  BigUInt<8> u, v;
  memcpy((void *)&u, (void *)&x, sizeof(u));
  memcpy((void *)&v, (void *)&y, sizeof(v));

  if (t == 0) return u == v;

  int d = int(BigInt<8>(u) - BigInt<8>(v));
  return -t <= d && d <= t;
}
#endif // #ifdef __TLFLOAT_HPP_INCLUDED__

template<typename T>
vector<T> genTestValues(unsigned n, shared_ptr<RNG> rng) {
  T values[] = {
    T("+0.0"),
    T("-0.0"),
    T::nan(),
    T::infinity(false),
    T::infinity(true),
    T::flt_true_min(false),
    nextafter(T::flt_true_min(false), 1),
    nextafter(nextafter(T::flt_true_min(false), 1), 1),
    nextafter(nextafter(nextafter(T::flt_true_min(false), 1), 1), 1),
    nextafter(nextafter(nextafter(nextafter(T::flt_true_min(false), 1), 1), 1), 1),
    T::flt_true_min(true),
    nextafter(T::flt_true_min(true), -1),
    nextafter(nextafter(T::flt_true_min(true), -1), -1),
    nextafter(nextafter(nextafter(T::flt_true_min(true), -1), -1), -1),
    nextafter(nextafter(nextafter(nextafter(T::flt_true_min(true), -1), -1), -1), -1),
    T::flt_max(false),
    nextafter(T::flt_max(false), 0),
    nextafter(nextafter(T::flt_max(false), 0), 0),
    nextafter(nextafter(nextafter(T::flt_max(false), 0), 0), 0),
    nextafter(nextafter(nextafter(nextafter(T::flt_max(false), 0), 0), 0), 0),
    T::flt_max(true),
    nextafter(T::flt_max(true), 0),
    nextafter(nextafter(T::flt_max(true), 0), 0),
    nextafter(nextafter(nextafter(T::flt_max(true), 0), 0), 0),
    nextafter(nextafter(nextafter(nextafter(T::flt_max(true), 0), 0), 0), 0),
    nextafter(nextafter(T::flt_min(false), 0), 0),
    nextafter(T::flt_min(false), 0),
    T::flt_min(false),
    nextafter(T::flt_min(false), 1),
    nextafter(nextafter(T::flt_min(false), 1), 1),
    nextafter(nextafter(T::flt_min(true), 0), 0),
    nextafter(T::flt_min(true), 0),
    T::flt_min(true),
    nextafter(T::flt_min(true), -1),
    nextafter(nextafter(T::flt_min(true), -1), -1),
    nextafter(T("+1.0"), 0),
    nextafter(nextafter(T("+1.0"), 0), 0),
    nextafter(nextafter(nextafter(T("+1.0"), 0), 0), 0),
    nextafter(T("+1.0"), 3),
    nextafter(nextafter(T("+1.0"), 3), 3),
    nextafter(nextafter(nextafter(T("+1.0"), 3), 3), 3),
    nextafter(T("+2.0"), 0),
    nextafter(nextafter(T("+2.0"), 0), 0),
    nextafter(nextafter(nextafter(T("+2.0"), 0), 0), 0),
    nextafter(T("+2.0"), 3),
    nextafter(nextafter(T("+2.0"), 3), 3),
    nextafter(nextafter(nextafter(T("+2.0"), 3), 3), 3),
    nextafter(T("-1.0"), 0),
    nextafter(nextafter(T("-1.0"), 0), 0),
    nextafter(nextafter(nextafter(T("-1.0"), 0), 0), 0),
    nextafter(T("-1.0"), -3),
    nextafter(nextafter(T("-1.0"), -3), -3),
    nextafter(nextafter(nextafter(T("-1.0"), -3), -3), -3),
    nextafter(T("-2.0"), 0),
    nextafter(nextafter(T("-2.0"), 0), 0),
    nextafter(nextafter(nextafter(T("-2.0"), 0), 0), 0),
    nextafter(T("-2.0"), -3),
    nextafter(nextafter(T("-2.0"), -3), -3),
    nextafter(nextafter(nextafter(T("-2.0"), -3), -3), -3),
    T("+0.25"),
    T("-0.25"),
    T("+0.5"),
    T("-0.5"),
    T("+1.0"),
    T("-1.0"),
    T("+1.5"),
    T("-1.5"),
    T("+2.0"),
    T("-2.0"),
    T("+2.5"),
    T("-2.5"),
    T("+3.0"),
    T("-3.0"),
    T("+4.0"),
    T("-4.0"),
    T("+100.0"),
    T("-100.0"),
    T("+100.5"),
    T("-100.5"),
    T("+101.0"),
    T("-101.0"),
    T("+102.0"),
    T("-102.0"),
    T("+103.0"),
    T("-103.0"),
    T("+1.234"),
    T("-1.234"),
    T("+1.234e+10"),
    T("-1.234e+10"),
    T("+1.234e+30"),
    T("-1.234e+30"),
    T("+1.234e-10"),
    T("-1.234e-10"),
    T("+1.234e-30"),
    T("-1.234e-30"),
    T("0x1.6ac5b262ca1ffp+849"),
    T("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170676"),
    T("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170676e+30"),
    T("2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274"),
  };

  vector<T> ret;
  for(unsigned i=0;i<sizeof(values)/sizeof(T);i++) ret.push_back(values[i]);

  while(ret.size() < n) {
    T f;
    rng->nextBytesW((unsigned char *)&f, sizeof(f));
    ret.push_back(f);
  }

  return ret;
}

#ifdef __BIGINT_HPP_INCLUDED__
struct converter128 {
  BigInt<7> bi;
#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
  __int128_t i128;
  __uint128_t u128;
#endif
#ifdef __TLFLOAT_H_INCLUDED__
  tlfloat_int128_t ti128;
  tlfloat_uint128_t tu128;
#endif

  converter128(uint64_t high, uint64_t low) {
#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
    uint64_t u[2] = { low, high };
#else
    uint64_t u[2] = { high, low };
#endif
    memcpy(&bi, u, sizeof(bi));
#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
    memcpy(&i128, u, sizeof(i128));
    memcpy(&u128, u, sizeof(u128));
#endif
#ifdef __TLFLOAT_H_INCLUDED__
    memcpy(&ti128, u, sizeof(ti128));
    memcpy(&tu128, u, sizeof(tu128));
#endif
  }
};
#endif

size_t fwrite_(const void *ptr, size_t size, size_t nmemb, FILE *fp) {
#if !defined(__BYTE_ORDER__) || (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
  return fwrite(ptr, size, nmemb, fp);
#else
  size_t z = 0;
  for(size_t m=0;m<nmemb;m++) {
    for(size_t s=0;s<size;s++) {
      if (fwrite(&((uint8_t *)ptr)[(m+1) * size - 1 - s], 1, 1, fp) == 0) return z;
      z++;
    }
  }
  return z;
#endif
}
