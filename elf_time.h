#pragma once

#include <cstdint>
#include <ctime>
#include <string>

namespace elf {
  using date_t = int32_t;
  constexpr date_t INVALID_DATE = -1;
  using timestamp_t = uint64_t;
  using timedelta_t = int64_t;

  struct Date {
    Date() = default;
    Date(const std::string& s_date) { from_string(s_date); }
    Date(date_t i_date) { from_int(i_date); }

    operator int() const  { return _d; }
    void from_string(const std::string& s_date);
    void from_int(date_t i_date);
    std::string to_string() const;
    date_t to_int() const;
    date_t y() const;
    date_t m() const;
    date_t d() const;
    bool is_valid() const { return _d == INVALID_DATE; }

    static const int required_len = 8;
    date_t _d = INVALID_DATE;
  };

  bool validate_date(date_t date);
  Date find_date_from_file(const std::string& fname, date_t user_date=0);

  namespace TimeConstants {
    constexpr timestamp_t midnight = 0;

    constexpr timestamp_t ticks_per_usec = 1;
    constexpr timestamp_t ticks_per_msec = 1e3;
    constexpr timestamp_t ticks_per_second = 1e6;
    constexpr timestamp_t ticks_per_minute = 60e6;
    constexpr timestamp_t ticks_per_hour = 3600e6;
    constexpr timestamp_t ticks_per_day = 24 * ticks_per_hour;

    constexpr timestamp_t max_hour = 24;
    constexpr timestamp_t max_minute = 60;
    constexpr timestamp_t max_second = 60;
    constexpr timestamp_t max_usec = 1e6;
  }

  struct Timestamp {
    constexpr Timestamp()
      : _ts(0) {}
    constexpr Timestamp(timestamp_t ts)
      : _ts(ts) {}
    Timestamp(const std::string& s_ts);

    std::string str(bool show_usec=true) const;
    std::string to_hms() const;
    std::string to_hms_msec() const;
    void from_string(const std::string& s_ts);
    void from_go_ts(const std::string& s_ts);
    timestamp_t convert(const std::string& s_ts) const;
    constexpr operator timestamp_t() const { return _ts; }
    timestamp_t get() const { return _ts; };
    int to_seconds() const { return _ts/TimeConstants::ticks_per_second; }

    timestamp_t _ts;
  };

  struct Timedelta {
    constexpr Timedelta()
      : _td(0) {}
    constexpr Timedelta(timedelta_t td)
      : _td(td) {}
    constexpr Timedelta(timestamp_t ts)
      : _td(static_cast<timedelta_t>(ts)) {}
    Timedelta(const std::string& s_ts);

    std::string str() const;
    std::string to_hms() const;
    void from_string(const std::string& s_td) { _td = convert(s_td); }
    timedelta_t convert(const std::string& s_td);
    constexpr operator timedelta_t() const { return _td; }
    timedelta_t _td;
  };

  Timedelta operator-(const Timestamp& ts1, const Timestamp& ts2);
  Timestamp operator+(const Timestamp& ts, const Timedelta& td);
}
