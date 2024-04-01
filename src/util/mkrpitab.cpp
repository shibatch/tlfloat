#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <mpfr.h>

#define N (1 << 19)

int main(int argc, char **argv) {
  static uint8_t table[N / 8];

  mpfr_set_default_prec(N);

  mpfr_t pi, m, n, o;
  mpfr_inits(pi, m, n, o, NULL);
  mpfr_const_pi(pi, GMP_RNDN);

  mpfr_d_div(n, 1.0, pi, GMP_RNDN);

  for(int e=0;e < N / 8;e++) {
    mpfr_set(m, n, GMP_RNDN);

    mpfr_set_ui_2exp(o, 1, e * 8, GMP_RNDN);
    mpfr_mul(m, m, o, GMP_RNDN);

    mpfr_frac(m, m, GMP_RNDN);

    mpfr_set_ui_2exp(o, 1, 8, GMP_RNDN);
    mpfr_mul(m, m, o, GMP_RNDN);

    mpfr_trunc(m, m);

    table[e] = (uint8_t)mpfr_get_d(m, GMP_RNDN);
  }

  mpfr_clears(pi, m, n, o, NULL);

  printf("#include <cstdint>\n");
  printf("\n");
  printf("namespace tlfloat {\n");
  printf("  namespace detail {\n");
  printf("#ifndef __CUDA_ARCH__\n");
  printf("    static constexpr uint8_t tlfloat_rpitab[] = {\n");
  printf("#else\n");
  printf("    static __device__ constexpr uint8_t tlfloat_rpitab[] = {\n");
  printf("#endif");
  for(int i=0;i<N / 8;i++) {
    if ((i & 15) == 0) printf("\n      ");
    printf("0x%02x, ", (unsigned)table[i]);
  }
  printf("\n    };\n");
  printf("  };\n");
  printf("}\n");
}
