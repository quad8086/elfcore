#pragma once

#include <stdint.h>
#include <cstddef>

namespace elf {
  size_t num_digits(uint64_t n);
  bool substring_atoi(const char* buf, size_t len, int64_t& n);
  bool substring_atod(const char* buf, size_t len, double& d);
}
