#include "elf_time.h"
#include "elf_exception.h"
#include "elf_util.h"

#include <fmt/format.h>
#include <regex>

using namespace std;
using namespace elf;

void
Date::from_string(const string& s_date) {
  if(s_date.size() != Date::required_len)
    throw elf_error("date::from_string: invalid format input="+s_date);
  from_int(std::strtol(s_date.c_str(), nullptr, 10));
}

void
Date::from_int(date_t i_date) {
  if(!validate_date(i_date))
    throw elf_error("date::from_string: invalid date input="+std::to_string(i_date));
  _d = i_date;
}

date_t
Date::to_int() const {
  return _d;
}

std::string
Date::to_string() const {
  return fmt::format("{}", _d);
}

date_t
Date::y() const {
  if(_d==INVALID_DATE)
    throw elf_error("date::y: not initialized");
  return _d / 10000;
}

date_t
Date::m() const {
  if(_d==INVALID_DATE)
    throw elf_error("date::m: not initialized");
  auto date = _d / 100;
  return date % 100;
}

date_t
Date::d() const {
  if(_d==INVALID_DATE)
    throw elf_error("date::d: not initialized");
  return _d % 100;
}

Date
find_date_from_file(const string& fname, date_t user_date) {
  if(user_date>0)
    return Date(user_date);

  std::regex fmt(".*(\\d{8}).*");
  std::smatch match;

  if(!std::regex_match(fname, match, fmt))
    throw elf_error("find_date_from_file: unparseable pattern="+fname);
  auto s_ymd = std::string(match[1]);
  if(s_ymd.size() != Date::required_len)
    throw elf_error("find_date_from_file: invalid pattern="+fname);

  return Date(s_ymd);
}

bool
elf::validate_date(date_t date) {
  if(date <= 0)
    return false;

  if(num_digits(date) != 8)
    return false;

  const int days = date % 100;
  date /= 100;
  const int month = date % 100;
  date /= 100;
  const int year = date;

  if((month < 1) || (month > 12))
    return false;
  if((year < 1970) || (year >= 9999))
    return false;

  int enddays;

  switch(month) {
  case 1:
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    enddays = 31;
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    enddays = 30;
    break;
  case 2:
    // check for leap year
    if(!(year % 4) && ((year % 100) || !(year % 400)))
      enddays = 29;
    else
      enddays = 28;
    break;
  }

  if((days < 1) || (days > enddays))
    return false;

  return true;
}
