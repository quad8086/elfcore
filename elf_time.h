#pragma once

#include <cstdint>
#include <ctime>
#include <string>

namespace elf {
  using date_t = int32_t;
  constexpr date_t INVALID_DATE = -1;

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
}
