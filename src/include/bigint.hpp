#ifndef __BIGINT_HPP_INCLUDED__
#define __BIGINT_HPP_INCLUDED__

#include <bit>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cassert>

#ifdef SUPPRESS_WARNINGS
#include "suppress.hpp"
#endif

#define NOINLINE

#ifndef DISABLE_ARCH_OPTIMIZATION
#if defined(__GNUC__) || defined(__clang__)

#if defined(__x86_64__) && !defined(__CUDA_ARCH__)
#include <x86intrin.h>
#define ENABLE_X86INTRIN
#endif

#if defined(__SIZEOF_INT128__) && !defined(__CUDA_ARCH__)
#define ENABLE_UINT128
#endif

#if !defined(__CUDA_ARCH__)
#define ENABLE_GNUC_CLZ
#else
#define ENABLE_CUDA_UMUL64HI
#define ENABLE_CUDA_CLZ
#endif

#undef NOINLINE
#define NOINLINE __attribute__((noinline))
#endif // #if defined(__GNUC__) || defined(__clang__)

#ifdef _MSC_VER
#if !defined(__CUDA_ARCH__)

#if defined(_M_X64)
#include <intrin.h>
#define ENABLE_X86INTRIN
#endif // #if defined(_M_X64)

#define ENABLE_VCUMUL128
#define ENABLE_VCBITSCANREVERSE
#endif // #if !defined(__CUDA_ARCH__)
#endif // #ifdef _MSC_VER
#endif // #ifndef DISABLE_ARCH_OPTIMIZATION

namespace tlfloat {
  namespace detail {
    template<typename T, int N>
    struct xarray { T e[N]; };

    template<typename firsttype, typename secondtype>
    struct xpair {
      firsttype first;
      secondtype second;
      constexpr xpair(const firsttype& f, const secondtype& s) : first(f), second(s) {}
    };

    static constexpr size_t xstrlen(const char *s) {
      if (!std::is_constant_evaluated()) return strlen(s);
      const char *p;
      for(p=s;*p != '\0';p++) ;
      return p - s;
    }

#if defined(ENABLE_X86INTRIN)

#if __cplusplus < 202002L && !defined(_MSC_VER)
    static constexpr xpair<uint64_t, bool> adc64(bool cin, uint64_t lhs, uint64_t rhs, bool enable=true) {
      xpair<uint64_t, bool> ret(0, false);
      if (enable) ret.second = _addcarry_u64(cin, lhs, rhs, (long long unsigned *)&ret.first);
      return ret;
    }

    static constexpr xpair<uint64_t, bool> sbc64(bool cin, uint64_t lhs, uint64_t rhs, bool enable=true) {
      xpair<uint64_t, bool> ret(0, false);
      if (enable) ret.second = _subborrow_u64(cin, lhs, rhs, (long long unsigned *)&ret.first);
      return ret;
    }
#else
    static constexpr xpair<uint64_t, bool> adc64(bool cin, uint64_t lhs, uint64_t rhs) {
      if (!std::is_constant_evaluated()) {
	xpair<uint64_t, bool> ret(0, false);
	ret.second = _addcarry_u64(cin, lhs, rhs, (long long unsigned *)&ret.first);
	return ret;
      } else {
	uint64_t al = (lhs & 0xffffffff) + (rhs & 0xffffffff) + cin;
	lhs >>= 32; rhs >>= 32;
	uint64_t ah = (lhs & 0xffffffff) + (rhs & 0xffffffff) + ((al >> 32) != 0);
	return xpair<uint64_t, bool>((al & 0xffffffff) | (ah << 32), (ah >> 32) != 0);
      }
    }

    static constexpr xpair<uint64_t, bool> sbc64(bool cin, uint64_t lhs, uint64_t rhs) {
      if (!std::is_constant_evaluated()) {
	xpair<uint64_t, bool> ret(0, false);
	ret.second = _subborrow_u64(cin, lhs, rhs, (long long unsigned *)&ret.first);
	return ret;
      } else {
	uint64_t al = (lhs & 0xffffffff) - (rhs & 0xffffffff) - cin;
	lhs >>= 32; rhs >>= 32;
	uint64_t ah = (lhs & 0xffffffff) - (rhs & 0xffffffff) - ((al >> 32) != 0);
	return xpair<uint64_t, bool>((al & 0xffffffff) | (ah << 32), (ah >> 32) != 0);
      }
    }
#endif // #if __cplusplus < 202002L

#elif defined(ENABLE_UINT128)
    static constexpr xpair<uint64_t, bool> adc64(bool cin, uint64_t lhs, uint64_t rhs) {
      __uint128_t a = __uint128_t(lhs) + rhs + cin;
      return xpair<uint64_t, bool>(uint64_t(a), (a >> 64) != 0);
    }

    static constexpr xpair<uint64_t, bool> sbc64(bool cin, uint64_t lhs, uint64_t rhs) {
      __uint128_t a = __uint128_t(lhs) - rhs - cin;
      return xpair<uint64_t, bool>(uint64_t(a), (a >> 64) != 0);
    }
#else
    static constexpr xpair<uint64_t, bool> adc64(bool cin, uint64_t lhs, uint64_t rhs) {
      uint64_t al = (lhs & 0xffffffff) + (rhs & 0xffffffff) + cin;
      lhs >>= 32; rhs >>= 32;
      uint64_t ah = (lhs & 0xffffffff) + (rhs & 0xffffffff) + ((al >> 32) != 0);
      return xpair<uint64_t, bool>((al & 0xffffffff) | (ah << 32), (ah >> 32) != 0);
    }

    static constexpr xpair<uint64_t, bool> sbc64(bool cin, uint64_t lhs, uint64_t rhs) {
      uint64_t al = (lhs & 0xffffffff) - (rhs & 0xffffffff) - cin;
      lhs >>= 32; rhs >>= 32;
      uint64_t ah = (lhs & 0xffffffff) - (rhs & 0xffffffff) - ((al >> 32) != 0);
      return xpair<uint64_t, bool>((al & 0xffffffff) | (ah << 32), (ah >> 32) != 0);
    }
#endif

#ifdef ENABLE_VCUMUL128
    static constexpr xpair<uint64_t, uint64_t> mul128(uint64_t lhs, uint64_t rhs) {
      if (!std::is_constant_evaluated()) {
	xpair<uint64_t, uint64_t> ret(0, 0);
	ret.second = _umul128(lhs, rhs, &ret.first);
	return ret;
      }
      uint64_t al = lhs & 0xffffffff, ah = lhs >> 32;
      uint64_t bl = rhs & 0xffffffff, bh = rhs >> 32;
      uint64_t m = ah * bl + ((al * bl) >> 32) + ((al * bh) & 0xffffffff);
      return xpair<uint64_t, uint64_t> (ah * bh + (m >> 32) + ((al * bh) >> 32), lhs * rhs);
    }
#elif defined(ENABLE_UINT128)
    static constexpr xpair<uint64_t, uint64_t> mul128(uint64_t lhs, uint64_t rhs) {
      __uint128_t m = lhs * __uint128_t(rhs);
      return xpair<uint64_t, uint64_t>(uint64_t(m >> 64), uint64_t(m));
    }
#elif defined(ENABLE_CUDA_UMUL64HI)
    static constexpr xpair<uint64_t, uint64_t> mul128(uint64_t lhs, uint64_t rhs) {
      if (!std::is_constant_evaluated()) {
	return xpair<uint64_t, uint64_t>(__umul64hi(lhs, rhs), lhs * rhs);
      }
      uint64_t al = lhs & 0xffffffff, ah = lhs >> 32;
      uint64_t bl = rhs & 0xffffffff, bh = rhs >> 32;
      uint64_t m = ah * bl + ((al * bl) >> 32) + ((al * bh) & 0xffffffff);
      return xpair<uint64_t, uint64_t> (ah * bh + (m >> 32) + ((al * bh) >> 32), lhs * rhs);
    }
#else
    static constexpr xpair<uint64_t, uint64_t> mul128(uint64_t lhs, uint64_t rhs) {
      uint64_t al = lhs & 0xffffffff, ah = lhs >> 32;
      uint64_t bl = rhs & 0xffffffff, bh = rhs >> 32;
      uint64_t m = ah * bl + ((al * bl) >> 32) + ((al * bh) & 0xffffffff);
      return xpair<uint64_t, uint64_t> (ah * bh + (m >> 32) + ((al * bh) >> 32), lhs * rhs);
    }
#endif

#ifdef ENABLE_VCBITSCANREVERSE
    static constexpr unsigned clz64(uint64_t u) {
      if (!std::is_constant_evaluated()) {
	unsigned long idx = 0;
	return _BitScanReverse64(&idx, u) ? (63 - idx) : 64;
      } else {
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
    }
#elif defined(ENABLE_GNUC_CLZ)
    static constexpr unsigned clz64(uint64_t u) { return u == 0 ? 64 : __builtin_clzl(u); }
#elif defined(ENABLE_CUDA_CLZ)
    static constexpr unsigned clz64(uint64_t u) {
      if (!std::is_constant_evaluated()) return __clzll(u);
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
#else
    static constexpr unsigned clz64(uint64_t u) {
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
#endif

    template<typename T>
    class SafeArray {
      T* bufPtr;
      const int64_t bufSize;
      T dummyElm = 0;
    public:
      SafeArray(T* const bufPtr_, const int64_t bufSize_) :
	bufPtr(bufPtr_), bufSize(bufSize_) {}

      T operator[](int64_t pos) const {
	if (0 <= pos && pos < bufSize) return bufPtr[pos];
	return 0;
      }

      T& operator[](int64_t pos) {
	if (0 <= pos && pos < bufSize) return bufPtr[pos];
	return dummyElm;
      }

      void strcpyFrom(int64_t pos, const T *p) {
	if (pos < 0) return;
	while(pos < bufSize-1 && *p != 0) bufPtr[pos++] = *p++;
	if (pos < bufSize) bufPtr[pos] = 0;
      }

      void memmove(int64_t pos, const T *p, int64_t size) {
	if (!(0 <= pos && pos + size < bufSize)) return;
	memcpy(&bufPtr[pos], p, size * sizeof(T));
      }

      void memmove(int64_t dpos, int64_t spos, int64_t size) {
	if (!(0 <= dpos && dpos + size < bufSize)) return;
	if (!(0 <= spos && spos + size < bufSize)) return;
	::memmove(&bufPtr[dpos], &bufPtr[spos], size * sizeof(T));
      }
    };
  } // namespace detail

  template<int N> class BigUInt {
    static_assert(N >= 7, "N must be 7 or larger");
    static_assert(sizeof(unsigned long long int) == 8, "unsigned long long int must be 64-bit");
    static_assert(sizeof(long long int) == 8, "long long int must be 64-bit");
    static_assert(sizeof(unsigned int) == 4, "unsigned int must be 32-bit");
    static_assert(sizeof(int) == 4, "int must be 32-bit");
  public:
    BigUInt<N-1> low = 0, high = 0;

    constexpr BigUInt(const BigUInt<N-1>& h, const BigUInt<N-1>& l) : low(l), high(h) {}

    template<int..., int K = N, std::enable_if_t<(K >= 10), int> = 0>
    static constexpr BigUInt mul(const BigUInt<N-1>& lhs, const BigUInt<N-1>& rhs) {
      static_assert(sizeof(BigUInt) == (sizeof(uint64_t) << (N - 6)), "Class memory layout");
#if __cplusplus >= 202002L || defined(_MSC_VER)
      if (std::is_constant_evaluated()) return BigUInt(lhs.mulhi(rhs), lhs * rhs);
#endif
      if (lhs.isZero() || rhs.isZero()) return BigUInt(0);
      if (lhs.isAllOne() && rhs.isAllOne()) return BigUInt(~BigUInt<N-1>(1), BigUInt<N-1>(1));
      auto x0tx1 = lhs.high.adc(lhs.low, false), y0ty1 = rhs.high.adc(rhs.low, false);
      BigUInt<N-1> z1 = BigUInt<N-1>::mul(x0tx1.first, y0ty1.first);
      unsigned z1c = x0tx1.second & y0ty1.second;
      if (y0ty1.second) {
	auto t = z1.adc(BigUInt<N-1>(x0tx1.first, 0), false);
	z1c += t.second;
	z1 = t.first;
      }
      if (x0tx1.second) {
	auto t = z1.adc(BigUInt<N-1>(y0ty1.first, 0), false);
	z1c += t.second;
	z1 = t.first;
      }
      BigUInt<N-1> z0 = BigUInt<N-1>::mul(lhs.low, rhs.low), z2 = BigUInt<N-1>::mul(lhs.high, rhs.high);
      auto z0pz2 = z0.adc(z2, false);
      z1c -= z0pz2.second;
      auto t = z1.sbc(z0pz2.first, false);
      return BigUInt(z2, z0) + (BigUInt(z1c - t.second, t.first) << (1 << (N-2)));
    }

    template<int..., int K = N, std::enable_if_t<(K == 8 || K == 9), int> = 0>
    static constexpr BigUInt mul(const BigUInt<N-1>& lhs, const BigUInt<N-1>& rhs) {
      static_assert(sizeof(BigUInt) == (sizeof(uint64_t) << (N - 6)), "Class memory layout");
#if __cplusplus >= 202002L || defined(_MSC_VER)
      if (std::is_constant_evaluated()) return BigUInt(lhs.mulhi(rhs), lhs * rhs);
#endif
      BigUInt ret(0);
      uint64_t *plhs = (uint64_t *)&lhs, *prhs = (uint64_t *)&rhs, *pret = (uint64_t *)&ret;
      uint64_t ah = 0, am = 0, al = 0;
      const unsigned n = 1 << (N - 7);
      for(unsigned i=0;i<n;i++) {
	for(unsigned j=0;j<=i;j++) {
	  detail::xpair<uint64_t, uint64_t> m = detail::mul128(plhs[j], prhs[i - j]);
	  detail::xpair<uint64_t, bool> ub = detail::adc64(false, m.second, al);
	  al = ub.first;
	  ub = detail::adc64(ub.second, m.first, am);
	  am = ub.first;
	  ah += ub.second;
	}
	pret[i] = uint64_t(al);
	al = am; am = ah; ah = 0;
      }
      for(unsigned i=n;i<n*2-1;i++) {
	for(unsigned j=i-(n-1);j<n;j++) {
	  detail::xpair<uint64_t, uint64_t> m = detail::mul128(plhs[j], prhs[i - j]);
	  detail::xpair<uint64_t, bool> ub = detail::adc64(false, m.second, al);
	  al = ub.first;
	  ub = detail::adc64(ub.second, m.first, am);
	  am = ub.first;
	  ah += ub.second;
	}
	pret[i] = uint64_t(al);
	al = am; am = ah; ah = 0;
      }
      pret[n*2-1] = uint64_t(al);
      return ret;
    }

    template<int..., int K = N, std::enable_if_t<K == 7, int> = 0>
    static constexpr BigUInt mul(const BigUInt<N-1>& lhs, const BigUInt<N-1>& rhs) {
      detail::xpair<uint64_t, uint64_t> m = detail::mul128(lhs.u64, rhs.u64);
      return BigUInt(m.first, m.second);
    }

    constexpr detail::xpair<BigUInt, bool> inc() const {
      auto rl = low.inc(), rh = high.inc();
      detail::xpair<BigUInt, bool> s(0, false);
      s.first.low  = rl.first;
      s.first.high = rl.second ? rh.first : high;
      s.second = rl.second ? rh.second : false;
      return s;
    }
    constexpr detail::xpair<BigUInt, bool> dec() const {
      auto rl = low.dec(), rh = high.dec();
      detail::xpair<BigUInt, bool> s(0, false);
      s.first.low  = rl.first;
      s.first.high = rl.second ? rh.first : high;
      s.second = rl.second ? rh.second : false;
      return s;
    }
    constexpr detail::xpair<BigUInt, bool> adc(const BigUInt& rhs, bool c) const {
      auto rl = low.adc(rhs.low, c), rh = high.adc(rhs.high, rl.second);
      detail::xpair<BigUInt, bool> s(0, false);
      s.first.high = rh.first;
      s.first.low  = rl.first;
      s.second = rh.second;
      return s;
    }
    constexpr detail::xpair<BigUInt, bool> sbc(const BigUInt& rhs, bool c) const {
      auto rl = low.sbc(rhs.low, c), rh = high.sbc(rhs.high, rl.second);
      detail::xpair<BigUInt, bool> s(0, false);
      s.first.high = rh.first;
      s.first.low  = rl.first;
      s.second = rh.second;
      return s;
    }
    constexpr bool msb() const { return high.msb(); }

    constexpr BigUInt mulhi(const BigUInt& rhs) const {
      BigUInt ll = mul(low , rhs.low), lh = mul(low , rhs.high);
      BigUInt hl = mul(high, rhs.low);
      BigUInt m = hl + ll.high + lh.low, hh = mul(high, rhs.high);
      return hh + m.high + lh.high;
    }
    constexpr BigUInt mulhiAprx(const BigUInt& rhs) const {
      BigUInt hh = mul(high, rhs.high);
      BigUInt<N-1> lh = low.mulhiAprx(rhs.high), hl = high.mulhiAprx(rhs.low);
      return hh + hl + lh;
    }
    constexpr BigUInt mulhiAprx2(const BigUInt<N-1>& rhs) const {
      return mul(high, rhs) + low.mulhiAprx(rhs);
    }

    constexpr BigUInt reciprocalAprx() const {
      BigUInt<N-1> x((high + ((~high >> (sizeof(high)*8 - 1)) & 1)).reciprocalAprx());
      BigUInt y((-mulhiAprx2(x)).mulhiAprx2(x));
      return y + y;
    }

    template<int> friend class BigUInt;
    template<int> friend class BigInt;

    constexpr BigUInt() = default;

    constexpr BigUInt(unsigned long long int u) : low(u), high(uint64_t(0)) {}
    constexpr BigUInt(unsigned long int u) : low(u), high(uint64_t(0)) {}
    constexpr BigUInt(unsigned int u) : low(u), high(uint64_t(0)) {}
    constexpr BigUInt(unsigned short int u) : low(u), high(uint64_t(0)) {}
    constexpr BigUInt(unsigned char u) : low(u), high(uint64_t(0)) {}
    constexpr BigUInt(bool u) : low(u), high(uint64_t(0)) {}

    constexpr explicit operator unsigned long long int() const { return (unsigned long long)uint64_t(low); }
    constexpr explicit operator unsigned long int() const { return (unsigned long)uint64_t(low); }
    constexpr explicit operator unsigned int() const { return (unsigned int)uint64_t(low); }
    constexpr explicit operator unsigned short() const { return (unsigned short)uint64_t(low); }
    constexpr explicit operator unsigned char() const { return (unsigned char)uint64_t(low); }
    constexpr explicit operator bool() const { return !isZero(); }

    constexpr BigUInt(long long int i) :
      BigUInt(i < 0 ? (~BigUInt(0) - uint64_t(-i) + 1) : uint64_t(i)) {}
    constexpr BigUInt(long int i) : BigUInt((long long int)(i)) {}
    constexpr BigUInt(int i) : BigUInt((long long int)(i)) {}
    constexpr BigUInt(short int i) : BigUInt((long long int)(i)) {}
    constexpr BigUInt(signed char i) : BigUInt((long long int)(i)) {}
    constexpr BigUInt(char i) : BigUInt((long long int)(i)) {}

    constexpr explicit operator long long int() const { return uint64_t(low); }
    constexpr explicit operator long int() const { return uint64_t(low); }
    constexpr explicit operator int() const { return uint64_t(low); }
    constexpr explicit operator short int() const { return uint64_t(low); }
    constexpr explicit operator signed char() const { return uint64_t(low); }
    constexpr explicit operator char() const { return uint64_t(low); }

    constexpr BigUInt(const BigUInt& m) = default;
    constexpr BigUInt(const BigUInt<N+1>& h) : low(h.low.low), high(h.low.high) {}

    template<int K, std::enable_if_t<(K < N)>* = nullptr>
    constexpr BigUInt(const BigUInt<K>& l) : low(BigUInt<N-1>(l)), high(uint64_t(0)) {}
    template<int K, std::enable_if_t<(K > (N+1))>* = nullptr>
    constexpr BigUInt(const BigUInt<K>& h) : BigUInt(BigUInt<N+1>(h)) {}

    constexpr BigUInt& operator=(const BigUInt &u) = default;

    constexpr BigUInt(const uint64_t *p) : low(p), high(p + (1 << (N - 7))) {}

    constexpr explicit BigUInt(double d) {
      int x = 0;
      d = frexp(d, &x);
      d = ldexp(d, 53);
      x -= 53;
      uint64_t l = (uint64_t)fabs(d);
      if (x >= 0) {
	BigUInt u(l);
	u <<= x;
	low = u.low; high = u.high;
      } else if (x > -60) {
	l = l >> -x;
	BigUInt u(l);
	low = u.low; high = u.high;
      } else {
	low = high = 0;
      }
    }

    constexpr explicit operator double() const {
      int e0 = ilogbp1();
      double d0 = double((*this << (53 - e0)).getWord(0));
      double d1 = ldexp(d0, e0 - 53);
      BigUInt b = *this - BigUInt(d1);
      int e1 = b.ilogbp1();
      double d2 = double((b << (64 - e1)).getWord(0));
      double d3 = ldexp(d2, e1 - 64);
      return d1 + d3;
    }

    constexpr unsigned clz() const {
      unsigned z = high.clz();
      if (z == (1 << (N-1))) return low.clz() + (1 << (N-1));
      return z;
    }
    constexpr unsigned ilogbp1() const { return (1U << N) - clz(); }
    constexpr bool isZero() const { return low.isZero() && high.isZero(); }
    constexpr bool isAllOne() const { return low.isAllOne() && high.isAllOne(); }
    constexpr uint64_t getWord(unsigned idx) const {
      return idx >= (1 << (N-7)) ? high.getWord(idx - (1 << (N-7))) : low.getWord(idx);
    }
    constexpr void setWord(unsigned idx, uint64_t u) {
      if (idx >= (1 << (N-7))) high.setWord(idx - (1 << (N-7)), u);
      else low.setWord(idx, u);
    }
    constexpr bool bit(unsigned idx) const {
      return (getWord(idx >> 6) >> (idx & 63)) & 1;
    }

    constexpr BigUInt& operator++()    { *this = inc().first; return *this; }
    constexpr BigUInt& operator--()    { *this = dec().first; return *this; }
    constexpr BigUInt  operator++(int) { BigUInt t = *this; *this = inc().first; return t; }
    constexpr BigUInt  operator--(int) { BigUInt t = *this; *this = dec().first; return t; }

    constexpr BigUInt operator+(const BigUInt& rhs) const { return adc(rhs, false).first; }
    constexpr BigUInt operator+(const BigUInt<N-1>& rhs) const {
      auto rl = low.adc(rhs, false);
      auto rh = high.inc();
      return BigUInt(rl.second ? rh.first : high, rl.first);
    }

    constexpr BigUInt operator-(const BigUInt& rhs) const { return sbc(rhs, false).first; }
    constexpr BigUInt operator-(const BigUInt<N-1>& rhs) const {
      auto rl = low.sbc(rhs, false);
      auto rh = high.dec();
      return BigUInt(rl.second ? rh.first : high, rl.first);
    }

    constexpr BigUInt operator-() const {
      BigUInt r = ~*this;
      r = r.inc().first;
      return r;
    }

    constexpr BigUInt operator+() const { return *this; }

    constexpr BigUInt operator*(const BigUInt& rhs) const {
      BigUInt ll = mul(low, rhs.low), lh = mul(low, rhs.high);
      BigUInt hl = mul(high, rhs.low);
      return BigUInt(lh.low + hl.low + ll.high, ll.low);
    }
    constexpr BigUInt operator*(const BigUInt<N-1>& rhs) const {
      BigUInt ll = mul(low, rhs), hl = mul(high, rhs);
      return BigUInt(hl.low + ll.high, ll.low);
    }

    constexpr BigUInt operator/(const BigUInt& rhs) const {
      if (rhs == 1) return *this;
      BigUInt q = this->mulhi(rhs.reciprocal());
      if (!(rhs > *this - q * rhs)) q++;
      return q;
    }

    constexpr BigUInt operator%(const BigUInt& rhs) const {
      BigUInt q = this->mulhi(rhs.reciprocal());
      BigUInt m = *this - q * rhs;
      if (!(rhs > m)) m -= rhs;
      return m;
    }

    constexpr BigUInt reciprocal() const {
      unsigned z = clz();
      BigUInt t0 = (*this << z).reciprocalAprx();
      if (z < (1 << N) - (1 << (N-4))) return t0 >> ((1 << N) - 1 - z);
      BigUInt t1 = (-(*this << z).mulhi(t0)).mulhi(t0);
      return t1 >> ((1 << N) - 2 - z);
    }

    constexpr bool eq(BigUInt const& rhs) const { return low.eq(rhs.low) && high.eq(rhs.high); }
    constexpr bool operator==(BigUInt const& rhs) const { return eq(rhs); }
    constexpr bool operator!=(BigUInt const& rhs) const { return !eq(rhs); }

    constexpr int compare(BigUInt const& rhs) const {
      int c = high.compare(rhs.high);
      if (c == 0) return low.compare(rhs.low);
      return c;
    }
    constexpr bool operator> (BigUInt const& rhs) const { return compare(rhs) >  0; }
    constexpr bool operator< (BigUInt const& rhs) const { return rhs > *this; }
    constexpr bool operator<=(BigUInt const& rhs) const { return !(*this > rhs); }
    constexpr bool operator>=(BigUInt const& rhs) const { return !(*this < rhs); }

    constexpr BigUInt operator&(const BigUInt& rhs) const { return BigUInt(high & rhs.high, low & rhs.low); }
    constexpr BigUInt operator|(const BigUInt& rhs) const { return BigUInt(high | rhs.high, low | rhs.low); }
    constexpr BigUInt operator^(const BigUInt& rhs) const { return BigUInt(high ^ rhs.high, low ^ rhs.low); }
    constexpr BigUInt operator~() const { return BigUInt(~high, ~low); }

    constexpr BigUInt operator<<(int n) const {
      if (n >= (1 << N) || n <= -(1 << N)) return uint64_t(0);
      if (n >= 0) {
	return BigUInt((high <<  n) | (low  >> ((1 << (N-1)) - n)) , (low <<  n));
      } else {
	return BigUInt((high >> -n) , (high << ((1 << (N-1)) + n)) | (low >> -n));
      }
    }
    constexpr BigUInt operator>>(int n) const { return *this << -n; }

    constexpr BigUInt& operator<<=(int n) { *this = *this << n; return *this; }
    constexpr BigUInt& operator>>=(int n) { *this = *this >> n; return *this; }

    //

    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator+(const rhstype &rhs) const { return operator+(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator-(const rhstype &rhs) const { return operator-(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator*(const rhstype &rhs) const { return operator*(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator/(const rhstype &rhs) const { return operator/(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator%(const rhstype &rhs) const { return operator%(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator&(const rhstype &rhs) const { return operator&(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator|(const rhstype &rhs) const { return operator|(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigUInt operator^(const rhstype &rhs) const { return operator^(BigUInt(rhs)); }

    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator+(const lhstype c, const BigUInt& rhs) { return BigUInt(c) + rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator-(const lhstype c, const BigUInt& rhs) { return BigUInt(c) - rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator*(const lhstype c, const BigUInt& rhs) { return BigUInt(c) * rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator/(const lhstype c, const BigUInt& rhs) { return BigUInt(c) / rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator%(const lhstype c, const BigUInt& rhs) { return BigUInt(c) % rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator&(const lhstype c, const BigUInt& rhs) { return BigUInt(c) & rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator|(const lhstype c, const BigUInt& rhs) { return BigUInt(c) | rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigUInt operator^(const lhstype c, const BigUInt& rhs) { return BigUInt(c) ^ rhs; }

    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator==(const rhstype& rhs) const { return operator==(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator!=(const rhstype& rhs) const { return operator!=(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator> (const rhstype& rhs) const { return operator> (BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator< (const rhstype& rhs) const { return operator< (BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator>=(const rhstype& rhs) const { return operator>=(BigUInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator<=(const rhstype& rhs) const { return operator<=(BigUInt(rhs)); }

    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator==(const lhstype& lhs, const BigUInt& rhs) { return BigUInt(lhs) == rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator!=(const lhstype& lhs, const BigUInt& rhs) { return BigUInt(lhs) != rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator> (const lhstype& lhs, const BigUInt& rhs) { return BigUInt(lhs) >  rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator< (const lhstype& lhs, const BigUInt& rhs) { return BigUInt(lhs) <  rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator>=(const lhstype& lhs, const BigUInt& rhs) { return BigUInt(lhs) >= rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator<=(const lhstype& lhs, const BigUInt& rhs) { return BigUInt(lhs) <= rhs; }

    //

    template<typename srctype>
    constexpr BigUInt& operator=(const srctype& s) {
      BigUInt n(s);
      *this = n;
      return *this;
    }

    template<typename rhstype>
    constexpr BigUInt& operator+=(const rhstype& rhs) { *this = *this + BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator-=(const rhstype& rhs) { *this = *this - BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator*=(const rhstype& rhs) { *this = *this * BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator/=(const rhstype& rhs) { *this = *this / BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator%=(const rhstype& rhs) { *this = *this % BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator&=(const rhstype& rhs) { *this = *this & BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator|=(const rhstype& rhs) { *this = *this | BigUInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigUInt& operator^=(const rhstype& rhs) { *this = *this ^ BigUInt(rhs); return *this; }

    //

    constexpr BigUInt divmod(const BigUInt& rhs, const BigUInt& recip, BigUInt* mod) const {
      if (rhs == 1) { *mod = 0; return *this; }
      BigUInt q = this->mulhi(recip), m = *this - q * rhs;
      if (!(rhs > m)) { q++; m = m - rhs; }
      if (mod) *mod = m;
      return q;
    }

    constexpr detail::xpair<BigUInt, BigUInt> divmod(const BigUInt& rhs, const BigUInt& recip) const {
      if (rhs == 1) detail::xpair<BigUInt, BigUInt>(*this, 0);
      BigUInt q = this->mulhi(recip), m = *this - q * rhs;
      if (!(rhs > m)) { q++; m = m - rhs; }
      return detail::xpair<BigUInt, BigUInt>(q, m);
    }

    constexpr BigUInt mod(const BigUInt& rhs, const BigUInt& recip) const {
      if (rhs == 1) return 0;
      BigUInt q = this->mulhi(recip), m = *this - q * rhs;
      return m >= rhs ? m - rhs : m;
    }

    constexpr BigUInt pow(BigUInt e, const BigUInt& m = 0, BigUInt recm = 0) const {
      if (m == 1) return 0;
      bool mIsZero = m.isZero();
      if (!mIsZero && recm.isZero()) recm = m.reciprocal();
      BigUInt r = 1, b = *this;
      if (!mIsZero) b = b.mod(m, recm);
      while(e > 0) {
	if (e.getWord(0) & 1) {
	  r *= b;
	  if (!mIsZero) r = r.mod(m, recm);
	}
	e >>= 1;
	b *= b;
	if (!mIsZero) b = b.mod(m, recm);
      }
      return r;
    }

    constexpr BigUInt gcd(BigUInt b) const {
      BigUInt a = *this;
      while(!b.isZero()) {
	BigUInt t = b;
	b = a % b;
	a = t;
      }
      return a;
    }

    //

    constexpr BigUInt(const char *p) {
      while(*p == ' ') p++;
      BigUInt r;
      uint64_t u = 0, d = 1;
      while('0' <= *p && *p <= '9') {
	u = u * 10 + (*p - '0');
	d *= 10;
	if (d >= (uint64_t(1) << 60)) {
	  r = r * d + u; u = 0; d = 1;
	}
	p++;
      }
      r = r * d + BigUInt(u);
      low = r.low; high = r.high;
    }

    class Montgomery {
      BigUInt<N> ninv(BigUInt<N> n) {
	BigUInt<N> nr(1);
	for (int i = 0; i < N; i++) nr *= 2 - n * nr;
	return nr;
      }
    public:
      const BigUInt<N> n, nr;
      const BigUInt<N+1> recn;
      const BigUInt<N> one;

      constexpr Montgomery(BigUInt<N> n) : n(n), nr(ninv(n)), recn(BigUInt<N+1>(n).reciprocal()), one(transform(1)) {}

      Montgomery& operator=(const Montgomery&) = delete;
      Montgomery(Montgomery&) = delete;

      constexpr BigUInt<N> reduce(const BigUInt<N+1>& x) const {
	BigUInt<N> q = x.low * nr;
	BigUInt<N> m = q.mulhi(n);
	BigUInt<N> y = x.high - m;
	return x.high < m ? y + n : y;
      }

      constexpr BigUInt<N> mul(const BigUInt<N>& x, const BigUInt<N>& y) const {
	return reduce(BigUInt<N+1>::mul(x, y));
      }

      constexpr BigUInt<N> transform(const BigUInt<N>& x) const {
	return BigUInt<N+1>(x, 0).mod(n, recn).low;
      }

      constexpr BigUInt<N> pow(BigUInt<N> a, BigUInt<N> e) const {
	// a is in Montgomery representation
	BigUInt<N> r = one;
  
	for (;;) {
	  if (e.getWord(0) & 1) r = mul(r, a);
	  e >>= 1;
	  if (e.isZero()) break;
	  a = mul(a, a);
	}

	return r; // r is in Montgomery representation
      }
    };
  };

  template<> class BigUInt<6> {
  public:
    uint64_t u64 = 0;

    constexpr detail::xpair<BigUInt<6>, bool> inc() const {
      detail::xpair<BigUInt<6>, bool> s = { 0, false };
      s.first = u64 + 1;
      s.second = s.first == 0;
      return s;
    }
    constexpr detail::xpair<BigUInt<6>, bool> dec() const {
      detail::xpair<BigUInt<6>, bool> s = { 0, false };
      s.second = u64 == 0;
      s.first = u64 - 1;
      return s;
    }
    constexpr detail::xpair<BigUInt<6>, bool> adc(const BigUInt<6>& rhs, bool c) const {
      detail::xpair<BigUInt<6>, bool> s = { 0, false };
      detail::xpair<uint64_t, bool> ub = detail::adc64(c, u64, rhs.u64);
      s.first = ub.first;
      s.second = ub.second;
      return s;
    }
    constexpr detail::xpair<BigUInt<6>, bool> sbc(const BigUInt<6>& rhs, bool c) const {
      detail::xpair<BigUInt<6>, bool> s = { 0, false };
      detail::xpair<uint64_t, bool> ub = detail::sbc64(c, u64, rhs.u64);
      s.first = ub.first;
      s.second = ub.second;
      return s;
    }
    constexpr bool msb() const { return u64 >> 63; }

    constexpr BigUInt<6> reciprocalAprx() const {
      BigUInt<6> x(((~0ULL) / (u64 >> 32)) << 31);
      return (BigUInt<6>(uint64_t(0)) - mulhi(x)).mulhi(x) << 1;
    }

    constexpr BigUInt<6> mulhi(const BigUInt<6>& rhs) const { return detail::mul128(u64, rhs.u64).first; }
    constexpr BigUInt<6> mulhiAprx(const BigUInt<6>& rhs) const { return mulhi(rhs); }

    template<int> friend class BigUInt;
    template<int> friend class BigInt;

    constexpr explicit BigUInt(const uint64_t *p) : u64(*p) {}

    constexpr bool isZero() const { return u64 == 0; }
    constexpr bool isAllOne() const { return u64 == ~uint64_t(0); }
    constexpr uint64_t getWord(unsigned idx) const { return u64; }
    constexpr void setWord(unsigned idx, uint64_t u) { u64 = u; }

    constexpr bool eq(BigUInt<6> const& rhs) const { return u64 == rhs.u64; }

    constexpr BigUInt<6>& operator=(const BigUInt<6> &u) = default;

    constexpr BigUInt<6> operator+(const BigUInt<6>& rhs) const { return u64 + rhs.u64; }
    constexpr BigUInt<6> operator-(const BigUInt<6>& rhs) const { return u64 - rhs.u64; }
    constexpr BigUInt<6> operator*(const BigUInt<6>& rhs) const { return u64 * rhs.u64; }
    constexpr BigUInt<6> operator/(const BigUInt<6>& rhs) const { return u64 / rhs.u64; }
    constexpr BigUInt<6> operator%(const BigUInt<6>& rhs) const { return u64 % rhs.u64; }

    constexpr bool operator==(BigUInt<6> const& rhs) const { return u64 == rhs.u64; }
    constexpr bool operator!=(BigUInt<6> const& rhs) const { return !(*this == rhs); }
    constexpr bool operator> (BigUInt<6> const& rhs) const { return u64 > rhs.u64; }

    constexpr int compare(BigUInt<6> const& rhs) const {
      if (u64 > rhs.u64) return +1;
      if (u64 < rhs.u64) return -1;
      return 0;
    }

    constexpr BigUInt<6> operator&(const BigUInt<6>& rhs) const { return u64 & rhs.u64; }
    constexpr BigUInt<6> operator|(const BigUInt<6>& rhs) const { return u64 | rhs.u64; }
    constexpr BigUInt<6> operator^(const BigUInt<6>& rhs) const { return u64 ^ rhs.u64; }
    constexpr BigUInt<6> operator~()                      const { return ~u64; }
    constexpr BigUInt<6> operator<<(int n)                const {
      return (n > 63 || n < -63) ? 0 : n >= 0 ? (u64 << n) : (u64 >> -n);
    }
    constexpr BigUInt<6> operator>>(int n)                const { return *this << -n; }
    constexpr unsigned clz() const { return detail::clz64(u64); }

    //

    constexpr BigUInt() = default;
    constexpr BigUInt(const BigUInt &m) = default;
    constexpr BigUInt(uint64_t u) : u64(u) {}
    constexpr explicit operator unsigned long long int() const { return (unsigned long long)u64; }
    constexpr explicit operator unsigned long int() const { return (unsigned long)u64; }

    constexpr BigUInt(const BigUInt<7>& h) : BigUInt(h.low) {}
  };

  template<int N> class BigInt {
    static_assert(N >= 7, "N must be 7 or larger");
    BigUInt<N> u = 0;

    template<int> friend class BigUInt;
    template<int> friend class BigInt;
  public:
    constexpr BigInt() = default;

    constexpr BigInt(long long int i) : u(i < 0 ? -BigUInt<N>(uint64_t(-i)) : BigUInt<N>(uint64_t(i))) {}
    constexpr BigInt(long int i) : BigInt((long long int)(i)) {}
    constexpr BigInt(int i) : BigInt((long long int)(i)) {}
    constexpr BigInt(short int i) : BigInt((long long int)(i)) {}
    constexpr BigInt(signed char i) : BigInt((long long int)(i)) {}
    constexpr BigInt(char i) : BigInt((long long int)(i)) {}

    constexpr explicit operator long long int() const { return (long long int)(u.getWord(0)); }
    constexpr explicit operator long int() const { return (long int)(u.getWord(0)); }
    constexpr explicit operator int() const { return (int)(u.getWord(0)); }
    constexpr explicit operator short int() const { return (short int)(u.getWord(0)); }
    constexpr explicit operator signed char() const { return (signed char)(u.getWord(0)); }
    constexpr explicit operator char() const { return (char)(u.getWord(0)); }

    constexpr BigInt(unsigned long long u) : u(BigUInt<N>(u)) {}
    constexpr BigInt(unsigned long u) : u(BigUInt<N>(u)) {}
    constexpr BigInt(unsigned int u) : u(BigUInt<N>(u)) {}
    constexpr BigInt(unsigned short u) : u(BigUInt<N>(u)) {}
    constexpr BigInt(unsigned char u) : u(BigUInt<N>(u)) {}
    constexpr BigInt(bool u) : u(BigUInt<N>(u)) {}

    constexpr explicit operator unsigned long long int() const { return int64_t(*this); }
    constexpr explicit operator unsigned long int() const { return int64_t(*this); }
    constexpr explicit operator unsigned int() const { return int64_t(*this); }
    constexpr explicit operator unsigned short int() const { return int64_t(*this); }
    constexpr explicit operator unsigned char() const { return int64_t(*this); }
    constexpr explicit operator bool() const { return !u.isZero(); }

    constexpr BigInt(const BigUInt<N>& up) : u(up) {}

    template<int K, std::enable_if_t<(K < (N-1))>* = nullptr>
    constexpr BigInt(const BigInt<K>& l) : BigInt(BigInt<N-1>(l)) {}
    template<int K, std::enable_if_t<(K == (N-1))>* = nullptr>
    constexpr BigInt(const BigInt<K>& l) : 
      u(BigUInt<N>(l.u.msb() ? ~BigUInt<N-1>(0) : BigUInt<N-1>(0), l.u)) {}

    constexpr BigInt(const BigInt& m) = default;

    template<int K, std::enable_if_t<(K == (N+1))>* = nullptr>
    constexpr BigInt(const BigInt<K>& h) : u(BigUInt<N>(h.u.low)) {}
    template<int K, std::enable_if_t<(K > (N+1))>* = nullptr>
    constexpr BigInt(const BigInt<K>& h) : BigInt(BigInt<N+1>(h)) {}

    constexpr BigInt(const uint64_t *p) : u(p) {}

    constexpr explicit BigInt(double d) : u(d < 0 ? -BigUInt<N>(-d) : BigUInt<N>(d)) {}

    constexpr explicit operator double() const { return u.msb() ? -double(-u) : double(u); }

    constexpr operator BigUInt<N>() const { return u; }

    constexpr BigInt abs() const { return u.msb() ? -u : u; }

    constexpr BigInt& operator=(const BigInt &ip) = default;

    constexpr BigInt& operator++()    { u = u.inc().first; return *this; }
    constexpr BigInt  operator++(int) { BigInt t = *this; u = u.inc().first; return t; }
    constexpr BigInt& operator--()    { u = u.dec().first; return *this; }
    constexpr BigInt  operator--(int) { BigInt t = *this; u = u.dec().first; return t; }

    constexpr BigInt operator+(const BigInt& rhs) const { return u + rhs.u; }
    constexpr BigInt operator-(const BigInt& rhs) const { return u - rhs.u; }
    constexpr BigInt operator-()                     const { return -u; }
    constexpr BigInt operator+()                     const { return +u; }
    constexpr BigInt operator*(const BigInt& rhs) const { return u * rhs.u; }

    constexpr BigInt operator/(const BigInt& rhs) const {
      BigUInt<N> m = abs().u / rhs.abs().u;
      return (u.msb() ^ rhs.u.msb()) ? -m : m;
    }

    constexpr BigInt operator%(const BigInt& rhs) const {
      BigUInt<N> m = abs().u % rhs.abs().u;
      return u.msb() ? -m : m;
    }

    constexpr bool isNegative() const { return u.msb(); }
    constexpr bool isZero() const { return u.isZero(); }

    constexpr BigInt divmod(const BigInt& rhs, const BigUInt<N>& recip, BigInt* mod) const {
      BigUInt<N> r, q = abs().u.divmod(rhs.abs().u, recip, &r);
      if (u.msb() ^ rhs.u.msb()) { q = -q; r = -r; }
      *mod = r;
      return q;
    }

    constexpr BigInt pow(BigUInt<N> e, const BigUInt<N>& m = 0, BigUInt<N> recm = 0) const {
      BigInt p = abs().u.pow(e, m, recm);
      if (isNegative() && (e.getWord(0) & 1) == 1) p = -p;
      return p;
    }

    constexpr BigUInt<N> reciprocal() const { return u.reciprocal(); }

    constexpr int64_t getWord(unsigned idx) const { return u.getWord(idx); }

    constexpr bool operator==(BigInt const& rhs) const { return u.eq(rhs.u); }
    constexpr bool operator!=(BigInt const& rhs) const { return !u.eq(rhs.u); }

    constexpr int compare(BigInt const& rhs) const {
      if (u.msb()) {
	if (!rhs.u.msb()) return -1;
	return u.compare(rhs.u);
      } else {
	if (rhs.u.msb()) return +1;
	return u.compare(rhs.u);
      }
    }
    constexpr bool operator> (BigInt const& rhs) const { return compare(rhs) > 0; }
    constexpr bool operator< (BigInt const& rhs) const { return rhs > *this; }
    constexpr bool operator<=(BigInt const& rhs) const { return !(*this > rhs); }
    constexpr bool operator>=(BigInt const& rhs) const { return !(*this < rhs); }

    constexpr BigInt operator&(const BigInt& rhs) const { return u & rhs.u; }
    constexpr BigInt operator|(const BigInt& rhs) const { return u | rhs.u; }
    constexpr BigInt operator^(const BigInt& rhs) const { return u ^ rhs.u; }
    constexpr BigInt operator~()                     const { return ~u; }

    constexpr BigInt operator>>(int n) const {
      if (!u.msb()) return u >> n;
      return (u >> n) | ((~BigUInt<N>(0)) << ((1 << N) - n));
    }
    constexpr BigInt  operator<<(int n) const { return u << n; }

    constexpr BigInt& operator>>=(int n) { *this = *this >> n; return *this; }
    constexpr BigInt& operator<<=(int n) { *this = *this << n; return *this; }

    //

    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator+(const rhstype &rhs) const { return operator+(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator-(const rhstype &rhs) const { return operator-(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator*(const rhstype &rhs) const { return operator*(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator/(const rhstype &rhs) const { return operator/(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator%(const rhstype &rhs) const { return operator%(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator&(const rhstype &rhs) const { return operator&(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator|(const rhstype &rhs) const { return operator|(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr BigInt operator^(const rhstype &rhs) const { return operator^(BigInt(rhs)); }

    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator+(const lhstype c, const BigInt& rhs) { return BigInt(c) + rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator-(const lhstype c, const BigInt& rhs) { return BigInt(c) - rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator*(const lhstype c, const BigInt& rhs) { return BigInt(c) * rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator/(const lhstype c, const BigInt& rhs) { return BigInt(c) / rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator%(const lhstype c, const BigInt& rhs) { return BigInt(c) % rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator&(const lhstype c, const BigInt& rhs) { return BigInt(c) & rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator|(const lhstype c, const BigInt& rhs) { return BigInt(c) | rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend BigInt operator^(const lhstype c, const BigInt& rhs) { return BigInt(c) ^ rhs; }

    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator==(const rhstype& rhs) const { return operator==(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator!=(const rhstype& rhs) const { return operator!=(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator> (const rhstype& rhs) const { return operator> (BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator< (const rhstype& rhs) const { return operator< (BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator>=(const rhstype& rhs) const { return operator>=(BigInt(rhs)); }
    template<typename rhstype, std::enable_if_t<(
      std::is_integral_v<rhstype> || (sizeof(rhstype) < (8 << (N - 6)) && !std::is_floating_point_v<rhstype>)), int> = 0>
    constexpr bool operator<=(const rhstype& rhs) const { return operator<=(BigInt(rhs)); }

    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator==(const lhstype& lhs, const BigInt& rhs) { return BigInt(lhs) == rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator!=(const lhstype& lhs, const BigInt& rhs) { return BigInt(lhs) != rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator> (const lhstype& lhs, const BigInt& rhs) { return BigInt(lhs) >  rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator< (const lhstype& lhs, const BigInt& rhs) { return BigInt(lhs) <  rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator>=(const lhstype& lhs, const BigInt& rhs) { return BigInt(lhs) >= rhs; }
    template<typename lhstype, std::enable_if_t<(
      std::is_integral_v<lhstype> || (sizeof(lhstype) < (8 << (N - 6)) && !std::is_floating_point_v<lhstype>)), int> = 0>
    constexpr friend bool operator<=(const lhstype& lhs, const BigInt& rhs) { return BigInt(lhs) <= rhs; }

    //

    template<typename srctype>
    constexpr BigInt& operator=(const srctype& s) {
      BigInt n(s);
      *this = n;
      return *this;
    }

    template<typename rhstype>
    constexpr BigInt& operator+=(const rhstype& rhs) { *this = *this + BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator-=(const rhstype& rhs) { *this = *this - BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator*=(const rhstype& rhs) { *this = *this * BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator/=(const rhstype& rhs) { *this = *this / BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator%=(const rhstype& rhs) { *this = *this % BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator&=(const rhstype& rhs) { *this = *this & BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator|=(const rhstype& rhs) { *this = *this | BigInt(rhs); return *this; }
    template<typename rhstype>
    constexpr BigInt& operator^=(const rhstype& rhs) { *this = *this ^ BigInt(rhs); return *this; }

    //

    constexpr BigInt(const char *p) {
      bool sign = false;
      while(*p == ' ') p++;
      if (*p == '-') { p++; sign = true; }
      u = BigUInt<N>(p);
      if (sign) u = -u;
    }

    NOINLINE static int snprint(char *cbuf, size_t bufsize, BigInt<N> value, char typespec = 'd',
				int width = 0, int precision = -1, int base = 10, int nbits = 1 << N,
				bool flag_sign = false, bool flag_blank = false, bool flag_alt = false,
				bool flag_left = false, bool flag_zero = false, bool flag_upper = false,
				bool flag_unsigned = false, bool flag_ptr = false, const char *prefix = "") {
      detail::SafeArray<char> buf(cbuf, bufsize);

      if (width > (int)bufsize) width = bufsize;

      if (nbits < (1 << N)) {
	bool signbit = value < 0;
	value &= (BigUInt<N>(1) << nbits) - 1;
	if (!flag_unsigned && signbit) value |= ~BigUInt<N>(0) & ~((BigUInt<N>(1) << nbits) - 1);
      }

      int sign = 0;
      if (!flag_unsigned) {
	if (value < 0) {
	  value = -value;
	  sign = '-';
	} else if (flag_sign) {
	  sign = '+';
	} else if (flag_blank) {
	  sign = ' ';
	}
      }

      int64_t idx = bufsize-1;
      const char *digits = flag_upper ? "0123456789ABCDEF" : "0123456789abcdef";
      int length = 0;

      if (flag_ptr && value == 0) {
	buf.memmove(idx-5, "(nil)", 5);
	idx -= 5;
      } else {
	if (flag_unsigned) {
	  BigUInt<N> u = value;
	  do {
	    buf[--idx] = digits[int(u % base)];
	    u /= base;
	  } while(u > 0);
	} else {
	  BigInt<N> v = value;
	  do {
	    buf[--idx] = digits[int(v % base)];
	    v /= base;
	  } while(v > 0);
	}
	if (precision == 0 && value == 0) idx++;
	length = bufsize-1 - idx;

	for(int i = precision-length;i > 0;i--) buf[--idx] = '0';

	if (sign) buf[--idx] = sign;

	if (flag_alt) {
	  if (!(base == 8 && buf[idx] == '0') && !(base != 8 && value == 0)) {
	    int prefixlen = strlen(prefix);
	    buf.memmove(idx - prefixlen, prefix, prefixlen);
	    idx -= prefixlen;
	  }
	}
      }

      length = bufsize-1 - idx;

      if (!flag_left) {
	for(int i=0;i<width-length;i++) buf[--idx] = ' ';
	length = bufsize-1 - idx;
      }

      buf.memmove(0, idx, length);

      if (flag_left) {
	idx = length;
	for(int i=0;i<width-length;i++) buf[idx++] = ' ';
	length = idx;
      }

      buf[length] = '\0';

      return length;
    }
  };
}

#ifndef NO_LIBSTDCXX

#include <iostream>
#include <vector>
#include <string>

namespace tlfloat {
  std::string toHexString(const uint64_t u);
  std::string toHexString(uint32_t x);
  std::string toHexString(uint16_t x);
  std::string toHexString(const BigUInt<6> &u);
  template<int N> static std::string toHexString(const BigUInt<N>& u) { return toHexString(u.high) + toHexString(u.low); }
  template<int N> static std::string toHexString(const BigInt<N>& u) { return toHexString(u.u); }

  template<int N>
  std::string to_string(const BigUInt<N>& u) {
    std::vector<char> s((1 << N)/3 + 3);
    BigInt<N>::snprint(s.data(), s.size(), BigInt<N>(u), 'u');
    return std::string(s.data());
  }

  template<int N> static std::ostream& operator<<(std::ostream &os, const BigUInt<N>& u) { return os << to_string(u); }

  template<int N>
  std::string to_string(const BigInt<N>& i) {
    std::vector<char> s((1 << N)/3 + 3);
    BigInt<N>::snprint(s.data(), s.size(), i, 'd');
    return std::string(s.data());
  }

  template<int N> static std::ostream& operator<<(std::ostream &os, const BigInt<N>& u) { return os << to_string(u); }
}
#endif // #ifndef NO_LIBSTDCXX
#endif // #ifndef __BIGINT_HPP_INCLUDED__
