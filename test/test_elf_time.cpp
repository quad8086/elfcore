#include "elf_time.h"
#include "elf_exception.h"

#include <boost/test/unit_test.hpp>

#include <string>

#include <cstdlib>
#include <ctime>

using namespace elf;

BOOST_AUTO_TEST_SUITE(elf_time)

BOOST_AUTO_TEST_CASE(date) {
  Date d;
  BOOST_TEST(d.to_int() == INVALID_DATE);
  BOOST_CHECK_THROW(d.from_string("123"), elf_error);
  BOOST_CHECK_THROW(d.from_string("01200101"), elf_error);
  BOOST_CHECK_THROW(d.from_string("201701010"), elf_error);
  BOOST_CHECK_THROW(d.y(), elf_error);
  BOOST_CHECK_THROW(d.m(), elf_error);
  BOOST_CHECK_THROW(d.d(), elf_error);

  d.from_string("20210303");
  BOOST_TEST(d.to_int() == 20210303);
  BOOST_TEST(d.to_string() == "20210303");
  BOOST_TEST(d.y() == 2021);
  BOOST_TEST(d.m() == 3);
  BOOST_TEST(d.d() == 3);

  d.from_int(20220304);
  BOOST_TEST(d.to_int() == 20220304);
  BOOST_TEST(d.to_string() == "20220304");
  BOOST_TEST(d.y() == 2022);
  BOOST_TEST(d.m() == 3);
  BOOST_TEST(d.d() == 4);
}

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

BOOST_AUTO_TEST_CASE(test_date_pattern) {
  BOOST_CHECK_THROW(elf::find_date_from_file("test.csv"), elf_error);
  BOOST_CHECK_THROW(elf::find_date_from_file("test.csv", 10), elf_error);
  BOOST_TEST(elf::find_date_from_file("20210303.test.csv") == 20210303);
  BOOST_TEST(elf::find_date_from_file("test.20210303.test.csv") == 20210303);
  BOOST_TEST(elf::find_date_from_file("test.20210303.test.csv", 20220303) == 20220303);
}

BOOST_AUTO_TEST_CASE(elf_timestamp) {
  Timestamp ts1;
  BOOST_TEST(ts1._ts == TimeConstants::midnight);
  ts1.from_string("09:30:00");
  BOOST_TEST(ts1._ts == 9*TimeConstants::ticks_per_hour + 30*TimeConstants::ticks_per_minute);
  BOOST_CHECK(ts1.to_hms() == "09:30:00");

  BOOST_CHECK_THROW(Timestamp ts3("25:30:00"), elf_error);
  BOOST_CHECK_THROW(Timestamp ts5("01"), elf_error);
  BOOST_CHECK_THROW(Timestamp ts2("01"), elf_error);

  Timestamp ts4("16:44:01.389473");
  BOOST_TEST(ts4._ts == 16*TimeConstants::ticks_per_hour + 44*TimeConstants::ticks_per_minute + 1*TimeConstants::ticks_per_second +
       389473*TimeConstants::ticks_per_usec);

  BOOST_TEST(ts4.str() == "16:44:01.389473");

  Timestamp ts6("3D09:44:00");
  BOOST_TEST(ts6 == 3*TimeConstants::ticks_per_day + 9*TimeConstants::ticks_per_hour + 44*TimeConstants::ticks_per_minute);
  Timestamp ts7("4D00:00:00.123433");
  BOOST_TEST(ts7 == 4*TimeConstants::ticks_per_day + 123433*TimeConstants::ticks_per_usec);

  //check for msec
  Timestamp ts8("12:37:51.048983");
  BOOST_TEST(ts8.to_hms() == "12:37:51");
  BOOST_TEST(ts8.to_hms_msec() == "12:37:51.048");
}

BOOST_AUTO_TEST_CASE(elf_timedelta) {
  using namespace TimeConstants;

  Timedelta td1;
  BOOST_TEST(td1 == 0);
  td1.from_string("9msec");
  BOOST_TEST(td1 == (timedelta_t)(9*ticks_per_msec));

  td1.from_string("-10sec");
  BOOST_TEST(td1 == (timedelta_t)(-10*ticks_per_second));

  td1.from_string("5min");
  BOOST_TEST(td1 == (timedelta_t)(5*ticks_per_minute));

  Timestamp ts1("09:30:00");
  Timestamp ts2 = ts1 + Timedelta("5min");
  BOOST_TEST(ts2 == 9*ticks_per_hour + 35*ticks_per_minute);

  Timestamp ts3("10:05:15");
  Timedelta diff = ts3 - ts1;
  BOOST_TEST(diff == (int)(35*ticks_per_minute + 15*ticks_per_second));
  BOOST_TEST(diff.to_hms() == "00:35:15");

  Timedelta diff2 = ts1 - ts3;
  BOOST_TEST(diff2 == -1 * (int)(35*ticks_per_minute + 15*ticks_per_second));
}

BOOST_AUTO_TEST_SUITE_END()
