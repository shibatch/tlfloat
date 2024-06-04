#include <iostream>
#include <cstdint>
#include <cstdlib>

#include "tlfloat/tlfloat.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

typedef UnpackedFloat<BigUInt<8>, BigUInt<9>, 0, 254> xoctuple;
typedef UnpackedFloat<BigUInt<9>, BigUInt<10>, 0, 510> xsedecuple;

template<typename T, int N>
struct yarray {
  T e[N];
  int n() { return N; }
};

template<unsigned N>
static yarray<xsedecuple, N+1> precomputeFactorial() {
  yarray<xsedecuple, N+1> a;
  a.e[0] = xsedecuple::castFromInt(1);
  for(unsigned i=1;i<N+1;i++) a.e[i] = a.e[i-1] * xsedecuple::castFromInt(i);
  return a;
}

template<unsigned N>
static yarray<xsedecuple, N+1> aFactorial = precomputeFactorial<N>();

template<unsigned N>
static xsedecuple factorial(unsigned n) { return aFactorial<N>.e[n]; }

template<unsigned N>
static xsedecuple binomial(unsigned n, unsigned k) {
  return factorial<N>(n)/(factorial<N>(k)*factorial<N>(n-k));
}

template<unsigned N>
static xsedecuple binomial(const xsedecuple &x, unsigned k) {
  xsedecuple d = xsedecuple::castFromInt(1);
  for(unsigned i=0;i<k;i++) d *= x - xsedecuple::castFromInt(i);
  return d / factorial<N>(k);
}

template<unsigned N>
static yarray<xsedecuple, N+1> precomputeBernoulli() {
  yarray<xsedecuple, N+1> a;
  a.e[0] = xsedecuple::castFromInt(1);
  for(unsigned n=1;n<=N;n++) {
    xsedecuple s = xsedecuple::castFromInt(0);
    for(unsigned k=0;k<n;k++)
      s += binomial<N+1>(n+1, k) * a.e[k];
    a.e[n] = -s / xsedecuple::castFromInt(n + 1);
  }
  return a;
}

template<unsigned N>
static xsedecuple bernoulli(unsigned n) {
  const yarray<xsedecuple, N+1> pc = precomputeBernoulli<N>();
  return pc.e[n];
}

template<unsigned N>
static yarray<xsedecuple, N+1> genSinCoef() {
  yarray<xsedecuple, N+1> a;
  for(unsigned i=0;i<N+1;i++) a.e[i] = xsedecuple::castFromInt(1) / factorial<N*2+2>(i*2+1);
  return a;
}

template<unsigned N>
static yarray<xsedecuple, N+2> genCosCoef() {
  yarray<xsedecuple, N+2> a;
  for(unsigned i=0;i<N+2;i++) a.e[i] = xsedecuple::castFromInt(1) / factorial<N*2+2>(i*2+0);
  return a;
}

template<unsigned N>
static yarray<xsedecuple, N> genTanCoef() {
  yarray<xsedecuple, N> a;
  for(unsigned k=1;k<=N;k++) {
    xsedecuple p4 = ldexp_(xsedecuple::castFromInt(1), 2*k);
    xsedecuple c = (p4 * (p4 - xsedecuple::castFromInt(1)) * bernoulli<N*2>(2*k)) / factorial<N*2>(2*k);
    if (!(k & 1)) c = -c;
    a.e[k-1] = c;
  }
  return a;
}

template<unsigned N>
static constexpr yarray<xsedecuple, N+1> genAtanCoef() {
  yarray<xsedecuple, N+1> a;
  for(unsigned i=0;i<N+1;i++) {
    a.e[i] = xsedecuple::castFromInt((i & 1) ? -1 : 1) / xsedecuple::castFromInt(1 + i * 2);
  }
  return a;
}

template<unsigned N>
static constexpr yarray<xsedecuple, N+1> genExpCoef() {
  yarray<xsedecuple, N+1> a;
  for(unsigned i=0;i<N+1;i++) a.e[i] = xsedecuple::castFromInt(1) / factorial<N>(i);
  return a;
}

template<unsigned N>
static constexpr yarray<xsedecuple, N+1> genLogCoef() {
  yarray<xsedecuple, N+1> a;
  for(unsigned i=0;i<N+1;i++) a.e[i] = xsedecuple::castFromInt(2) / xsedecuple::castFromInt(2 * i + 1);
  return a;
}

template<unsigned N>
static constexpr yarray<xsedecuple, N+1> genCbrtCoef() {
  yarray<xsedecuple, N+1> a;
  for(unsigned i=0;i<N+1;i++)
    a.e[i] = binomial<N+1>(xsedecuple::castFromInt(-1) / xsedecuple::castFromInt(3), i);
  return a;
}

int main(int argc, char **argv) {
  char buf[1024];

  if (argc > 1) {
    xsedecuple x(argv[1]);
    snprint(buf, sizeof(buf), x, 'a', 0, -1);
    cout << "\"" << buf << "\",  // " << to_double(x) << endl;
    x.mant &= ~((BigUInt<9>(1) << 250) - 1);
    snprint(buf, sizeof(buf), x, 'a', 0, -1);
    cout << "\"" << buf << "\",  // " << to_double(x) << endl;
    return 0;
  }

  cout << "namespace tlfloat {" << endl;
  cout << "  namespace detail {" << endl;

  {
    auto coef = genSinCoef<26>();

    cout << "    static constexpr const char *sinCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    auto coef = genCosCoef<26>();

    cout << "    static constexpr const char *cosCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    auto coef = genTanCoef<26>();

    cout << "    static constexpr const char *tanCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    auto coef = genAtanCoef<22>();

    cout << "    static constexpr const char *atanCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    auto coef = genExpCoef<36+1>();

    cout << "    static constexpr const char *expCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    auto coef = genLogCoef<33+1>();

    cout << "    static constexpr const char *logCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    auto coef = genCbrtCoef<4+1>();

    cout << "    static constexpr const char *cbrtCoefStr[] = {" << endl;

    for(int i=0;i<coef.n();i++) {
      auto o = coef.e[i];
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << endl;

  {
    cout << "    static constexpr const char *gammaCoefStr[] = {" << endl;

    static const int N = 36;

    for(int i=1;i<=N;i++) {
      auto n = bernoulli<N*2>(i*2);
      auto d = xsedecuple::castFromInt((2*i) * (2*i-1));

      auto o = n / d;
      o.mant &= ~((BigUInt<9>(1) << 250) - 1);
      snprint(buf, sizeof(buf), o, 'a', 0, -1);
      cout << "      \"" << buf << "\",  // " << to_double(o) << endl;
    }

    cout << "    };" << endl;
  }

  cout << "  }" << endl;
  cout << "}" << endl;
}
