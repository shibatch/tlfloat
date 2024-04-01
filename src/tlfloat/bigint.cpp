#include "bigint.hpp"

#ifdef SUPPRESS_WARNINGS
#include "suppress.hpp"
#endif

using namespace std;

namespace tlfloat {
  string toHexString(const uint64_t u) {
    char buf[17];
    snprintf(buf, 17, "%016llx", (unsigned long long)u);
    return string(buf);
  }
  string toHexString(uint32_t x) { return toHexString(uint64_t(x)); }
  string toHexString(uint16_t x) { return toHexString(uint64_t(x)); }

  string toHexString(const BigUInt<6> &u) { return toHexString(u.u64); }

  ostream& operator<<(ostream &os, const BigUInt<6>& u) { return os << u.u64; }
}
