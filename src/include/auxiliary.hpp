#include <memory>
#include <cstdint>
#include <ctime>

using namespace std;
using namespace tlfloat;

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

  uint64_t nextLT(uint64_t bound) {
    if (bound == 0) return 0;

    unsigned b = sizeof(uint64_t)*8 - tlfloat::detail::clz64(bound - 1);
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
    } c = { .u = next(52) };
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

  LCG64() : LCG64(time(NULL)) {}

  uint32_t next32() {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return uint32_t(state >> 32);
  }
  uint64_t next64() {
    uint32_t u = next32();
    return u | (uint64_t(next32()) << 32);
  }
};

class CryptUtil {
public:
  template<int N>
  static tlfloat::BigUInt<N> genRand(tlfloat::BigUInt<N> bound, shared_ptr<RNG> rng) {
    tlfloat::BigUInt<N> r = 0;
    if (bound == 0) {
      rng->nextBytes((unsigned char *)&r, sizeof(r));
    } else {
      unsigned b = (bound - 1).ilogbp1();
      tlfloat::BigUInt<N> u = tlfloat::BigUInt<N>(1) << b;
      rng->nextBytes((unsigned char *)&r, (b >> 6) << 3);
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

shared_ptr<RNG> createPreferredRNG() {
  return shared_ptr<RNG>(new LCG64());
}

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
#endif
