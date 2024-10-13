#include <iostream>
#include <bit>
#include <atomic>

#include <detect.h>

#include "suppress.hpp"

#include "tlfloat/tlmath.hpp"
#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

#define TEST_ARITH
#define TEST_COMPARE
#define TEST_CASTFP16

typedef UnpackedFloat<uint16_t, uint32_t, 8, 7> ubf16;
typedef TLFloat<ubf16> BFloat16;
typedef UnpackedFloat<uint16_t, uint32_t, 5, 10> uhalf;
typedef TLFloat<uhalf> Half;

int main(int argc, char **argv) {
  atomic_int progress;

  #pragma omp parallel for
  for(uint32_t u32=0;u32 < 0x10000;u32++) {
    if ((u32 & 0xff) == 0xff) {
      progress++;
      int k = progress;
      printf(" %d / 256\r", k);
      fflush(stdout);
    }
    uint16_t u = ((u32 << 8) | (u32 >> 8)) & 0xffff;
    __bf16 fu;

#if defined(TEST_CASTFP16) && defined(TLFLOAT_COMPILER_SUPPORTS_FP16)
    {
      __fp16 gu;
      gu = bit_cast<__fp16>(u);

      __bf16 fw = (__bf16)(float)gu;
      __bf16 fx = (__bf16)(BFloat16)Half(gu);

      if (!(isnan((double)fw) && isnan((double)fx)) && fw != fx) {
	printf("cast from fp16 to bf16 : u = %04x\n", (unsigned)u);
	cout << "u: " << (double)gu << " : " << to_string_d(uhalf(gu)) << endl;
	cout << "c: " << (double)fw << " : " << to_string_d(ubf16(fw)) << endl;
	cout << "t: " << (double)fx << " : " << to_string_d(ubf16(fx)) << endl;
	exit(-1);
      }

      fu = bit_cast<__bf16>(u);

      __fp16 gw = (__fp16)(float)fu;
      __fp16 gx = (__fp16)(Half)BFloat16(fu);

      if (!(isnan((double)gw) && isnan((double)gx)) && gw != gx) {
	printf("cast from bf16 to fp16 : u = %04x\n", (unsigned)u);
	cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << (double)gw << " : " << to_string_d(uhalf(gw)) << endl;
	cout << "t: " << (double)gx << " : " << to_string_d(uhalf(gx)) << endl;
	exit(-1);
      }

      float hw = (float)fu;
      float hx = (float)(Float)BFloat16(fu);

      if (!(isnan((double)hw) && isnan((double)hx)) && hw != hx) {
	printf("cast from bf16 to float : u = %04x\n", (unsigned)u);
	cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	cout << "c: " << (double)hw << " : " << to_string_d(ufloat(hw)) << endl;
	cout << "t: " << (double)hx << " : " << to_string_d(ufloat(hx)) << endl;
	exit(-1);
      }

      hw = (float)gu;
      hx = (float)(Float)Half(gu);

      if (!(isnan((double)hw) && isnan((double)hx)) && hw != hx) {
	printf("cast from half to float : u = %04x\n", (unsigned)u);
	cout << "u: " << (double)gu << " : " << to_string_d(uhalf(gu)) << endl;
	cout << "c: " << (double)hw << " : " << to_string_d(ufloat(hw)) << endl;
	cout << "t: " << (double)hx << " : " << to_string_d(ufloat(hx)) << endl;
	exit(-1);
      }
    }
#endif

    memcpy((void *)&fu, (void *)&u, sizeof(fu));

    for(uint32_t v32=0;v32 < 0x10000;v32++) {
      uint16_t v = ((v32 << 8) | (v32 >> 8)) & 0xffff;
      __bf16 fv;
      memcpy((void *)&fv, (void *)&v, sizeof(fv));

#ifdef TEST_ARITH
      {
	__bf16 fw = fu + fv;
	__bf16 fx = __bf16(ubf16::addsub(ubf16(fu), ubf16(fv), false));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan((double)fw) && isnan((double)fx)) && w != x) {
	  printf("u = %04x, v = %04x, w = %04x, x = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  cout << "c: " << (double)fw << " : " << to_string_d(ubf16(fw)) << endl;
	  cout << "t: " << (double)fx << " : " << to_string_d(ubf16(fx)) << endl;
	  exit(-1);
	}
      }

      {
	__bf16 fw = fu - fv;
	__bf16 fx = __bf16(ubf16::addsub(ubf16(fu), ubf16(fv), true));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan((double)fw) && isnan((double)fx)) && w != x) {
	  printf("u = %04x, v = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  cout << "c: " << (double)fw << " : " << to_string_d(ubf16(fw)) << endl;
	  cout << "t: " << (double)fx << " : " << to_string_d(ubf16(fx)) << endl;
	  exit(-1);
	}
      }

      {
	__bf16 fw = fu * fv;
	__bf16 fx = __bf16(ubf16::mul(ubf16(fu), ubf16(fv)));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan((double)fw) && isnan((double)fx)) && w != x) {
	  printf("u = %04x, v = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  cout << "c: " << (double)fw << " : " << to_string_d(ubf16(fw)) << endl;
	  cout << "t: " << (double)fx << " : " << to_string_d(ubf16(fx)) << endl;
	  exit(-1);
	}
      }

      {
	__bf16 fw = fu / fv;
	__bf16 fx = __bf16(ubf16::div(ubf16(fu), ubf16(fv)));
	uint16_t w, x;
	memcpy((void *)&w, (void *)&fw, sizeof(w));
	memcpy((void *)&x, (void *)&fx, sizeof(x));

	if (!(isnan((double)fw) && isnan((double)fx)) && w != x) {
	  printf("u = %04x, v = %04x, y = %04x, z = %04x\n", (unsigned)u, (unsigned)v, (unsigned)w, (unsigned)x);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  cout << "c: " << (double)fw << " : " << to_string_d(ubf16(fw)) << endl;
	  cout << "t: " << (double)fx << " : " << to_string_d(ubf16(fx)) << endl;
	  exit(-1);
	}
      }
#endif

#ifdef TEST_COMPARE
      {
	bool bc = (fu == fv);
	bool bt = ubf16(fu) == ubf16(fv);

	if (bc != bt) {
	  printf("== : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = (fu <= fv);
	bool bt = ubf16(fu) <= ubf16(fv);

	if (bc != bt) {
	  printf("<= : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = (fu >= fv);
	bool bt = ubf16(fu) >= ubf16(fv);

	if (bc != bt) {
	  printf(">= : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = (fu < fv);
	bool bt = ubf16(fu) < ubf16(fv);

	if (bc != bt) {
	  printf("< : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  exit(-1);
	}
      }

      {
	bool bc = (fu > fv);
	bool bt = ubf16(fu) > ubf16(fv);

	if (bc != bt) {
	  printf("> : u = %04x, v = %04x, bc = %d, bt = %d\n", (unsigned)u, (unsigned)v, (int)bc, (int)bt);
	  cout << "u: " << (double)fu << " : " << to_string_d(ubf16(fu)) << endl;
	  cout << "v: " << (double)fv << " : " << to_string_d(ubf16(fv)) << endl;
	  exit(-1);
	}
      }
#endif
    }
  }

  cout << endl << "OK" << endl;
  return 0;
}
