#pragma once


#include <string>
#include "cpp_cgns/base/exception.hpp"
#include "std_e/utils/enum.hpp"


namespace cgns {


// from http://cgns.github.io/CGNS_docs_current/filemap/general.html
using C1 = char;
using I4 = std::int32_t;
using I8 = std::int64_t;
using R4 = float;
using R8 = double;


template<class T> constexpr auto
is_cgns_data_type_impl() -> bool {
  if (std::is_same_v<T,C1>) return true;
  if (std::is_same_v<T,I4>) return true;
  if (std::is_same_v<T,I8>) return true;
  if (std::is_same_v<T,R4>) return true;
  if (std::is_same_v<T,R8>) return true;
  return false;
}
template<class T> constexpr bool is_data_type = is_cgns_data_type_impl<T>();

template<class T> concept Data_type = is_data_type<T>;


template<Data_type T> auto to_string    () -> std::string { return "Unknown CGNS data_type"; };
template<> inline     auto to_string<C1>() -> std::string { return "C1"; }
template<> inline     auto to_string<I4>() -> std::string { return "I4"; }
template<> inline     auto to_string<I8>() -> std::string { return "I8"; }
template<> inline     auto to_string<R4>() -> std::string { return "R4"; }
template<> inline     auto to_string<R8>() -> std::string { return "R8"; }


inline auto
n_byte(const std::string& dt) -> int {
  if (dt=="C1") return 1;
  if (dt=="I4") return 4;
  if (dt=="I8") return 8;
  if (dt=="R4") return 4;
  if (dt=="R8") return 8;
  else throw cgns_exception("n_byte: unknown data type \"" + dt + "\"");
}


} // cgns
