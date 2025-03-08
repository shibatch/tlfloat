#ifndef __TLMATH_HPP_INCLUDED__
#define __TLMATH_HPP_INCLUDED__

#include <tlfloat/tlfloat.hpp>
#include <tlfloat/tlmathcoef.hpp>
#include <tlfloat/rpitab.hpp>

#if defined(__clang_major__) && !defined(_MSC_VER)
#if __clang_major__ < 17
#warning Unsupported clang version
#endif
#endif

namespace tlfloat {
  namespace detail {
    typedef UnpackedFloat<uint16_t, uint32_t, 0, 14> xhalf;
    typedef UnpackedFloat<uint32_t, uint64_t, 0, 30> xfloat;
    typedef UnpackedFloat<uint64_t, BigUInt<7>, 0, 62> xdouble;
    typedef UnpackedFloat<BigUInt<7>, BigUInt<8>, 0, 126> xquad;
    typedef UnpackedFloat<BigUInt<8>, BigUInt<9>, 0, 254> xoctuple;
    typedef UnpackedFloat<BigUInt<9>, BigUInt<10>, 0, 510> xsedecuple;

    //

    template<typename Unpacked_t>
    static consteval Unpacked_t constSqrt2O2() { return ldexp_(sqrt_(Unpacked_t(2)), -1); }

    template<typename Unpacked_t>
    static consteval Unpacked_t constOneOthree() { return Unpacked_t(1) / Unpacked_t(3); }

    template<typename Unpacked_t>
    static consteval Unpacked_t constRSqrtPI() { return Unpacked_t(1) / sqrt_(const_M_PI_<Unpacked_t>()); }

    template<typename T>
    static consteval T constLNPI() {
      return T("0x1.250d048e7a1bd0bd5f956c6a843f49985e6ddbf3b3f2606e33802ecaefa9308e5aa6c4df523160e6d2402c256db0b866738aa8787561937377769e99dceda3bfp+0");
    }

    template<typename T>
    static consteval T constLN2PI() {
      return T("0x1.d67f1c864beb4a6929792002883240479f611f1a268b169bbd8d46267b542aba425f3affc01d0d7f27d57f20b8aad37760b956ae3699b4b79ecdd9c2ee575be5p+0");
    }

    template<typename T>
    static consteval T constLN2() {
      return T("0x1.62e42fefa39ef35793c7673007e5ed5e81e6864ce5316c5b141a2eb71755f457cf70ec40dbd75930ab2aa5f695f43621da5d5c6b827042884eae765222d37048p-1");
    }

    template<typename T>
    static consteval T upper(T v) {
      typedef decltype(T::mant) mant_t;
      v.mant &= ~((mant_t(1) << (sizeof(mant_t)*8/2)) - 1);
      return v;
    }

    template<typename T>
    static consteval T constLN2U() { return upper(constLN2<T>()); }

    template<typename T>
    static consteval T constLN2L() {
      return (constLN2<xsedecuple>() - constLN2U<T>().cast((const xsedecuple *)nullptr)).cast((const T *)nullptr);
    }

    template<typename T>
    static consteval T constLN10() {
      return T("0x1.26bb1bbb5551582dd4adac5705a61451c51fd9f3b4bbf21d078c3d0403e05ae52c5e05af635020b998e1978058be1ad058c448308217c5b5f71ef10805ca2dacp+1");
    }

    template<typename T>
    static consteval T constRLN2() {
      return T("0x1.71547652b82fe1777d0ffda0d23a7d11d6aef551bad2b4b1164a2cd9a342648fbc3887eeaa2ed9ac49b25eeb82d7c167d52173cc1895213f897f5e06a7be7368p+0");
    }

    template<typename T>
    static consteval T constRLN10() {
      return T("0x1.bcb7b1526e50e32a6ab7555f5a67b8647dc68c048b934404747e5a89ef1d4a78ea849df42805e7e52446ab258c8c942a319c77b3fa71b978df4571a5919b4f6p-2");
    }

    template<typename T>
    static consteval T constCbrt2() {
      return T("0x1.428a2f98d728ae223ddab715be250d0c288f10291631fbc061800cc36fa2dd3a60b7d03da26f0840f25cc16b0f95075c30967a6b5ac6827ab31f06edf4a3486cp+0");
    }

    template<typename T>
    static consteval T constCbrt4() {
      return T("0x1.965fea53d6e3c82b05999ab43dc4def1980762158a0a815f2291ac0cf93041e5875f13657a43adc3896c0b95c0244e271d6f695f41523046622bc574b2caeaecp+0");
    }

    template<typename T>
    static consteval T constEuler() {
      return T("0x9.3c467e37db0c7a4d1be3f810152cb56a1cecc3af65cc0190c03df34709affbd8e4b59fa03a9f0eed0649ccb621057d11056ae9132135a08e43b4673d74bafea5p-4");
    }

    //

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genSinCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(sinCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+2> genCosCoef() {
      xarray<Unpacked_t, N+2> a;
      for(unsigned k=0;k<N+2;k++) a.e[k] = Unpacked_t(cosCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N> genTanCoef() {
      xarray<Unpacked_t, N> a;
      for(unsigned k=0;k<N;k++) a.e[k] = Unpacked_t(tanCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genAtanCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(atanCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genExpCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(expCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genLogCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(logCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genCbrtCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(cbrtCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genGammaCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(gammaCoefStr[k]);
      return a;
    }

    //

    template<typename Unpacked_t>
    static constexpr xpair<Unpacked_t, int> ph(const Unpacked_t &ux, int p) {
      typedef decltype(ux.mant) mant_t;
      typedef decltype(Unpacked_t::longmant_t_()) longmant_t;

      if (ux.iszero || ux.exp < Unpacked_t::expoffset()-5) return xpair<Unpacked_t, int>(ux, 0);

      const int mantexp = ux.exp - Unpacked_t::expoffset() - Unpacked_t::nbmant_() + 1;
      const int m = (mantexp - 1) < 0 ? 0 : (mantexp - 1);

      int rpiexp = p - (m & ~7) - sizeof(mant_t) * 8;
      longmant_t rpih = 0, rpil = 0;
      for(unsigned i=0;i<sizeof(mant_t);i++) rpih = (rpih << 8) + tlfloat_rpitab[i                  + m/8];
      for(unsigned i=0;i<sizeof(mant_t);i++) rpil = (rpil << 8) + tlfloat_rpitab[i + sizeof(mant_t) + m/8];

      longmant_t rm = ux.mant * rpih + ((ux.mant * rpil) >> (sizeof(mant_t) * 8));
      int q = 0;
      if (-(mantexp + rpiexp) < (int)sizeof(longmant_t)*8) {
	longmant_t qm = (rm + (longmant_t(1) << (-(mantexp + rpiexp) - 1))) & (~longmant_t(0) << -(mantexp + rpiexp));
	rm -= qm;
	q = int(qm >> -(mantexp + rpiexp));
      }

      int exp = mantexp + rpiexp - p;
      for(unsigned j=sizeof(mant_t) * 2;Unpacked_t::clz(rm) > sizeof(mant_t) * 8 || Unpacked_t::clz(~rm) > sizeof(mant_t) * 8;j += sizeof(mant_t)) {
	rm = (rm << (sizeof(mant_t) * 8)) + ((ux.mant * rpil) & ((longmant_t(1) << (sizeof(mant_t) * 8)) - 1));
	rpil = 0;
	for(unsigned i=0;i<sizeof(mant_t);i++) rpil = (rpil << 8) + tlfloat_rpitab[i + j + m/8];
	rm += ((ux.mant * rpil) >> (sizeof(mant_t) * 8));
	exp -= sizeof(mant_t) * 8;
      }

      bool sign = ux.sign;
      if (rm >> (sizeof(rm) * 8 - 1)) { rm = -rm; q = -q; sign = !sign; }

      int x = sizeof(rm) * 8 - Unpacked_t::clz(rm);
      int s = Unpacked_t::nbmant_() - (x - 1);
      mant_t u = mant_t(s >= 0 ? (rm << s) : ((rm + (mant_t(1) << (-s - 1))) >> -s));
      if (Unpacked_t::bit(u, Unpacked_t::nbmant_() + 1)) { u >>= 1; x++; }
      Unpacked_t r = Unpacked_t(u, x + Unpacked_t::expoffset() - 2 + exp, false, false, false, false) * const_M_PI_<Unpacked_t>();

      return xpair<Unpacked_t, int>(sign ? -r : r, sign ? -q & 0xff : q & 0xff);
    }

    //

    template<typename Unpacked_t, unsigned N>
    static constexpr Unpacked_t sin_(const Unpacked_t& a) {
      const xpair<Unpacked_t, int> p = ph<Unpacked_t>(a, 1);

      Unpacked_t d = Unpacked_t(0);

      if (p.second & 1) {
	constexpr xarray<Unpacked_t, N+2> cosCoef = genCosCoef<Unpacked_t, N>();
	d = cosCoef.e[N+1];
	for(int i=N-0;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, cosCoef.e[i]);
      } else {
	constexpr xarray<Unpacked_t, N+1> sinCoef = genSinCoef<Unpacked_t, N>();
	d = sinCoef.e[N];
	for(int i=N-1;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, sinCoef.e[i]);
	d *= p.first;
      }

      return (p.second & 2) ? -d : d;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N>
    static constexpr tlfloat_t sin(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto y = sin_<Unpacked_t, N>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(y.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t, unsigned N>
    static constexpr Unpacked_t cos_(const Unpacked_t& a) {
      const xpair<Unpacked_t, int> p = ph<Unpacked_t>(a, 1);

      Unpacked_t d = Unpacked_t(0);

      if (p.second & 1) {
	constexpr xarray<Unpacked_t, N+1> sinCoef = genSinCoef<Unpacked_t, N>();
	d = sinCoef.e[N];
	for(int i=N-1;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, sinCoef.e[i]);
	d *= -p.first;
      } else {
	constexpr xarray<Unpacked_t, N+2> cosCoef = genCosCoef<Unpacked_t, N>();
	d = cosCoef.e[N+1];
	for(int i=N-0;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, cosCoef.e[i]);
      }

      return (p.second & 2) ? -d : d;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N>
    static constexpr tlfloat_t cos(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto y = cos_<Unpacked_t, N>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(y.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr xpair<Unpacked_t, Unpacked_t> tan_(const Unpacked_t& a) {
      const xpair<Unpacked_t, int> p = ph<Unpacked_t>(a, 1);

      Unpacked_t x = ldexp_(p.first, -1-int(M)), y;

      constexpr xarray<Unpacked_t, N> tanCoef = genTanCoef<Unpacked_t, N>();
      Unpacked_t d = tanCoef.e[N-1];
      for(int i=N-1;i>0;i--) d = Unpacked_t::fma(d, x * x, tanCoef.e[i-1]);
      d *= x;

      if constexpr (M > 0) {
	for(unsigned i=0;i<M;i++) {
	  y = Unpacked_t::fma(d, d, Unpacked_t(-1));
	  x = -ldexp_(d, 1);
	  d = x / y;
	}
      }

      y = Unpacked_t::fma(d, d, Unpacked_t(-1));
      x = -ldexp_(d, 1);

      if (p.second & 1) { d = x; x = y; y = -d; }

      return xpair<Unpacked_t, Unpacked_t> { x, y };
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t tan(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto y = tan_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t((y.first / y.second).cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N>
    static constexpr Unpacked_t sinpi_(const Unpacked_t& a) {
      constexpr xarray<Unpacked_t, N+1> sinCoef = genSinCoef<Unpacked_t, N>();
      const Unpacked_t p = const_M_PI_<Unpacked_t>() * a;
      Unpacked_t d = sinCoef.e[N];
      for(int i=N-1;i>=0;i--) d = Unpacked_t::fma(d, -p * p, sinCoef.e[i]);
      return d * -p;
    }

    template<typename Unpacked_t, unsigned N>
    static constexpr Unpacked_t cospi_(const Unpacked_t& a) {
      constexpr xarray<Unpacked_t, N+2> cosCoef = genCosCoef<Unpacked_t, N>();
      const Unpacked_t p = const_M_PI_<Unpacked_t>() * a;
      Unpacked_t d = cosCoef.e[N+1];
      for(int i=N-0;i>=0;i--) d = Unpacked_t::fma(d, -p * p, cosCoef.e[i]);
      return d;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N>
    static constexpr tlfloat_t sinpi(const tlfloat_t& a) {
      if (isnan(a) || iszero(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto x = ldexp_(a.getUnpacked().cast((const Unpacked_t *)nullptr), 1), y = ldexp_(x - round(x), -1);
      int lx = intlsb(ldexp_(x, 1));
      if (((lx >= 0 && (lx & 3) == 0) || (lx < 0 && (lx & 3) == 3)) && isint(x)) return Unpacked_t::zero(x.sign);
      Unpacked_t z = ((lx+3)&2) ? sinpi_<Unpacked_t, N>(y) : cospi_<Unpacked_t, N>(y);
      if ((lx+7)&4) z.sign = !z.sign;
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N>
    static constexpr tlfloat_t cospi(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto x = ldexp_(a.getUnpacked().cast((const Unpacked_t *)nullptr), 1), y = ldexp_(x - round(x), -1);
      int lx = intlsb(ldexp_(x, 1));
      if (!((lx+3)&2) && y.iszero) return Unpacked_t::zero(false);
      Unpacked_t z = ((lx+3)&2) ? cospi_<Unpacked_t, N>(y) : sinpi_<Unpacked_t, N>(y);
      if ((lx+1)&4) z.sign = !z.sign;
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t tanpi(const tlfloat_t& a) {
      if (isnan(a) || iszero(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto x = ldexp_(a.getUnpacked().cast((const Unpacked_t *)nullptr), 1), y = ldexp_(x - round(x), -1);
      int lx = intlsb(ldexp_(x, 1));
      if (((lx >= 0 && (lx & 3) == 0) || (lx < 0 && (lx & 3) == 3)) && isint(x)) return Unpacked_t::zero(lx & 4);
      auto p = tan_<Unpacked_t, N, M>(const_M_PI_<Unpacked_t>() * y);
      auto z = ((lx+1)&2) ? p.second / p.first : p.first / p.second;
      if (z.isinf) z.sign = !(lx & 4);
      if ((lx+1)&2) z.sign = !z.sign;
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, int N, unsigned M>
    static constexpr Unpacked_t atan2_(Unpacked_t y, Unpacked_t x) {
      constexpr xarray<Unpacked_t, N+1> atanCoef = genAtanCoef<Unpacked_t, N>();
      int q = 0;
      if (fabs(y) > fabs(x)) {
	Unpacked_t t = x; x = y; y = -t;
	q = x.sign ? -1 : 1;
      } else if (x.sign) {
	q = y.sign? -2 : 2;
      }

      Unpacked_t d = y / x, r = atanCoef.e[N];

      for(unsigned i=0;i<M;i++) {
	d = d / (sqrt_(Unpacked_t::fma(d, d, Unpacked_t(1))) + Unpacked_t(1));
      }

      for(int i=N-1;i>=0;i--) r = Unpacked_t::fma(r, d * d, atanCoef.e[i]);
      r *= d;

      Unpacked_t qu = Unpacked_t(q);
      if (q == 0) qu.sign = r.sign;
      if (x.iszero) { r = Unpacked_t(0); qu.sign = r.sign = y.sign; }

      return Unpacked_t::fma(qu, ldexp_(const_M_PI_<Unpacked_t>(), -1),  ldexp_(r, M));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t atan2(tlfloat_t y, tlfloat_t x) {
      if (isnan(y)) return y;
      if (isnan(x)) return x;
      if (int(isinf(y)) | isinf(x)) {
	y = isinf(y) ? (signbit(y) ? -1 : 1) : (signbit(y) ? -0.0 : +0.0);
	x = isinf(x) ? (signbit(x) ? -1 : 1) : (signbit(x) ? -0.0 : +0.0);
      }

      Unpacked_t z = atan2_<Unpacked_t, N, M>(y.getUnpacked().cast((const Unpacked_t *)nullptr), x.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(x.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t atan(const tlfloat_t &a) {
      if (isnan(a)) return a;
      Unpacked_t y = isinf(a) ? Unpacked_t(signbit(a) ? -1 : 1) : a.getUnpacked().cast((const Unpacked_t *)nullptr);
      Unpacked_t x = Unpacked_t(isinf(a) ? 0 : 1);
      Unpacked_t z = atan2_<Unpacked_t, N, M>(y, x);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t asin(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (fabs(a) > 1) return tlfloat_t::nan();
      Unpacked_t y = a.getUnpacked().cast((const Unpacked_t *)nullptr);
      Unpacked_t x = sqrt_((Unpacked_t(1) + y) * (Unpacked_t(1) - y));
      Unpacked_t z = atan2_<Unpacked_t, N, M>(y, x);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t acos(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (fabs(a) > 1) return tlfloat_t::nan();
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr);
      Unpacked_t y = sqrt_((Unpacked_t(1) + x) * (Unpacked_t(1) - x));
      Unpacked_t z = atan2_<Unpacked_t, N, M>(y, x);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t exp_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> expCoef = genExpCoef<Unpacked_t, N+1>();

      if (d.exp > 28) return d.sign ? Unpacked_t(0) : Unpacked_t::infinity();

      int64_t q = rint(d * constRLN2<Unpacked_t>()).castToInt((int64_t *)nullptr);

      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2U<Unpacked_t>(), d);
      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2L<Unpacked_t>(), d);

      d = ldexp_(d, -M);

      Unpacked_t s = expCoef.e[N+1];
      for(int i=N;i>=0;i--) s = Unpacked_t::fma(s, d, expCoef.e[i]);

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) s *= s;

      return ldexp_(s, q);
    }

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t expm1_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> expCoef = genExpCoef<Unpacked_t, N+1>();

      if (d.iszero) return d;
      if (d.exp > 28) return d.sign ? Unpacked_t(-1) : Unpacked_t::infinity();

      int64_t q = rint(d * constRLN2<Unpacked_t>()).castToInt((int64_t *)nullptr);

      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2U<Unpacked_t>(), d);
      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2L<Unpacked_t>(), d);

      d = ldexp_(d, -M);

      Unpacked_t s = expCoef.e[N+1];
      for(int i=N;i>=1;i--) s = Unpacked_t::fma(s, d, expCoef.e[i]);
      s *= d;

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) s = Unpacked_t::fma(s, s, ldexp_(s, 1));

      return ldexp_(s, q) + (ldexp_(Unpacked_t(1), q) - Unpacked_t(1));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t exp(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (isinf(a) && signbit(a)) return 0;
      Unpacked_t z = exp_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t exp(const tlfloat_t &a, const Unpacked_t &SCALE) {
      if (isnan(a)) return a;
      if (isinf(a) && signbit(a)) return 0;
      Unpacked_t z = exp_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr) * SCALE);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t expm1(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (isinf(a) && signbit(a)) return -1;
      Unpacked_t z = expm1_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M, int T>
    static constexpr Unpacked_t log_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> logCoef = genLogCoef<Unpacked_t, N+1>();

      xpair<Unpacked_t, int> p = frexp_(d);
      Unpacked_t fr = p.first;
      int e = p.second, m = 0;

      if (fr < constSqrt2O2<Unpacked_t>()) { fr.exp++; e--; }

      if constexpr (M > 0) {
	if (fabs(fr - Unpacked_t(1)).ilogb() > T) {
	  for(unsigned i=0;i<M;i++) fr = sqrt_(fr);
	  m = M;
	}
      }

      Unpacked_t x = (fr - Unpacked_t(1)) / (fr + Unpacked_t(1));

      Unpacked_t y = logCoef.e[N+1];
      for(int i=N;i>=0;i--) y = Unpacked_t::fma(y, x*x, logCoef.e[i]);
      y *= x;

      y = Unpacked_t::fma(Unpacked_t(e), constLN2L<Unpacked_t>(), ldexp_(y, m));
      return Unpacked_t::fma(Unpacked_t(e), constLN2U<Unpacked_t>(), y);
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int T>
    static constexpr tlfloat_t log(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (a == 0) return tlfloat_t::infinity(true);
      if (signbit(a)) return tlfloat_t::nan();
      if (isinf(a)) return tlfloat_t::infinity();
      Unpacked_t z = log_<Unpacked_t, N, M, T>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int T>
    static constexpr tlfloat_t log(const tlfloat_t &a, const Unpacked_t &SCALE) {
      if (isnan(a)) return a;
      if (a == 0) return tlfloat_t::infinity(true);
      if (signbit(a)) return tlfloat_t::nan();
      if (isinf(a)) return tlfloat_t::infinity();
      Unpacked_t z = log_<Unpacked_t, N, M, T>(a.getUnpacked().cast((const Unpacked_t *)nullptr)) * SCALE;
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t log1p_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> logCoef = genLogCoef<Unpacked_t, N+1>();

      xpair<Unpacked_t, int> p = frexp_(d + Unpacked_t(1));
      Unpacked_t fr = p.first;
      int e = p.second;

      if (fr < constSqrt2O2<Unpacked_t>()) { fr.exp++; e--; }

      fr = e == 0 ? d : (fr - Unpacked_t(1));

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) fr = fr / (sqrt_(fr + Unpacked_t(1)) + Unpacked_t(1));

      Unpacked_t x = fr / (fr + Unpacked_t(2));

      Unpacked_t y = logCoef.e[N+1];
      for(int i=N;i>=0;i--) y = Unpacked_t::fma(y, x*x, logCoef.e[i]);
      y *= x;

      y = Unpacked_t::fma(Unpacked_t(e), constLN2L<Unpacked_t>(), ldexp_(y, M));
      return Unpacked_t::fma(Unpacked_t(e), constLN2U<Unpacked_t>(), y);
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t log1p(const tlfloat_t &a) {
      if (isnan(a) || a == 0) return a;
      if (a == -1) return tlfloat_t::infinity(true);
      if (a < -1) return tlfloat_t::nan();
      if (isinf(a)) return tlfloat_t::infinity();
      Unpacked_t z = log1p_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename tlfloat_t, typename Unpacked_t, unsigned LN, unsigned LM, int LT, unsigned EN, unsigned EM>
    static constexpr tlfloat_t pow(const tlfloat_t &x, const tlfloat_t &y) {
      if (signbit(x) && finite(x) && !iszero(x) && finite(y) && !isint(y)) return tlfloat_t::nan();
      if (x == 1 || iszero(y)) return 1;
      if (isnan(x)) return x;
      if (isnan(y)) return y;
      bool yisoddint = isint(y) && !iseven(y);
      if (!signbit(y) && !iszero(y) && iszero(x) && yisoddint) return tlfloat_t::zero(signbit(x));
      if (iszero(x) && !signbit(y) && !iszero(y) && !yisoddint) return 0;
      if (isinf(y) && x == -1) return 1;
      bool absxlt1 = -1 < x && x < 1;
      if (isinf(y) && signbit(y) && absxlt1) return tlfloat_t::infinity();
      bool absxgt1 = x < -1 || 1 < x;
      if (isinf(y) && signbit(y) && absxgt1) return 0;
      if (isinf(y) && !signbit(y) && absxlt1) return 0;
      if (isinf(y) && !signbit(y) && absxgt1) return tlfloat_t::infinity();
      if (isinf(x) && signbit(x) && yisoddint && signbit(y)) return tlfloat_t::zero(true);
      if (isinf(x) && signbit(x) && signbit(y) && !iszero(y) && !yisoddint) return 0;
      if (isinf(x) && signbit(x) && !signbit(y) && yisoddint) return tlfloat_t::infinity(true);
      if (isinf(x) && signbit(x) && !signbit(y) && !iszero(y) && !yisoddint) return tlfloat_t::infinity(false);
      if (isinf(x) && !signbit(x) && signbit(y) && !iszero(y)) return 0;
      if (isinf(x) && !signbit(x) && !signbit(y) && !iszero(y)) return tlfloat_t::infinity(false);
      if (iszero(x) && yisoddint && signbit(y)) return tlfloat_t::infinity(signbit(x));
      if (iszero(x) && signbit(y) && !iszero(y) && !yisoddint) return tlfloat_t::infinity(false);
      
      Unpacked_t z = log_<Unpacked_t, LN, LM, LT>(fabs(x).getUnpacked().cast((const Unpacked_t *)nullptr));
      z = exp_<Unpacked_t, EN, EM>(z * y.getUnpacked().cast((const Unpacked_t *)nullptr));
      if (signbit(x) && yisoddint) z.sign = !z.sign;
      return tlfloat_t(z.cast((const decltype(x.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t cbrt_(Unpacked_t d) {
      const int eoffset = 1 << 20;
      constexpr xarray<Unpacked_t, N+2> cbrtCoef = genCbrtCoef<Unpacked_t, N+1>();

      int e = (d * constOneOthree<Unpacked_t>()).ilogb() + 2;
      d.exp -= e;
      int r = (e + eoffset * 3) % 3;
      Unpacked_t q = Unpacked_t(1);
      if (r == 1) q = constCbrt2<Unpacked_t>();
      if (r == 2) q = constCbrt4<Unpacked_t>();

      q.exp += (e + eoffset * 3) / 3 - eoffset;

      if (d.sign) q.sign = !q.sign;
      d.sign = false;

      Unpacked_t x = cbrtCoef.e[N+1], y;
      for(int i=N;i>=0;i--) x = Unpacked_t::fma(x, d - Unpacked_t(1), cbrtCoef.e[i]);

      if constexpr (M > 0) {
	for(unsigned i=0;i<M;i++) {
	  y = x * x;
	  y = y * y;
	  x = Unpacked_t::fma(Unpacked_t::fma(d, y, -x), -constOneOthree<Unpacked_t>(), x);
	}
      }
      y = x * x * d;

      return Unpacked_t::fma(Unpacked_t::fma(y, x, Unpacked_t(-1)), -ldexp_(constOneOthree<Unpacked_t>(), 1) * y, y) * q;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t cbrt(const tlfloat_t &a) {
      if (iszero(a) || isinf(a) || isnan(a)) return a;
      Unpacked_t z = cbrt_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int O=20>
    static constexpr tlfloat_t sinh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan || x.isinf) return a;
      if (y > Unpacked_t::castFromInt(1ULL << O)) return tlfloat_t::infinity(x.sign);
      Unpacked_t d = expm1_<Unpacked_t, N, M>(fabs(x));
      d = ldexp_(d * (d + Unpacked_t(2)) / (d + Unpacked_t(1)), -1);
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int O=20>
    static constexpr tlfloat_t cosh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan) return a;
      if (x.iszero) return 1;
      if (y > Unpacked_t::castFromInt(1ULL << O)) return tlfloat_t::infinity();
      Unpacked_t d = exp_<Unpacked_t, N, M>(fabs(x));
      d = ldexp_(d + Unpacked_t(1) / d, -1);
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int O=20>
    static constexpr tlfloat_t tanh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan) return a;
      if (y.isinf || y > Unpacked_t::castFromInt(1ULL << O)) return x.sign ? -1 : 1;
      Unpacked_t d = expm1_<Unpacked_t, N, M>(ldexp_(y, 1));
      d = d / (d + Unpacked_t(2));
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int T>
    static constexpr tlfloat_t asinh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan || y.isinf) return a;
      bool ygt1 = y > Unpacked_t(1);
      Unpacked_t d = ygt1 ? Unpacked_t(1) / y : y;
      d = sqrt_(Unpacked_t::fma(d, d, Unpacked_t(1)));
      if (ygt1) {
	d = log_<Unpacked_t, N, M, T>(Unpacked_t::fma(d, y, y));
      } else {
	d = log1p_<Unpacked_t, N, M>(d - Unpacked_t(1) + y);
      }
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M, int T>
    static constexpr tlfloat_t acosh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr);
      if (x.isnan) return a;
      if (x < Unpacked_t(1)) return tlfloat_t::nan();
      if (x.isinf) return tlfloat_t::infinity();
      Unpacked_t d = log_<Unpacked_t, N, M, T>(sqrt_((x + Unpacked_t(1))*(x - Unpacked_t(1))) + x);
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t atanh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan) return a;
      if (x.isinf) return tlfloat_t::nan();
      if (y > Unpacked_t(1)) return tlfloat_t::nan();
      if (y == Unpacked_t(1)) return tlfloat_t::infinity(x.sign);
      Unpacked_t d = ldexp_(log1p_<Unpacked_t, N, M>(ldexp_(y, 1) / (Unpacked_t(1) - y)), -1);
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t>
    static constexpr Unpacked_t fmod_(const Unpacked_t &n, const Unpacked_t &d, const Unpacked_t &rd) {
      Unpacked_t r = n, d3 = d * Unpacked_t(3), d2 = ldexp_(d, 1);
      for(int i=0;i<(1 << 20);i++) {
	Unpacked_t q = trunc(toward0(r * rd));
	if (d3 > r && r >  d) q = Unpacked_t(2);
	if (d2 > r && r >= d) q = Unpacked_t(1);
	r = Unpacked_t::fma(q, -d, r);
	if (r < d) break;
      }
      return r;
    }

    template<typename tlfloat_t, typename Unpacked_t>
    static constexpr tlfloat_t fmod(const tlfloat_t &n, const tlfloat_t &d) {
      Unpacked_t un = n.getUnpacked().cast((Unpacked_t *)0);
      Unpacked_t ud = d.getUnpacked().cast((Unpacked_t *)0);
      if (un.isinf || ud.iszero) return tlfloat_t::nan();
      if (un.isnan || ud.isinf) return n;
      if (ud.isnan) return d;
      Unpacked_t ur = fmod_<Unpacked_t>(fabs(un), fabs(ud), toward0(Unpacked_t(1) / fabs(ud)));
      if (un.sign) ur.sign = !ur.sign;
      return tlfloat_t(ur.cast((decltype(n.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t>
    static constexpr Unpacked_t remainder_(const Unpacked_t &n, const Unpacked_t &d, const Unpacked_t &rd) {
      Unpacked_t r = n, d15 = d * ldexp_(Unpacked_t(3), -1), d05 = ldexp_(d, -1);
      bool qisodd = false;
      for(int i=0;i<(1 << 20);i++) {
	Unpacked_t q = rint(r * rd);
	if (fabs(r) < d15) q = Unpacked_t(r.sign ? -1 : 1);
	if (fabs(r) < d05 || (!qisodd && fabs(r) == d05)) q = Unpacked_t(0);
	if (q.iszero) break;
	qisodd = qisodd != (isint(q) && !iseven(q));
	r = Unpacked_t::fma(q, -d, r);
      }
      return r;
    }

    template<typename tlfloat_t, typename Unpacked_t>
    static constexpr tlfloat_t remainder(const tlfloat_t &n, const tlfloat_t &d) {
      Unpacked_t un = n.getUnpacked().cast((Unpacked_t *)0);
      Unpacked_t ud = d.getUnpacked().cast((Unpacked_t *)0);
      if (un.isinf || ud.iszero) return tlfloat_t::nan();
      if (un.isnan || ud.isinf) return n;
      if (ud.isnan) return d;
      Unpacked_t ur = remainder_<Unpacked_t>(fabs(un), fabs(ud), toward0(Unpacked_t(1) / fabs(ud)));
      if (un.sign) ur.sign = !ur.sign;
      return tlfloat_t(ur.cast((decltype(n.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t>
    static constexpr xpair<Unpacked_t, int64_t> remquo_(const Unpacked_t &n, const Unpacked_t &d, const Unpacked_t &rd) {
      Unpacked_t r = n, d15 = d * ldexp_(Unpacked_t(3), -1), d05 = ldexp_(d, -1);
      int64_t quo = 0;
      bool qisodd = false;
      for(int i=0;i<(1 << 20);i++) {
	Unpacked_t q = rint(r * rd);
	if (fabs(r) < d15) q = Unpacked_t(r.sign ? -1 : 1);
	if (fabs(r) < d05 || (!qisodd && fabs(r) == d05)) q = Unpacked_t(0);
	if (q.iszero) break;
	qisodd = qisodd != (isint(q) && !iseven(q));
	quo += q.sign ? -int64_t(q.castToInt2() & ((1ULL << 63)-1)) : int64_t(q.castToInt2() & ((1ULL << 63)-1));
	r = Unpacked_t::fma(q, -d, r);
      }
      quo &= (1ULL << 63)-1;
      return xpair<Unpacked_t, int64_t>(r, n.sign != d.sign ? -quo : quo);
    }

    template<typename tlfloat_t, typename Unpacked_t>
    static constexpr xpair<tlfloat_t, int64_t> remquo(const tlfloat_t &n, const tlfloat_t &d) {
      Unpacked_t un = n.getUnpacked().cast((Unpacked_t *)0);
      Unpacked_t ud = d.getUnpacked().cast((Unpacked_t *)0);
      if (un.isinf || ud.iszero) return xpair<tlfloat_t, int64_t>(tlfloat_t::nan(), 0);
      if (un.isnan || ud.isinf) return xpair<tlfloat_t, int64_t>(n, 0);
      if (ud.isnan) return xpair<tlfloat_t, int64_t>(d, 0);
      auto p = remquo_<Unpacked_t>(fabs(un), fabs(ud), toward0(Unpacked_t(1) / fabs(ud)));
      if (un.sign) p.first.sign = !p.first.sign;
      if (un.sign != ud.sign) p.second = -p.second; 
      return xpair<tlfloat_t, int64_t>(tlfloat_t(p.first.cast((decltype(n.getUnpacked()) *)nullptr)), p.second);
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned EN, unsigned EM>
    static constexpr Unpacked_t erf_(const Unpacked_t& z) {
      Unpacked_t n = Unpacked_t::castFromInt(N*2+1), d = Unpacked_t(1), t;
      for(int i=N;i>0;i-=2) {
	t = n;
	n = n*Unpacked_t(i*2-1) + z*z*Unpacked_t(i*2-0)*d;
	d = t;

	t = n;
	n = n*Unpacked_t(i*2-3) - z*z*Unpacked_t(i*2-2)*d;
	d = t;
      }
      d = ldexp_(d, 1);
      return z * constRSqrtPI<Unpacked_t>() * exp_<Unpacked_t, EN, EM>(-(z*z)) * (d / n);
    }

    template<typename Unpacked_t, unsigned N, unsigned EN, unsigned EM>
    static constexpr Unpacked_t erfc_(const Unpacked_t& z) {
      Unpacked_t n = Unpacked_t(1), d = n, t;
      for(int i=N;i>0;i-=2) {
	t = n;
	n = n + ldexp_(Unpacked_t(i), -1) * d;
	d = t;

	t = n;
	n = z * z * n + ldexp_(Unpacked_t(i - 1), -1) * d;
	d = t;
      }
      return z * constRSqrtPI<Unpacked_t>() * exp_<Unpacked_t, EN, EM>(-(z*z)) * (d / n);
    }

    template<typename tlfloat_t, typename Unpacked_t, int T, unsigned N, unsigned EN, unsigned EM>
    static constexpr tlfloat_t erf(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (isinf(a)) return signbit(a) ? -1 : 1;
      Unpacked_t z;
      if (a < (tlfloat_t)(-T * 0.1)) {
	z = Unpacked_t(-1) - erfc_<Unpacked_t, N, EN, EM>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      } else if (a < (tlfloat_t)(T * 0.1)) {
	z = erf_<Unpacked_t, N, EN, EM>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      } else {
	z = Unpacked_t(1) - erfc_<Unpacked_t, N, EN, EM>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      }
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, int T, unsigned N, unsigned EN, unsigned EM>
    static constexpr tlfloat_t erfc(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (isinf(a)) return signbit(a) ? 2 : 0;
      Unpacked_t z;
      if (a < (tlfloat_t)(-T * 0.1)) {
	z = Unpacked_t(2) + erfc_<Unpacked_t, N, EN, EM>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      } else if (a < (tlfloat_t)(T * 0.1)) {
	z = Unpacked_t(1) - erf_<Unpacked_t, N, EN, EM>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      } else {
	z = erfc_<Unpacked_t, N, EN, EM>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      }
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, int R, int O, unsigned SN, unsigned LN, unsigned LM, int LT>
    static constexpr xpair<bool, Unpacked_t> lgamma_(const Unpacked_t& z) {
      constexpr xarray<Unpacked_t, O+1> gammaCoef = genGammaCoef<Unpacked_t, O>();
      const Unpacked_t one = Unpacked_t(1), p5 = Unpacked_t("0x1p-1");

      const bool reflect = z < Unpacked_t(-1);
      Unpacked_t a = reflect ? one - z : z;
      const Unpacked_t in = rint(a), fr = a - in;

      Unpacked_t w = Unpacked_t(1);
      if (in < Unpacked_t(R)) {
	for(int i = in.castToInt((int *)0);i<R;i++) w *= (Unpacked_t(i) + fr);
	a = Unpacked_t(R) + fr;
      }

      Unpacked_t y = gammaCoef.e[O];
      for(int i=O-1;i>=0;i--) y = Unpacked_t::fma(y, one / (a*a), gammaCoef.e[i]);
      y = y/a + (a - p5)*log_<Unpacked_t, LN, LM, LT>(a) - a + p5*constLN2PI<Unpacked_t>();

      y -= log_<Unpacked_t, LN, LM, LT>(fabs(w));
      bool sign = w.sign;

      if (reflect) {
	Unpacked_t s = sin_<Unpacked_t, SN>(const_M_PI_<Unpacked_t>() * fr);
	y = constLNPI<Unpacked_t>() - log_<Unpacked_t, LN, LM, LT>(fabs(s)) - y;
	sign = sign != s.sign;
	sign = sign == iseven(in);
      }
      return xpair<bool, Unpacked_t>(sign, y);
    }

    template<typename tlfloat_t, typename Unpacked_t, int R, int O,
	     unsigned SN, unsigned LN, unsigned LM, int LT, unsigned EN, unsigned EM>
    static constexpr tlfloat_t tgamma(const tlfloat_t &z) {
      Unpacked_t zu = z.getUnpacked().cast((const Unpacked_t *)nullptr);
      if (zu.isnan || (!zu.sign && zu.isinf)) return z;
      if (zu.iszero) return tlfloat_t::infinity(zu.sign);
      if (zu.sign && isint(zu)) return tlfloat_t::nan();

      auto p = lgamma_<Unpacked_t, R, O, SN, LN, LM, LT>(zu);
      Unpacked_t r = exp_<Unpacked_t, EN, EM>(p.second);
      r.sign = r.sign != p.first;
      return tlfloat_t(r.cast((const decltype(z.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, int R, int O,
	     unsigned SN, unsigned LN, unsigned LM, int LT, unsigned EN, unsigned EM>
    static constexpr tlfloat_t lgamma(const tlfloat_t &z, bool *signptr = nullptr) {
      Unpacked_t zu = z.getUnpacked().cast((const Unpacked_t *)nullptr);
      if (signptr) *signptr = false;
      if (zu.isnan) return z;
      if (zu.iszero|| zu.isinf) { if (signptr) *signptr = zu.sign; return tlfloat_t::infinity(); }
      if (zu.sign && isint(zu)) return tlfloat_t::infinity();

      auto p = lgamma_<Unpacked_t, R, O, SN, LN, LM, LT>(zu);
      if (signptr) *signptr = p.first;
      return tlfloat_t(p.second.cast((const decltype(z.getUnpacked()) *)nullptr));
    }
  }

  //

  /** Returns 2^exp * PI */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_PI(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return detail::const_M_PI_<xUnpacked_t>(exp).cast((Unpacked_t *)0);
  }

  /** Returns 2^exp * log_2 (e) */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_LOG2E(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return ldexp_(detail::constRLN2<xUnpacked_t>(), exp).cast((Unpacked_t*)0);
  }

  /** Returns 2^exp * log_10 (e) */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_LOG10E(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return ldexp_(detail::constRLN10<xUnpacked_t>(), exp).cast((Unpacked_t*)0);
  }

  /** Returns 2^exp * log 2 */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_LN2(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return ldexp_(detail::constLN2<xUnpacked_t>(), exp).cast((Unpacked_t*)0);
  }

  /** Returns 2^exp * log 10 */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_LN10(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return ldexp_(detail::constLN10<xUnpacked_t>(), exp).cast((Unpacked_t*)0);
  }

  /** Returns 2^exp / PI */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_1_PI(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return (xUnpacked_t(1) / detail::const_M_PI_<xUnpacked_t>(-exp)).cast((Unpacked_t *)0);
  }

  /** Returns 2^exp / sqrt(PI) */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_1_SQRTPI(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return (ldexp_(xUnpacked_t(1), exp) / sqrt_(detail::const_M_PI_<xUnpacked_t>(0))).cast((Unpacked_t*)0);
  }

  /** Returns 2^exp * E */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_M_E(int exp = 0) {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return detail::const_M_E_<xUnpacked_t>(exp).cast((Unpacked_t *)0);
  }

  /** Returns the Euler–Mascheroni constant */
  template<typename tlfloat_t>
  static inline consteval tlfloat_t const_Euler() {
    typedef decltype(tlfloat_t::to_Unpacked_t()) Unpacked_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    return detail::constEuler<xUnpacked_t>().cast((Unpacked_t*)0);
  }

  //

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 sin(const BFloat16& a) { return detail::sin<BFloat16, detail::xhalf, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half sin(const Half& a) { return detail::sin<Half, detail::xhalf, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float sin(const Float& a) { return detail::sin<Float, detail::xfloat, 4>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double sin(const Double& a) { return detail::sin<Double, detail::xdouble, 9>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad sin(const Quad& a) { return detail::sin<Quad, detail::xquad, 14>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple sin(const Octuple& a) { return detail::sin<Octuple, detail::xoctuple, 25>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 cos(const BFloat16& a) { return detail::cos<BFloat16, detail::xhalf, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half cos(const Half& a) { return detail::cos<Half, detail::xhalf, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float cos(const Float& a) { return detail::cos<Float, detail::xfloat, 4>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double cos(const Double& a) { return detail::cos<Double, detail::xdouble, 9>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad cos(const Quad& a) { return detail::cos<Quad, detail::xquad, 14>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple cos(const Octuple& a) { return detail::cos<Octuple, detail::xoctuple, 25>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 tan(const BFloat16& a) { return detail::tan<BFloat16, detail::xhalf, 5, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half tan(const Half& a) { return detail::tan<Half, detail::xhalf, 5, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float tan(const Float& a) { return detail::tan<Float, detail::xfloat, 9, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double tan(const Double& a) { return detail::tan<Double, detail::xdouble, 11, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad tan(const Quad& a) { return detail::tan<Quad, detail::xquad, 16, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple tan(const Octuple& a) { return detail::tan<Octuple, detail::xoctuple, 25, 3>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 atan2(const BFloat16& y, const BFloat16& x) { return detail::atan2<BFloat16, detail::xhalf, 4, 1>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half atan2(const Half& y, const Half& x) { return detail::atan2<Half, detail::xhalf, 4, 1>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float atan2(const Float& y, const Float& x) { return detail::atan2<Float, detail::xfloat, 9, 1>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double atan2(const Double& y, const Double& x) { return detail::atan2<Double, detail::xdouble, 14, 2>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad atan2(const Quad& y, const Quad& x) { return detail::atan2<Quad, detail::xquad, 17, 3>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple atan2(const Octuple& y, const Octuple& x) { return detail::atan2<Octuple, detail::xoctuple, 21, 6>(y, x); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 asin(const BFloat16& a) { return detail::asin<BFloat16, detail::xhalf, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half asin(const Half& a) { return detail::asin<Half, detail::xhalf, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float asin(const Float& a) { return detail::asin<Float, detail::xfloat, 9, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double asin(const Double& a) { return detail::asin<Double, detail::xdouble, 14, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad asin(const Quad& a) { return detail::asin<Quad, detail::xquad, 17, 3>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple asin(const Octuple& a) { return detail::asin<Octuple, detail::xoctuple, 21, 6>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 acos(const BFloat16& a) { return detail::acos<BFloat16, detail::xhalf, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half acos(const Half& a) { return detail::acos<Half, detail::xhalf, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float acos(const Float& a) { return detail::acos<Float, detail::xfloat, 9, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double acos(const Double& a) { return detail::acos<Double, detail::xdouble, 14, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad acos(const Quad& a) { return detail::acos<Quad, detail::xquad, 17, 3>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple acos(const Octuple& a) { return detail::acos<Octuple, detail::xoctuple, 21, 6>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 atan(const BFloat16& a) { return detail::atan<BFloat16, detail::xhalf, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half atan(const Half& a) { return detail::atan<Half, detail::xhalf, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float atan(const Float& a) { return detail::atan<Float, detail::xfloat, 9, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double atan(const Double& a) { return detail::atan<Double, detail::xdouble, 14, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad atan(const Quad& a) { return detail::atan<Quad, detail::xquad, 17, 3>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple atan(const Octuple& a) { return detail::atan<Octuple, detail::xoctuple, 21, 6>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 exp(const BFloat16& a) { return detail::exp<BFloat16, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half exp(const Half& a) { return detail::exp<Half, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float exp(const Float& a) { return detail::exp<Float, detail::xfloat, 7, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double exp(const Double& a) { return detail::exp<Double, detail::xdouble, 13, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad exp(const Quad& a) { return detail::exp<Quad, detail::xquad, 20, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple exp(const Octuple& a) { return detail::exp<Octuple, detail::xoctuple, 33, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 expm1(const BFloat16& a) { return detail::expm1<BFloat16, detail::xhalf, 3, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half expm1(const Half& a) { return detail::expm1<Half, detail::xhalf, 3, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float expm1(const Float& a) { return detail::expm1<Float, detail::xfloat, 6, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double expm1(const Double& a) { return detail::expm1<Double, detail::xdouble, 12, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad expm1(const Quad& a) { return detail::expm1<Quad, detail::xquad, 21, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple expm1(const Octuple& a) { return detail::expm1<Octuple, detail::xoctuple, 33, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 exp2(const BFloat16& a) { return detail::exp<BFloat16, detail::xfloat, 3, 0>(a, detail::constLN2<detail::xfloat>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half exp2(const Half& a) { return detail::exp<Half, detail::xfloat, 3, 0>(a, detail::constLN2<detail::xfloat>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float exp2(const Float& a) { return detail::exp<Float, detail::xdouble, 7, 0>(a, detail::constLN2<detail::xdouble>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double exp2(const Double& a) { return detail::exp<Double, detail::xquad, 13, 0>(a, detail::constLN2<detail::xquad>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad exp2(const Quad& a) { return detail::exp<Quad, detail::xoctuple, 20, 1>(a, detail::constLN2<detail::xoctuple>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple exp2(const Octuple& a) { return detail::exp<Octuple, detail::xsedecuple, 33, 2>(a, detail::constLN2<detail::xsedecuple>()); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 exp10(const BFloat16& a) { return detail::exp<BFloat16, detail::xfloat, 3, 0>(a, detail::constLN10<detail::xfloat>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half exp10(const Half& a) { return detail::exp<Half, detail::xfloat, 3, 0>(a, detail::constLN10<detail::xfloat>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float exp10(const Float& a) { return detail::exp<Float, detail::xdouble, 7, 0>(a, detail::constLN10<detail::xdouble>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double exp10(const Double& a) { return detail::exp<Double, detail::xquad, 13, 0>(a, detail::constLN10<detail::xquad>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad exp10(const Quad& a) { return detail::exp<Quad, detail::xoctuple, 20, 1>(a, detail::constLN10<detail::xoctuple>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple exp10(const Octuple& a) { return detail::exp<Octuple, detail::xsedecuple, 33, 2>(a, detail::constLN10<detail::xsedecuple>()); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 log(const BFloat16& a) { return detail::log<BFloat16, detail::xhalf, 3, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half log(const Half& a) { return detail::log<Half, detail::xhalf, 3, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float log(const Float& a) { return detail::log<Float, detail::xfloat, 4, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double log(const Double& a) { return detail::log<Double, detail::xdouble, 10, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad log(const Quad& a) { return detail::log<Quad, detail::xquad, 21, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple log(const Octuple& a) { return detail::log<Octuple, detail::xoctuple, 25, 2, -15>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 log1p(const BFloat16& a) { return detail::log1p<BFloat16, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half log1p(const Half& a) { return detail::log1p<Half, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float log1p(const Float& a) { return detail::log1p<Float, detail::xfloat, 4, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double log1p(const Double& a) { return detail::log1p<Double, detail::xdouble, 10, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad log1p(const Quad& a) { return detail::log1p<Quad, detail::xquad, 21, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple log1p(const Octuple& a) { return detail::log1p<Octuple, detail::xoctuple, 25, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 log2(const BFloat16& a) { return detail::log<BFloat16, detail::xhalf, 3, 0, 0>(a, detail::constRLN2<detail::xhalf>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half log2(const Half& a) { return detail::log<Half, detail::xhalf, 3, 0, 0>(a, detail::constRLN2<detail::xhalf>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float log2(const Float& a) { return detail::log<Float, detail::xfloat, 4, 0, 0>(a, detail::constRLN2<detail::xfloat>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double log2(const Double& a) { return detail::log<Double, detail::xdouble, 10, 0, 0>(a, detail::constRLN2<detail::xdouble>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad log2(const Quad& a) { return detail::log<Quad, detail::xquad, 22, 0, 0>(a, detail::constRLN2<detail::xquad>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple log2(const Octuple& a) { return detail::log<Octuple, detail::xoctuple, 25, 2, -15>(a, detail::constRLN2<detail::xoctuple>()); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 log10(const BFloat16& a) { return detail::log<BFloat16, detail::xhalf, 3, 0, 0>(a, detail::constRLN10<detail::xhalf>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half log10(const Half& a) { return detail::log<Half, detail::xhalf, 3, 0, 0>(a, detail::constRLN10<detail::xhalf>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float log10(const Float& a) { return detail::log<Float, detail::xfloat, 4, 0, 0>(a, detail::constRLN10<detail::xfloat>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double log10(const Double& a) { return detail::log<Double, detail::xdouble, 10, 0, 0>(a, detail::constRLN10<detail::xdouble>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad log10(const Quad& a) { return detail::log<Quad, detail::xquad, 22, 0, 0>(a, detail::constRLN10<detail::xquad>()); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple log10(const Octuple& a) { return detail::log<Octuple, detail::xoctuple, 25, 2, -15>(a, detail::constRLN10<detail::xoctuple>()); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 pow(const BFloat16& x, const BFloat16& y) { return detail::pow<BFloat16, detail::xfloat, 3, 0, 0, 3, 0>(x, y); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half pow(const Half& x, const Half& y) { return detail::pow<Half, detail::xfloat, 3, 0, 0, 3, 0>(x, y); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float pow(const Float& x, const Float& y) { return detail::pow<Float, detail::xdouble, 5, 0, 0, 7, 0>(x, y); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double pow(const Double& x, const Double& y) { return detail::pow<Double, detail::xquad, 11, 0, 0, 13, 0>(x, y); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad pow(const Quad& x, const Quad& y) { return detail::pow<Quad, detail::xoctuple, 23, 0, 0, 20, 1>(x, y); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple pow(const Octuple& x, const Octuple& y) { return detail::pow<Octuple, detail::xsedecuple, 26, 2, -15, 33, 2>(x, y); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 cbrt(const BFloat16& a) { return detail::cbrt<BFloat16, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half cbrt(const Half& a) { return detail::cbrt<Half, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float cbrt(const Float& a) { return detail::cbrt<Float, detail::xfloat, 4, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double cbrt(const Double& a) { return detail::cbrt<Double, detail::xdouble, 4, 2>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad cbrt(const Quad& a) { return detail::cbrt<Quad, detail::xquad, 4, 3>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple cbrt(const Octuple& a) { return detail::cbrt<Octuple, detail::xoctuple, 4, 4>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 sinh(const BFloat16& a) { return detail::sinh<BFloat16, detail::xhalf, 4, 0, 10>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half sinh(const Half& a) { return detail::sinh<Half, detail::xhalf, 4, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float sinh(const Float& a) { return detail::sinh<Float, detail::xfloat, 7, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double sinh(const Double& a) { return detail::sinh<Double, detail::xdouble, 13, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad sinh(const Quad& a) { return detail::sinh<Quad, detail::xquad, 20, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple sinh(const Octuple& a) { return detail::sinh<Octuple, detail::xoctuple, 33, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 cosh(const BFloat16& a) { return detail::cosh<BFloat16, detail::xhalf, 3, 0, 10>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half cosh(const Half& a) { return detail::cosh<Half, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float cosh(const Float& a) { return detail::cosh<Float, detail::xfloat, 7, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double cosh(const Double& a) { return detail::cosh<Double, detail::xdouble, 13, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad cosh(const Quad& a) { return detail::cosh<Quad, detail::xquad, 20, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple cosh(const Octuple& a) { return detail::cosh<Octuple, detail::xoctuple, 33, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 tanh(const BFloat16& a) { return detail::tanh<BFloat16, detail::xhalf, 3, 0, 10>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half tanh(const Half& a) { return detail::tanh<Half, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float tanh(const Float& a) { return detail::tanh<Float, detail::xfloat, 7, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double tanh(const Double& a) { return detail::tanh<Double, detail::xdouble, 13, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad tanh(const Quad& a) { return detail::tanh<Quad, detail::xquad, 20, 1>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple tanh(const Octuple& a) { return detail::tanh<Octuple, detail::xoctuple, 33, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 asinh(const BFloat16& a) { return detail::asinh<BFloat16, detail::xfloat, 3, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half asinh(const Half& a) { return detail::asinh<Half, detail::xfloat, 3, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float asinh(const Float& a) { return detail::asinh<Float, detail::xdouble, 7, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double asinh(const Double& a) { return detail::asinh<Double, detail::xquad, 13, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad asinh(const Quad& a) { return detail::asinh<Quad, detail::xoctuple, 20, 1, -10>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple asinh(const Octuple& a) { return detail::asinh<Octuple, detail::xsedecuple, 33, 2, -15>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 acosh(const BFloat16& a) { return detail::acosh<BFloat16, detail::xfloat, 3, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half acosh(const Half& a) { return detail::acosh<Half, detail::xfloat, 3, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float acosh(const Float& a) { return detail::acosh<Float, detail::xdouble, 7, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double acosh(const Double& a) { return detail::acosh<Double, detail::xquad, 13, 0, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad acosh(const Quad& a) { return detail::acosh<Quad, detail::xoctuple, 20, 1, -10>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple acosh(const Octuple& a) { return detail::acosh<Octuple, detail::xsedecuple, 33, 2, -15>(a); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 atanh(const BFloat16& a) { return detail::atanh<BFloat16, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half atanh(const Half& a) { return detail::atanh<Half, detail::xhalf, 3, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float atanh(const Float& a) { return detail::atanh<Float, detail::xfloat, 4, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double atanh(const Double& a) { return detail::atanh<Double, detail::xdouble, 10, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad atanh(const Quad& a) { return detail::atanh<Quad, detail::xquad, 21, 0>(a); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple atanh(const Octuple& a) { return detail::atanh<Octuple, detail::xoctuple, 25, 2>(a); }

  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr BFloat16 fmod(const BFloat16& y, const BFloat16& x) { return detail::fmod<BFloat16, detail::xhalf>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Half fmod(const Half& y, const Half& x) { return detail::fmod<Half, detail::xhalf>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Float fmod(const Float& y, const Float& x) { return detail::fmod<Float, detail::xfloat>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Double fmod(const Double& y, const Double& x) { return detail::fmod<Double, detail::xdouble>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Quad fmod(const Quad& y, const Quad& x) { return detail::fmod<Quad, detail::xquad>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Octuple fmod(const Octuple& y, const Octuple& x) { return detail::fmod<Octuple, detail::xoctuple>(y, x); }

  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr BFloat16 remainder(const BFloat16& y, const BFloat16& x) { return detail::remainder<BFloat16, detail::xhalf>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Half remainder(const Half& y, const Half& x) { return detail::remainder<Half, detail::xhalf>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Float remainder(const Float& y, const Float& x) { return detail::remainder<Float, detail::xfloat>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Double remainder(const Double& y, const Double& x) { return detail::remainder<Double, detail::xdouble>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Quad remainder(const Quad& y, const Quad& x) { return detail::remainder<Quad, detail::xquad>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr Octuple remainder(const Octuple& y, const Octuple& x) { return detail::remainder<Octuple, detail::xoctuple>(y, x); }

  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr xpair<BFloat16, int64_t> remquo(const BFloat16& y, const BFloat16& x) { return detail::remquo<BFloat16, detail::xhalf>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr xpair<Half, int64_t> remquo(const Half& y, const Half& x) { return detail::remquo<Half, detail::xhalf>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr xpair<Float, int64_t> remquo(const Float& y, const Float& x) { return detail::remquo<Float, detail::xfloat>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr xpair<Double, int64_t> remquo(const Double& y, const Double& x) { return detail::remquo<Double, detail::xdouble>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr xpair<Quad, int64_t> remquo(const Quad& y, const Quad& x) { return detail::remquo<Quad, detail::xquad>(y, x); }
  /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
  static inline constexpr xpair<Octuple, int64_t> remquo(const Octuple& y, const Octuple& x) { return detail::remquo<Octuple, detail::xoctuple>(y, x); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 erf(const BFloat16& x) { return detail::erf<BFloat16, detail::xfloat, 16, 10, 5, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half erf(const Half& x) { return detail::erf<Half, detail::xfloat, 16, 10, 5, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float erf(const Float& x) { return detail::erf<Float, detail::xdouble, 20, 20, 8, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double erf(const Double& x) { return detail::erf<Double, detail::xquad, 22, 86, 15, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad erf(const Quad& x) { return detail::erf<Quad, detail::xoctuple, 31, 120, 22, 1>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple erf(const Octuple& x) { return detail::erf<Octuple, detail::xsedecuple, 36, 340, 36, 2>(x); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 erfc(const BFloat16& x) { return detail::erfc<BFloat16, detail::xfloat, 16, 10, 5, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half erfc(const Half& x) { return detail::erfc<Half, detail::xfloat, 16, 10, 5, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float erfc(const Float& x) { return detail::erfc<Float, detail::xdouble, 20, 20, 8, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double erfc(const Double& x) { return detail::erfc<Double, detail::xquad, 22, 86, 15, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad erfc(const Quad& x) { return detail::erfc<Quad, detail::xoctuple, 31, 120, 22, 1>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple erfc(const Octuple& x) { return detail::erfc<Octuple, detail::xsedecuple, 36, 340, 36, 2>(x); }

  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 tgamma(const BFloat16& x) { return detail::tgamma<BFloat16, detail::xfloat, 5, 20, 3, 3, 0, 0, 3, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Half tgamma(const Half& x) { return detail::tgamma<Half, detail::xfloat, 5, 20, 3, 3, 0, 0, 3, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Float tgamma(const Float& x) { return detail::tgamma<Float, detail::xdouble, 7, 15, 5, 5, 0, 0, 7, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Double tgamma(const Double& x) { return detail::tgamma<Double, detail::xquad, 8, 30, 10, 11, 0, 0, 13, 0>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Quad tgamma(const Quad& x) { return detail::tgamma<Quad, detail::xoctuple, 20, 35, 15, 23, 0, 0, 20, 1>(x); }
  /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple tgamma(const Octuple& x) { return detail::tgamma<Octuple, detail::xsedecuple, 50, 35, 26, 26, 2, -15, 33, 2>(x); }

  /** This function has the same functionality as the corresponding function in math.h. This implementation is experimental and has no error bound. */
  static inline constexpr BFloat16 lgamma(const BFloat16& x, bool *sign = 0) { return detail::lgamma<BFloat16, detail::xfloat, 5, 20, 3, 3, 0, 0, 3, 0>(x, sign); }
  /** This function has the same functionality as the corresponding function in math.h. This implementation is experimental and has no error bound. */
  static inline constexpr Half lgamma(const Half& x, bool *sign = 0) { return detail::lgamma<Half, detail::xfloat, 5, 20, 3, 3, 0, 0, 3, 0>(x, sign); }
  /** This function has the same functionality as the corresponding function in math.h. This implementation is experimental and has no error bound. */
  static inline constexpr Float lgamma(const Float& x, bool *sign = 0) { return detail::lgamma<Float, detail::xdouble, 7, 15, 5, 5, 0, 0, 7, 0>(x, sign); }
  /** This function has the same functionality as the corresponding function in math.h. This implementation is experimental and has no error bound. */
  static inline constexpr Double lgamma(const Double& x, bool *sign = 0) { return detail::lgamma<Double, detail::xquad, 8, 30, 10, 11, 0, 0, 13, 0>(x, sign); }
  /** This function has the same functionality as the corresponding function in math.h. This implementation is experimental and has no error bound. */
  static inline constexpr Quad lgamma(const Quad& x, bool *sign = 0) { return detail::lgamma<Quad, detail::xoctuple, 20, 35, 15, 23, 0, 0, 20, 1>(x, sign); }
  /** This function has the same functionality as the corresponding function in math.h. This implementation is experimental and has no error bound. */
  static inline constexpr Octuple lgamma(const Octuple& x, bool *sign = 0) { return detail::lgamma<Octuple, detail::xsedecuple, 50, 35, 26, 26, 2, -15, 33, 2>(x, sign); }

  /** This function returns sin(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 sinpi(const BFloat16& a) { return detail::sinpi<BFloat16, detail::xhalf, 2>(a); }
  /** This function returns sin(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Half sinpi(const Half& a) { return detail::sinpi<Half, detail::xhalf, 2>(a); }
  /** This function returns sin(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Float sinpi(const Float& a) { return detail::sinpi<Float, detail::xfloat, 4>(a); }
  /** This function returns sin(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Double sinpi(const Double& a) { return detail::sinpi<Double, detail::xdouble, 9>(a); }
  /** This function returns sin(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Quad sinpi(const Quad& a) { return detail::sinpi<Quad, detail::xquad, 14>(a); }
  /** This function returns sin(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple sinpi(const Octuple& a) { return detail::sinpi<Octuple, detail::xoctuple, 25>(a); }

  /** This function returns cos(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 cospi(const BFloat16& a) { return detail::cospi<BFloat16, detail::xhalf, 2>(a); }
  /** This function returns cos(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Half cospi(const Half& a) { return detail::cospi<Half, detail::xhalf, 2>(a); }
  /** This function returns cos(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Float cospi(const Float& a) { return detail::cospi<Float, detail::xfloat, 4>(a); }
  /** This function returns cos(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Double cospi(const Double& a) { return detail::cospi<Double, detail::xdouble, 9>(a); }
  /** This function returns cos(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Quad cospi(const Quad& a) { return detail::cospi<Quad, detail::xquad, 14>(a); }
  /** This function returns cos(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple cospi(const Octuple& a) { return detail::cospi<Octuple, detail::xoctuple, 25>(a); }

  /** This function returns tan(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr BFloat16 tanpi(const BFloat16& a) { return detail::tanpi<BFloat16, detail::xhalf, 5, 0>(a); }
  /** This function returns tan(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Half tanpi(const Half& a) { return detail::tanpi<Half, detail::xhalf, 5, 0>(a); }
  /** This function returns tan(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Float tanpi(const Float& a) { return detail::tanpi<Float, detail::xfloat, 9, 0>(a); }
  /** This function returns tan(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Double tanpi(const Double& a) { return detail::tanpi<Double, detail::xdouble, 11, 1>(a); }
  /** This function returns tan(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Quad tanpi(const Quad& a) { return detail::tanpi<Quad, detail::xquad, 16, 2>(a); }
  /** This function returns tan(PI * x). The accuracy of the return value is 1ULP. */
  static inline constexpr Octuple tanpi(const Octuple& a) { return detail::tanpi<Octuple, detail::xoctuple, 25, 3>(a); }

  static inline constexpr xpair<BFloat16, BFloat16> sincos(const BFloat16& a) {
    return { detail::sin<BFloat16, detail::xhalf, 2>(a), detail::cos<BFloat16, detail::xhalf, 2>(a) };
  }
  static inline constexpr xpair<Half, Half> sincos(const Half& a) {
    return { detail::sin<Half, detail::xhalf, 2>(a), detail::cos<Half, detail::xhalf, 2>(a) };
  }
  static inline constexpr xpair<Float, Float> sincos(const Float& a) {
    return { detail::sin<Float, detail::xfloat, 4>(a), detail::cos<Float, detail::xfloat, 4>(a) };
  }
  static inline constexpr xpair<Double, Double> sincos(const Double& a) {
    return { detail::sin<Double, detail::xdouble, 9>(a), detail::cos<Double, detail::xdouble, 9>(a) };
  }
  static inline constexpr xpair<Quad, Quad> sincos(const Quad& a) {
    return { detail::sin<Quad, detail::xquad, 14>(a), detail::cos<Quad, detail::xquad, 14>(a) };
  }
  static inline constexpr xpair<Octuple, Octuple> sincos(const Octuple& a) {
    return { detail::sin<Octuple, detail::xoctuple, 25>(a), detail::cos<Octuple, detail::xoctuple, 25>(a) };
  }

  static inline constexpr xpair<BFloat16, BFloat16> sincospi(const BFloat16& a) {
    return { detail::sinpi<BFloat16, detail::xhalf, 2>(a), detail::cospi<BFloat16, detail::xhalf, 2>(a) };
  }
  static inline constexpr xpair<Half, Half> sincospi(const Half& a) {
    return { detail::sinpi<Half, detail::xhalf, 2>(a), detail::cospi<Half, detail::xhalf, 2>(a) };
  }
  static inline constexpr xpair<Float, Float> sincospi(const Float& a) {
    return { detail::sinpi<Float, detail::xfloat, 4>(a), detail::cospi<Float, detail::xfloat, 4>(a) };
  }
  static inline constexpr xpair<Double, Double> sincospi(const Double& a) {
    return { detail::sinpi<Double, detail::xdouble, 9>(a), detail::cospi<Double, detail::xdouble, 9>(a) };
  }
  static inline constexpr xpair<Quad, Quad> sincospi(const Quad& a) {
    return { detail::sinpi<Quad, detail::xquad, 14>(a), detail::cospi<Quad, detail::xquad, 14>(a) };
  }
  static inline constexpr xpair<Octuple, Octuple> sincospi(const Octuple& a) {
    return { detail::sinpi<Octuple, detail::xoctuple, 25>(a), detail::cospi<Octuple, detail::xoctuple, 25>(a) };
  }
}
#endif // #ifndef __TLMATH_HPP_INCLUDED__
