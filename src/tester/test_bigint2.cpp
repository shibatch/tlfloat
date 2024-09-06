#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cstdio>

#include "suppress.hpp"

#include "tlfloat/bigint.hpp"
#include "testerutil.hpp"

#define M 9

using namespace std;
using namespace tlfloat;

static_assert(is_trivially_copyable_v<BigUInt<6>> == true);
static_assert(is_trivially_copyable_v<BigUInt<7>> == true);
static_assert(is_trivially_copyable_v<BigUInt<8>> == true);
static_assert(is_trivially_copyable_v<BigUInt<9>> == true);
static_assert(is_trivially_copyable_v<BigUInt<10>> == true);
static_assert(is_trivially_copyable_v<BigInt<7>> == true);
static_assert(is_trivially_copyable_v<BigInt<8>> == true);
static_assert(is_trivially_copyable_v<BigInt<9>> == true);
static_assert(is_trivially_copyable_v<BigInt<10>> == true);

template<int N>
xpair<BigUInt<N+1>, BigUInt<N+1>> xdivmod2(BigUInt<N> n, BigUInt<N> d) {
  BigUInt<N+1> xn = BigUInt<N+1>(n) << ((1 << N) - 1);
  BigUInt<N+1> xd = d | (BigUInt<N+1>(1) << ((1 << N)-1));
  return xpair<BigUInt<N+1>, BigUInt<N+1>>(BigUInt<N+1>(xn / xd), BigUInt<N+1>(xn % xd));
}

template<int N>
BigUInt<N> xreciprocal2(BigUInt<N> d_) {
  BigUInt<N+1> d = d_ | (BigUInt<N>(1) << ((1 << N)-1));
  BigUInt<N+1> u = (~BigUInt<N+1>(0)) / d;
  return (BigUInt<N>)u;
}

template<int N>
void doTestRec2(BigUInt<N> d) {
  auto tr = d.reciprocal2();
  auto cr = xreciprocal2(d);
  if (tr != (cr-1) && tr != cr && tr != (cr+1) && tr != (cr+2)) {
    cout << "N    = " << N << endl;
    cout << "d    = " << toHexString(d) << endl;
    cout << "c r2 = " << toHexString(cr) << endl;
    cout << "t r2 = " << toHexString(tr) << endl;
    exit(-1);
  }
}

template<int N>
void doTestDivmod2(BigUInt<N> n, BigUInt<N> d) {
  auto c = xdivmod2(n, d);

  auto t = n.divmod2(d, d.reciprocal2());
  if (c.first != t.first || c.second != t.second) {
    cout << "N    = " << N << endl;
    cout << "n    = " << toHexString(n) << " " << n << endl;
    cout << "d    = " << toHexString(d) << " " << d << endl;
    cout << "d.r2 = " << toHexString(d.reciprocal2()) << endl;
    cout << "t.q  = " << toHexString(t.first ) << endl;
    cout << "c.q  = " << toHexString(c.first ) << endl;
    cout << "t.r  = " << toHexString(t.second) << endl;
    cout << "c.r  = " << toHexString(c.second) << endl;
    exit(-1);
  }

  t = n.divmod2(d);
  if (c.first != t.first || c.second != t.second) {
    cout << "N   = " << N << endl;
    cout << "n   = " << toHexString(n) << " " << n << endl;
    cout << "d   = " << toHexString(d) << " " << d << endl;
    cout << "t.q = " << toHexString(t.first ) << endl;
    cout << "c.q = " << toHexString(c.first ) << endl;
    cout << "t.r = " << toHexString(t.second) << endl;
    cout << "c.r = " << toHexString(c.second) << endl;
    exit(-1);
  }

  if (d != 0) {
    t = n.divmod(d);
    if (t.second >= d || t.first * d + t.second != n) {
      cout << "N    = " << N << endl;
      cout << "n    = " << toHexString(n) << " " << n << endl;
      cout << "d    = " << toHexString(d) << " " << d << endl;
      cout << "t.q  = " << toHexString(t.first ) << endl;
      cout << "t.r  = " << toHexString(t.second) << endl;
      exit(-1);
    }
  }
}

template<int N>
void testLoop() {
  for(int i=0;i<(1 << N);i++) {
    BigUInt<N> d = BigUInt<N>(1) << i;
    doTestRec2(d-1);
    doTestRec2(d  );
    doTestRec2(d+1);

    for(int j=0;j<(1 << N);j++) {
      BigUInt<N> n = BigUInt<N>(1) << j;
      doTestDivmod2(n-1, d-1);
      doTestDivmod2(n-1, d);
      doTestDivmod2(n-1, d+1);
      doTestDivmod2(n, d-1);
      doTestDivmod2(n, d);
      doTestDivmod2(n, d+1);
      doTestDivmod2(n+1, d-1);
      doTestDivmod2(n+1, d);
      doTestDivmod2(n+1, d+1);
    }
  }
}

bool checkRNG(shared_ptr<RNG> rng, int n, int nloop, int thres) {
  vector<int> bin(n);

  cout << "[" << n << "]";
  fflush(stdout);

  for(int i=0;i<n;i++) bin.data()[i] = 0;
  for(int i=0;i<nloop;i++) bin.data()[rng->nextLT(n)]++;

  for(int i=0;i<n;i++) {
    if (bin.data()[i] < thres) {
      cout << "checkRNG failed : " << n << endl;
      return false;
    }
  }

  for(int i=0;i<n;i++) bin.data()[i] = 0;
  for(int i=0;i<nloop;i++) bin.data()[uint64_t(CryptUtil::genRand<M>(uint64_t(n), rng))]++;

  for(int i=0;i<n;i++) {
    if (bin.data()[i] < thres) {
      cout << "genRand failed : " << n << endl;
      return false;
    }
  }

  BigUInt<M> n49 = BigUInt<M>(unsigned(n)).pow(49);
  BigUInt<M> n50 = n49 * n, recn49 = n49.reciprocal();

  for(int i=0;i<n;i++) bin.data()[i] = 0;
  for(int i=0;i<nloop;i++) {
    bin.data()[uint64_t(CryptUtil::genRand<M>(n50, rng).divmod(n49, recn49, NULL))]++;
  }

  for(int i=0;i<n;i++) {
    if (bin.data()[i] < thres) {
      cout << "genRand2 failed : " << n << endl;
      return false;
    }
  }

  return true;
}

int main(int argc, char **argv) {
  int n = 1 << (M - 1), ntest = 100;
  if (argc != 1) n = stoi(argv[1]);
  if (argc == 3) ntest = stoi(argv[2]);

  testLoop<6>();
  testLoop<7>();
  testLoop<8>();
  testLoop<9>();

  shared_ptr<RNG> rng = createPreferredRNG();

  if (ntest != 1) {
    if (argc == 1) {
      for(int i=3;i<=23;i+=2) {
	if (!checkRNG(rng, i, 10000, (10000/i) * 80 / 100) &&
	    !checkRNG(rng, i, 10000, (10000/i) * 80 / 100)) {
	  cout << "NG" << endl;
	  exit(-1);
	}
      }
      ntest = 10;
    } else {
      for(int i=3;i<=23;i+=2) checkRNG(rng, i, 1000000, (1000000/i) * 98 / 100);
    }
  }

  for(int i=0;i<ntest;i++) {
    BigUInt<M> m = CryptUtil::genPrime<M>(BigUInt<M>(1) << n, rng);
    BigUInt<M> a = CryptUtil::genRand<M>(BigUInt<M>(1) << n, rng);
    BigUInt<M> ainv = (BigUInt<M>)BigUInt<M+1>(a).pow(m-2, m);
    BigUInt<M>::Montgomery mg(m);
    if (mg.reduce(mg.pow(mg.transform(a), m-2)) != ainv) {
      cout << "Montgomery" << endl;
      cout << "m = " << m << endl;
      cout << "a = " << a << endl;
      cout << "c ainv = " << ainv << endl;
      cout << "t ainv = " << mg.reduce(mg.pow(mg.transform(a), m-2)) << endl;
      cout << "NG" << endl;
      exit(-1);
    }

    if (ntest == 1) {
      cout << "m = " << m << endl;
      cout << "a = " << a << endl;
      cout << "ainv = " << ainv << endl;
      cout << "(a * ainv) mod m = " << (a * ainv) % m << endl;
    }
    if (a.gcd(m) != 1 || (a * ainv) % m != 1) {
      cout << "NG" << endl;
      exit(-1);
    }
  }

  if (ntest != 1) cout << endl << "OK" << endl;

  return 0;
}
