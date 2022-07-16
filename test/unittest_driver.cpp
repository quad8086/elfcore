#include <boost/test/included/unit_test.hpp>

#include "elf_exception.h"

using namespace std;
using namespace elf;

void
propagate_elf_error(const elf_error& e) {
  BOOST_ERROR(e.what());
}

void
propagate_runtime_error(const runtime_error& e) {
  BOOST_ERROR(e.what());
}

using namespace boost::unit_test;
test_suite*
init_unit_test_suite(int argc, char** argv) {
  unit_test_monitor.register_exception_translator<elf_error>(&propagate_elf_error);
  unit_test_monitor.register_exception_translator<runtime_error>(&propagate_runtime_error);

  test_suite* test = BOOST_TEST_SUITE("unittest_elfcore");
  return test;
}
