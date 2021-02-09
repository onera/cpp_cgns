// trial 0.
//
//// TODO dispatch_I4_I8 by type
//struct {
//  static constexpr auto runtime  = std_e::make_array("I4","I8");
//  static constexpr auto comptime = std  ::  tuple   ( I4 , I8 );
//} integer_type_mapping;
//
//template<class Type_mapping> constexpr auto
//is_mapping_type_array(Type_mapping x) -> bool {
//  return tuple_size<x.runtime> == tuple_size<x.comptime>;
//  // TODO + x.runtime is an array of Regular type
//  // TODO + x.comptime is a tuple
//}
//
//template<class Type_mapping> auto
//size(Type_mapping) -> int {
//  static_assert(is_mapping_type_array(Type_mapping));
//  return tuple_size<x.runtime>;
//}
//
//template<class T, class Type_mapping, class F> auto
//apply_matching_type(const T& type_id, Type_mapping, F f) {
//  
//}
//
//template<class I> auto
//element_type__impl(I, const tree& elements_node) -> ElementType_t {
//  auto val = view_as_span<I>(elements_node.value);
//  return (ElementType_t)val[0]; // from SIDS File Mapping Manual, Elements_t
//}
//
//auto
//element_type(const tree& elements_node) -> ElementType_t {
//  STD_E_ASSERT(elements_node.label=="Elements_t");
//  return apply_matching(
//    elements_node.value.data_type,
//    integer_type_mapping, 
//    [&elements_node](auto type_tag){ 
//      return element_type__impl(type_tag,
//    }
//  );
//}

// trial 1.
// TODO dispatch_I4_I8 by type
struct {
  static constexpr auto runtime  = std_e::make_array("I4","I8");
  static constexpr auto comptime = std  ::  tuple   ( I4 , I8 );
} integer_type_mapping;

template<class Type_mapping> constexpr auto
is_mapping_type_array(Type_mapping x) -> bool {
  return tuple_size<x.runtime> == tuple_size<x.comptime>;
  // TODO + x.runtime is an array of Regular type
  // TODO + x.comptime is a tuple
}

template<class Type_mapping> auto
size(Type_mapping) -> int {
  static_assert(is_mapping_type_array(Type_mapping));
  return tuple_size<x.runtime>;
}

template<class T, class Type_mapping, template<class tag_type, auto...> class F> auto
apply_matching_type(const T& type_id, Type_mapping, F f) {
  
}

template<class I> auto
element_type__impl(I, const tree& elements_node) -> ElementType_t {
  auto val = view_as_span<I>(elements_node.value);
  return (ElementType_t)val[0]; // from SIDS File Mapping Manual, Elements_t
}

auto
element_type(const tree& elements_node) -> ElementType_t {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return apply_matching(
    elements_node.value.data_type,
    integer_type_mapping, 
    [&elements_node](auto type_tag){ 
      return element_type__impl(type_tag,elements_node);
    }
  );
}


// trial 2.
template<class id_type, class F, class... Args> auto
dispatch_I4_I8(const id_type& id, F f, Args&&... args) {
  if (id=="I4") {
    return f(I4{},FWD(args)...);
  }
  if (id=="I8") {
    return f(I8{},FWD(args)...);
  }
}

template<class I> auto
element_type__impl(const tree& elements_node) -> ElementType_t {
  auto val = view_as_span<I>(elements_node.value);
  return (ElementType_t)val[0]; // from SIDS File Mapping Manual, Elements_t
}

auto
element_type(const tree& elements_node) -> ElementType_t {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  auto id_type = elements_node.value.data_type;
  return dispatch_I4_I8(
    id_type,
    [&elements_node]<class I>(I type_tag){ 
      return element_type__impl<I>(elements_node);
    }
  );
}

// trial 3.
template<class F, class Tree> auto
// requires Tree is tree or const tree
dispatch_I4_I8(F f, Tree& t) {
  auto type = t.value.data_type;
  if (type=="I4") {
    return f(I4{},FWD(args)...);
  }
  if (type=="I8") {
    return f(I8{},FWD(args)...);
  }
  throw cgns_exception("dispatch_I4_I8 expects an integer data_type");
}

template<class I> auto
element_type__impl(I, const tree& elements_node) -> ElementType_t {
  auto val = view_as_span<I>(elements_node.value);
  return (ElementType_t)val[0]; // from SIDS File Mapping Manual, Elements_t
}

auto
element_type(const tree& elements_node) -> ElementType_t {
  STD_E_ASSERT(elements_node.label=="Elements_t");
  return dispatch_I4_I8(
    LIFT(element_type__impl),
    elements_node
  );
}





