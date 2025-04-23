//  See https://github.com/shibatch/tlfloat for our project.
//        Copyright Naoki Shibata and contributors.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <atomic>

#include <detect.h>
#include "suppress.hpp"

#include "tlfloat/tlmath.hpp"
#include "testerutil.hpp"

using namespace std;
using namespace tlfloat;
using namespace tlfloat::detail;

int main(int argc, char **argv) {
  typedef UnpackedFloat<uint16_t, uint32_t, 5, 10> uhalf;

  atomic_int progress;

  #pragma omp parallel for
  for(uint32_t u32=0;u32 < 0x10000;u32++) {
    if ((u32 & 0xf) == 0xf) {
      progress++;
      int k = progress;
      printf(" %d / 4096\r", k);
      fflush(stdout);
    }
    uint16_t u = ((u32 << 8) | (u32 >> 8)) & 0xffff;
    __fp16 fu;
    memcpy((void *)&fu, (void *)&u, sizeof(fu));

    for(uint32_t v32=0;v32 < 0x10000;v32++) {
      uint16_t v = ((v32 << 8) | (v32 >> 8)) & 0xffff;
      __fp16 fv;
      memcpy((void *)&fv, (void *)&v, sizeof(fv));

      for(uint32_t w32=0;w32 < 0x10000;w32++) {
	uint16_t w = ((w32 << 8) | (w32 >> 8)) & 0xffff;
	__fp16 fw;
	memcpy((void *)&fw, (void *)&w, sizeof(fw));

	__fp16 fx = __fp16(uhalf::fma(uhalf(fw), uhalf(fu), uhalf(fv)));
	__fp16 fy = fma(double(fw), double(fu), double(fv));

	uint16_t x, y;
	memcpy((void *)&x, (void *)&fx, sizeof(x));
	memcpy((void *)&y, (void *)&fy, sizeof(y));

	if (!(isnan(fx) && isnan(fy)) && x != y) {
	  cout << "u: " << fu << " : " << to_string_d(uhalf(fu)) << endl;
	  cout << "v: " << fv << " : " << to_string_d(uhalf(fv)) << endl;
	  cout << "w: " << fw << " : " << to_string_d(uhalf(fw)) << endl;
	  cout << "c: " << fy << " : " << to_string_d(uhalf(fy)) << endl;
	  cout << "t: " << fx << " : " << to_string_d(uhalf(fx)) << endl;
	  exit(-1);
	}
      }
    }
  }

  cout << endl << "OK" << endl;
  return 0;
}
