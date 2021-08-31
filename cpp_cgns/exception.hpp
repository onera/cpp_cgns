#pragma once


#include <exception>
#include <string>


namespace cgns {


class cgns_exception : public std::exception {
  public:
    cgns_exception() = default;

    cgns_exception(std::string msg)
      : msg(std::move(msg))
    {}

    const char* what() const noexcept override {
      return msg.c_str();
    }
  private:
    std::string msg;
};


} // cgns
