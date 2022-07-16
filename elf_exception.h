#pragma once

#include <string>

namespace elf {
  class elf_error {
  public:
    elf_error() = delete;
    elf_error(const std::string& msg);
    elf_error(const elf_error& other);
    virtual const std::string& what() const;
    
  private:
    std::string _msg;
  };
}
