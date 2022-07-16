#include "elf_time.h"
#include "elf_exception.h"
#include "elf_util.h"

#include <boost/lexical_cast.hpp>
#include <fmt/format.h>
#include <regex>
#include <sys/time.h>

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
elf::find_date_from_file(const string& fname, date_t user_date) {
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

Timestamp::Timestamp(const string& s_ts) {
  _ts = convert(s_ts);
}

void
Timestamp::from_string(const string& s_ts) {
  _ts = convert(s_ts);
}

void
Timestamp::from_go_ts(const string& s_ts) {
  // 20220203-104528.093817
  bool is_valid = s_ts.size()==22 && s_ts[8]=='-' && s_ts[15]=='.';
  if(!is_valid)
    throw elf_error("timestamp::from_go_ts: unhandled format");
  int64_t h, m, s, u;
  const char* p = s_ts.c_str();
  substring_atoi(p+9, 2, h);
  substring_atoi(p+11, 2, m);
  substring_atoi(p+13, 2, s);
  substring_atoi(p+16, 6, u);
  _ts = h*TimeConstants::ticks_per_hour + m*TimeConstants::ticks_per_minute + s*TimeConstants::ticks_per_second + u*TimeConstants::ticks_per_usec;
}

timestamp_t
Timestamp::convert(const string& s_ts) const {
  std::regex fmt_hmsu("(\\d{2}):(\\d{2}):(\\d{2})(\\.(\\d{3,6}))?");
  std::regex fmt_dhmsu("(\\d)D(\\d{2}):(\\d{2}):(\\d{2})(\\.(\\d{6}))?");
  std::smatch match;

  timestamp_t ts;
  try {
    if(std::regex_match(s_ts, match, fmt_dhmsu)) {
      timestamp_t d = boost::lexical_cast<timestamp_t>(match[1]);
      timestamp_t h = boost::lexical_cast<timestamp_t>(match[2]);
      timestamp_t m = boost::lexical_cast<timestamp_t>(match[3]);
      timestamp_t s = boost::lexical_cast<timestamp_t>(match[4]);
      timestamp_t u = 0;

      if(match[5].length())
        u = boost::lexical_cast<timestamp_t>(string(match[5]).substr(1));

      if(h > TimeConstants::max_hour ||
         m > TimeConstants::max_minute ||
         s > TimeConstants::max_second ||
         u > TimeConstants::max_usec)
        throw elf_error("timestamp_convert: out of bounds");

      ts = d * TimeConstants::ticks_per_day + h * TimeConstants::ticks_per_hour
        + m * TimeConstants::ticks_per_minute + s * TimeConstants::ticks_per_second
        + u * TimeConstants::ticks_per_usec;

    } else if(std::regex_match(s_ts, match, fmt_hmsu)) {
      timestamp_t h = boost::lexical_cast<timestamp_t>(match[1]);
      timestamp_t m = boost::lexical_cast<timestamp_t>(match[2]);
      timestamp_t s = boost::lexical_cast<timestamp_t>(match[3]);
      timestamp_t u = 0;

      if(match[4].length())
        u = boost::lexical_cast<timestamp_t>(string(match[4]).substr(1));

      if(h > TimeConstants::max_hour ||
         m > TimeConstants::max_minute ||
         s > TimeConstants::max_second ||
         u > TimeConstants::max_usec)
        throw elf_error("timestamp_convert: out of bounds");

      ts = h * TimeConstants::ticks_per_hour + m * TimeConstants::ticks_per_minute
        + s * TimeConstants::ticks_per_second + u * TimeConstants::ticks_per_usec;

    } else {
      throw elf_error("timestamp_convert: unhandled format: " + s_ts);
    }

  } catch(const boost::bad_lexical_cast&) {
    throw elf_error("timestamp_convert: unparseable format: " + s_ts);
  }

  return ts;
}

string
Timestamp::to_hms() const {
  timestamp_t ts = _ts;
  ts %= TimeConstants::ticks_per_day;
  char buf[1024];

  int h = ts / TimeConstants::ticks_per_hour;
  ts -= h * TimeConstants::ticks_per_hour;
  int m = ts / TimeConstants::ticks_per_minute;
  ts -= m * TimeConstants::ticks_per_minute;
  int s = ts / TimeConstants::ticks_per_second;

  snprintf(buf, 1024, "%02d:%02d:%02d", h, m, s);
  return buf;
}

string
Timestamp::to_hms_msec() const {
  timestamp_t ts = _ts;
  ts %= TimeConstants::ticks_per_day;
  char buf[1024];

  int h = ts / TimeConstants::ticks_per_hour;
  ts -= h * TimeConstants::ticks_per_hour;
  int m = ts / TimeConstants::ticks_per_minute;
  ts -= m * TimeConstants::ticks_per_minute;
  int s = ts / TimeConstants::ticks_per_second;
  ts -= s * TimeConstants::ticks_per_second;
  int msec = ts / TimeConstants::ticks_per_msec;

  snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", h, m, s, msec);
  return buf;
}

string
Timestamp::str(bool show_usec) const {
  timestamp_t ts = _ts;
  string s_ts;
  int d = ts / TimeConstants::ticks_per_day;
  char buf[1024];

  if(d) {
    snprintf(buf, 1024, "%dD", d);
    s_ts = buf;
  }

  ts -= d * TimeConstants::ticks_per_day;
  int h = ts / TimeConstants::ticks_per_hour;
  ts -= h * TimeConstants::ticks_per_hour;
  int m = ts / TimeConstants::ticks_per_minute;
  ts -= m * TimeConstants::ticks_per_minute;
  int s = ts / TimeConstants::ticks_per_second;
  ts -= s * TimeConstants::ticks_per_second;
  int us = ts / TimeConstants::ticks_per_usec;

  if(show_usec)
    snprintf(buf, 1024, "%02d:%02d:%02d.%06d", h, m, s, us);
  else
    snprintf(buf, 1024, "%02d:%02d:%02d", h, m, s);

  s_ts += buf;
  return s_ts;
}

Timedelta::Timedelta(const string& s_td) { _td = convert(s_td); }

string
Timedelta::str() const {
  timedelta_t td = _td;
  timestamp_t ts = ::llabs(td);
  char buf[1024];

  // handle small quantities
  string s_td = _td < 0 ? "-" : "";
  if(ts < TimeConstants::ticks_per_msec) {
    snprintf(buf, 1024, "%dusec", (int)ts);
    return s_td + buf;

  } else if(ts < TimeConstants::ticks_per_second) {
    snprintf(buf, 1024, "%dmsec", (int)(ts / TimeConstants::ticks_per_msec));
    return s_td + buf;

  } else if(ts < TimeConstants::ticks_per_minute) {
    snprintf(buf, 1024, "%dsec", (int)(ts / TimeConstants::ticks_per_second));
    return s_td + buf;
  }

  // generic case
  int h = ts / TimeConstants::ticks_per_hour;
  ts -= h * TimeConstants::ticks_per_hour;
  int m = ts / TimeConstants::ticks_per_minute;
  ts -= m * TimeConstants::ticks_per_minute;
  int s = ts / TimeConstants::ticks_per_second;
  ts -= s * TimeConstants::ticks_per_second;
  int us = ts / TimeConstants::ticks_per_usec;

  if(us)
    snprintf(buf, 1024, "%02d:%02d:%02d.%06d", h, m, s, us);
  else
    snprintf(buf, 1024, "%02d:%02d:%02d", h, m, s);

  s_td += buf;
  return s_td;
}

string
Timedelta::to_hms() const {
  timestamp_t ts = ::llabs(_td);
  char buf[1024];

  int h = ts / TimeConstants::ticks_per_hour;
  ts -= h * TimeConstants::ticks_per_hour;
  int m = ts / TimeConstants::ticks_per_minute;
  ts -= m * TimeConstants::ticks_per_minute;
  int s = ts / TimeConstants::ticks_per_second;

  snprintf(buf, 1024, "%02d:%02d:%02d", h, m, s);
  return buf;
}

timedelta_t
Timedelta::convert(const string& us_td) {
  std::regex fmt_usec("(\\d+)usec");
  std::regex fmt_msec("(\\d+)msec");
  std::regex fmt_sec("(\\d+)sec");
  std::regex fmt_min("(\\d+)min");
  std::regex fmt_hour("(\\d+)hour");
  std::regex fmt_hmsu("(\\d{2}):(\\d{2}):(\\d{2})(\\.(\\d{6}))?");

  if(!us_td.size())
    throw elf_error("timedelta_convert: invalid input");

  string s_td = us_td;
  int sign = 1;
  timedelta_t td = 0;
  std::smatch match;

  if(s_td[0] == '-') {
    s_td = s_td.substr(1);
    sign = -1;
  }

  try {
    if(std::regex_match(s_td, match, fmt_usec)) {
      td = sign * TimeConstants::ticks_per_usec * boost::lexical_cast<timedelta_t>(match[1]);

    } else if(std::regex_match(s_td, match, fmt_msec)) {
      td = sign * TimeConstants::ticks_per_msec * boost::lexical_cast<timedelta_t>(match[1]);

    } else if(std::regex_match(s_td, match, fmt_sec)) {
      td = sign * TimeConstants::ticks_per_second * boost::lexical_cast<timedelta_t>(match[1]);

    } else if(std::regex_match(s_td, match, fmt_min)) {
      td = sign * TimeConstants::ticks_per_minute * boost::lexical_cast<timedelta_t>(match[1]);

    } else if(std::regex_match(s_td, match, fmt_hour)) {
      td = sign * TimeConstants::ticks_per_hour * boost::lexical_cast<timedelta_t>(match[1]);

    } else if(std::regex_match(s_td, match, fmt_hmsu)) {
      timestamp_t h = boost::lexical_cast<timestamp_t>(match[1]);
      timestamp_t m = boost::lexical_cast<timestamp_t>(match[2]);
      timestamp_t s = boost::lexical_cast<timestamp_t>(match[3]);
      timestamp_t u = 0;

      if(match[4].length())
        u = boost::lexical_cast<timestamp_t>(string(match[4]).substr(1));

      if(h > TimeConstants::max_hour ||
         m > TimeConstants::max_minute ||
         s > TimeConstants::max_second ||
         u > TimeConstants::max_usec)
        throw elf_error("timedelta_convert: out of bounds");

      td = sign
        * (h * TimeConstants::ticks_per_hour + m * TimeConstants::ticks_per_minute
           + s * TimeConstants::ticks_per_second + u * TimeConstants::ticks_per_usec);

    } else {
      throw elf_error("timedelta_convert: unhandled format" + us_td);
    }

  } catch(const boost::bad_lexical_cast&) {
    throw elf_error("timedelta_convert: unparseable format" + us_td);
  }

  return td;
}

Timedelta
elf::operator-(const Timestamp& ts1, const Timestamp& ts2) {
  Timedelta delta;
  delta._td = (long)ts1._ts - (long)ts2._ts;
  return delta;
}

Timestamp
elf::operator+(const Timestamp& ts, const Timedelta& td) {
  return Timestamp(ts._ts + td._td);
}

timestamp_t
get_rt_timestamp(timestamp_t midnight_offset_secs) {
  timestamp_t ts;

  struct timeval tv;
  ::gettimeofday(&tv, nullptr);

  int seconds = tv.tv_sec - midnight_offset_secs;
  ts = (timestamp_t)seconds * TimeConstants::ticks_per_second;
  ts += (timestamp_t)tv.tv_usec * TimeConstants::ticks_per_usec;
  return ts;
}
