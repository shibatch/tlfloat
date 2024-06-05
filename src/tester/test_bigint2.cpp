#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cstdio>

#include "suppress.hpp"

#include "tlfloat/bigint.hpp"
#include "testerutil.hpp"

#define N 9

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
  for(int i=0;i<nloop;i++) bin.data()[uint64_t(CryptUtil::genRand<N>(uint64_t(n), rng))]++;

  for(int i=0;i<n;i++) {
    if (bin.data()[i] < thres) {
      cout << "genRand failed : " << n << endl;
      return false;
    }
  }

  BigUInt<N> n49 = BigUInt<N>(unsigned(n)).pow(49);
  BigUInt<N> n50 = n49 * n, recn49 = n49.reciprocal();

  for(int i=0;i<n;i++) bin.data()[i] = 0;
  for(int i=0;i<nloop;i++) {
    bin.data()[uint64_t(CryptUtil::genRand<N>(n50, rng).divmod(n49, recn49, NULL))]++;
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
  int n = 1 << (N - 1), ntest = 100;
  if (argc != 1) n = stoi(argv[1]);
  if (argc == 3) ntest = stoi(argv[2]);

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
    BigUInt<N> m = CryptUtil::genPrime<N>(BigUInt<N>(1) << n, rng);
    BigUInt<N> a = CryptUtil::genRand<N>(BigUInt<N>(1) << n, rng);
    BigUInt<N> ainv = (BigUInt<N>)BigUInt<N+1>(a).pow(m-2, m);
    BigUInt<N>::Montgomery mg(m);
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
