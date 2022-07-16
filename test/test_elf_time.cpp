#include "elf_time.h"
#include "elf_exception.h"

#include <boost/test/unit_test.hpp>

#include <string>

#include <cstdlib>
#include <ctime>

using namespace elf;

BOOST_AUTO_TEST_SUITE(elf_time)

BOOST_AUTO_TEST_CASE(test_validate_date) {
  //zero/negative values
  BOOST_TEST(validate_date(0) == false);
  BOOST_TEST(validate_date(-1) == false);
  BOOST_TEST(validate_date(-20180708) == false);
  //zero month
  BOOST_TEST(validate_date(20180008) == false);
  //zero days
  BOOST_TEST(validate_date(20181100) == false);
  //bad year
  BOOST_TEST(validate_date(10001108) == false);
  BOOST_TEST(validate_date(99991108) == false);
  //bad month
  BOOST_TEST(validate_date(20171426) == false);
  //bad days
  BOOST_TEST(validate_date(20171232) == false);
  //digits > 8
  BOOST_TEST(validate_date(201712311) == false);
  //leap years
  BOOST_TEST(validate_date(20160229) == true);
  BOOST_TEST(validate_date(20170229) == false);
  BOOST_TEST(validate_date(21000229) == false);
  BOOST_TEST(validate_date(20000229) == true);
  //september 31 days
  BOOST_TEST(validate_date(20170931) == false);
  //valid value
  BOOST_TEST(validate_date(20180730) == true);
  BOOST_TEST(validate_date(20180831) == true);
}

BOOST_AUTO_TEST_SUITE_END()
