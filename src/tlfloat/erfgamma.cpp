//  See https://github.com/shibatch/tlfloat for our project.
//        Copyright Naoki Shibata and contributors.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  float tlfloat_erff(const float a) { return (float)erf(Float(a)); }
  double tlfloat_erf(const double a) { return (double)erf(Double(a)); }
  tlfloat_quad_ tlfloat_erfq(const tlfloat_quad_ a) { return (tlfloat_quad_)erf(Quad(a)); }
  tlfloat_octuple_ tlfloat_erfo(const tlfloat_octuple_ a) { return (tlfloat_octuple_)erf(Octuple(a)); }

  float tlfloat_erfcf(const float a) { return (float)erfc(Float(a)); }
  double tlfloat_erfc(const double a) { return (double)erfc(Double(a)); }
  tlfloat_quad_ tlfloat_erfcq(const tlfloat_quad_ a) { return (tlfloat_quad_)erfc(Quad(a)); }
  tlfloat_octuple_ tlfloat_erfco(const tlfloat_octuple_ a) { return (tlfloat_octuple_)erfc(Octuple(a)); }

  float tlfloat_tgammaf(const float a) { return (float)tgamma(Float(a)); }
  double tlfloat_tgamma(const double a) { return (double)tgamma(Double(a)); }
  tlfloat_quad_ tlfloat_tgammaq(const tlfloat_quad_ a) { return (tlfloat_quad_)tgamma(Quad(a)); }
  tlfloat_octuple_ tlfloat_tgammao(const tlfloat_octuple_ a) { return (tlfloat_octuple_)tgamma(Octuple(a)); }

  float tlfloat_lgammaf(const float a) { return (float)lgamma(Float(a)); }
  double tlfloat_lgamma(const double a) { return (double)lgamma(Double(a)); }
  tlfloat_quad_ tlfloat_lgammaq(const tlfloat_quad_ a) { return (tlfloat_quad_)lgamma(Quad(a)); }
  tlfloat_octuple_ tlfloat_lgammao(const tlfloat_octuple_ a) { return (tlfloat_octuple_)lgamma(Octuple(a)); }
}
