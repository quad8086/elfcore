#include "elf_util.h"

#include <boost/assert.hpp>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <locale>

using namespace elf;
using namespace std;

size_t
elf::num_digits(uint64_t n) {
  return 1+::floor(::log10((double)n));
}

bool
elf::substring_atoi(const char* p, std::size_t len, std::int64_t& out) {
  BOOST_ASSERT(p != nullptr);

  const char* end = p + len;
  for(; std::isspace(*p) && p != end && *p != '\0'; ++p)
    ;

  if(p == end || *p == '\0')
    return false;

  int sign = 1;
  if(*p == '+') {
    ++p;
  } else if(*p == '-') {
    sign = -1;
    ++p;
  }

  if(p == end || *p == '\0')
    return false;

  std::uint64_t result = 0;
  for(; p != end && *p != '\0'; ++p) {
    const int digit = static_cast<int>(*p) - '0';
    if(digit < 0 || digit > 9)
      return false;

    result = result * 10 + digit;
  }

  if(result == 0) {
    out = 0;
    return true;
  }

  if(sign == 1 && result > std::numeric_limits<std::int64_t>::max())
    return false;

  if(sign == -1 && (result - 1) > std::numeric_limits<std::int64_t>::max())
    return false;

  out = result * sign;
  return true;
}

bool
elf::substring_atod(const char* p, size_t len, double& out) {
  BOOST_ASSERT(p != nullptr);
  const char* end = p + len;
  for(; *p == ' ' && p != end && *p != '\0'; ++p)
    ;

  if(p == end || *p == '\0')
    return false;

  int sign = 1;
  if(*p == '+') {
    ++p;
  } else if(*p == '-') {
    sign = -1;
    ++p;
  }

  if(p == end || *p == '\0')
    return false;

  std::uint64_t before = 0;
  for(; p != end && *p != '\0'; ++p) {
    if(*p == '.')
      break;
    const int digit = static_cast<int>(*p) - '0';
    if(digit < 0 || digit > 9)
      return false;

    before = before * 10 + digit;
  }

  if(*p == '.')
    ++p;

  std::uint32_t div = 1;
  std::uint64_t after = 0;
  for(; p != end && *p != '\0'; ++p) {
    const int digit = static_cast<int>(*p) - '0';
    if(digit < 0 || digit > 9)
      return false;

    after = after * 10 + digit;
    div *= 10;
  }

  out = (before + (after / static_cast<double>(div))) * sign;
  return true;
}
