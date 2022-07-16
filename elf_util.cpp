#include "elf_util.h"

#include <cmath>

using namespace elf;
using namespace std;

size_t
elf::num_digits(uint64_t n) {
  return 1+::floor(::log10((double)n));
}
