#pragma once


#include <exception>
#include <string>


namespace cgns {


class cgns_exception : public std::exception {
  public:
    cgns_exception() = default;
    
    cgns_exception(std::string desc)
      : desc(std::move(desc))
    {}

    const char* what() const noexcept override {
      return desc.c_str();
    }
  private:
    std::string desc;
};


} // cgns
