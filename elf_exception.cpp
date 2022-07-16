#include "elf_exception.h"

using namespace std;
using namespace elf;

elf_error::elf_error(const string& msg) {
  _msg = msg;
}

elf_error::elf_error(const elf_error& other) {
  _msg = other.what();
}

const string&
elf_error::what() const {
  return _msg;
}
