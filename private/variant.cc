#include <string>
#include "types.hh"
#include "../context.hh"
namespace
{
  static wchar_t* kind_strings[] =
  {
    L"bool",
    L"double",
    L"string",
    L"null"
  };
}
using namespace std;
namespace drift
{
  wstring kind_string(variant_ptr v)
  {
    return kind_strings[(int)v->kind];
  }

  variant operator+(const variant& l, const variant& r)
  {
    using namespace std;
    switch(l.kind)
    {
      case variant::element_kind::bln:
        switch(r.kind)
        {
          case variant::element_kind::bln:
            return variant(bool(l.boolean|r.boolean));
          case variant::element_kind::dbl:
            return variant((bool)((int)l.boolean+r.num));
          default:
            throw runtime_error("Operator + incompatible type used with bool");
        }
        break;
      case variant::element_kind::dbl:
        switch(r.kind)
        {
          case variant::element_kind::bln:
            return variant(l.num+(int)r.boolean);
          case variant::element_kind::dbl:
            return variant(l.num+r.num);
          case variant::element_kind::str:
            return variant(l.num+stod(r.str));
          case variant::element_kind::null:
            throw runtime_error("Attempted usage of null value as rhand oprand");
          default:
            throw runtime_error("Attempted usage of unsupported type as rhand oprand");
        }
        break;
      case variant::element_kind::str:
        switch(r.kind)
        {
          case variant::element_kind::bln:
            return variant(l.str+(r.boolean ? L"true" : L"false"));
          case variant::element_kind::dbl:
            return variant(l.str+to_wstring(r.num));
          case variant::element_kind::str:
            return variant(l.str+r.str);
          case variant::element_kind::null:
            throw runtime_error("Attempted usage of null value as rhand oprand");
          default:
            throw runtime_error("Attempted usage of unsupported type as rhand oprand");
        }
        break;
      case variant::element_kind::null:
        throw runtime_error("Attempted usage of null value as lhand oprand");
      default:
        throw runtime_error("Attempted usage of unsupported type as lhand oprand");
    }
    return error_variant(__LINE__, __FILE__, L"Error handling + for variant");
  }
  variant operator-(const variant& l, const variant& r)
  {
    using namespace std;
    switch(l.kind)
    {
      case variant::element_kind::dbl:
        switch(r.kind)
        {
          case variant::element_kind::dbl:
            return variant(l.num-r.num);
          case variant::element_kind::str:
            return variant(l.num-stod(r.str));
          case variant::element_kind::null:
            throw runtime_error("Attempted usage of null value as rhand oprand");
        }
        break;
      case variant::element_kind::str:
        throw runtime_error("Unsupported operation: - attempted with lhand string object");
      case variant::element_kind::null:
        throw runtime_error("Attempted usage of null value as lhand oprand");
    }
    return error_variant(__LINE__, __FILE__, L"Error handling - for variant");
  }
  variant operator-(const variant& r)
  {
    using namespace std;
    switch(r.kind)
    {
      case variant::element_kind::bln:
        return variant(bool(-r.boolean));
      case variant::element_kind::dbl:
        return variant(-r.num);
      case variant::element_kind::str:
        throw runtime_error("Unsupported operation: unary - on rhand of string");
      case variant::element_kind::null:
        throw runtime_error("Attempted usage of null value as rhand to unary -");
    }
    return error_variant(__LINE__, __FILE__, L"Error handling unary - for variant");
  }
  variant operator/(const variant& l, const variant& r)
  {
    using namespace std;
    switch(l.kind)
    {
      case variant::element_kind::dbl:
        switch(r.kind)
        {
          case variant::element_kind::dbl:
            return variant(l.num/r.num);
          case variant::element_kind::str:
            return variant(l.num/stod(r.str));
          case variant::element_kind::null:
            throw runtime_error("Attempted usage of null value as rhand oprand");
        }
        break;
      case variant::element_kind::str:
        throw runtime_error("Unsupported operation: / attempted with lhand string object");
      case variant::element_kind::null:
        throw runtime_error("Attempted usage of null value as lhand oprand");
    }
    return error_variant(__LINE__, __FILE__, L"Error handling / for variant");
  }
  variant operator*(const variant& l, const variant& r)
  {
    using namespace std;
    switch(l.kind)
    {
      case variant::element_kind::dbl:
        switch(r.kind)
        {
          case variant::element_kind::dbl:
            return variant(l.num*r.num);
          case variant::element_kind::str:
            return variant(l.num*stod(r.str));
          case variant::element_kind::null:
            throw runtime_error("Attempted usage of null value as rhand oprand");
        }
        break;
      case variant::element_kind::str:
        throw runtime_error("Unsupported operation at this time: * attempted with lhand string object");
      case variant::element_kind::null:
        throw runtime_error("Attempted usage of null value as lhand oprand");
    }
    return error_variant(__LINE__, __FILE__, L"Error handling * for variant");
  }
  object object::operator()()
  {
    switch (ptr->kind)
    {
    case variant::element_kind::fptr:
      auto& fn_ptr = ptr->fn_ptr;
      fn_ptr.owner->exec({}, fn_ptr.address);
      auto result = fn_ptr.owner->stack.back();
      fn_ptr.owner->stack.pop_back();
      return result;
    }
  }
  wostream& operator<<(wostream& o, const drift::object& obj)
  {
    switch (obj.ptr->kind)
    {
    case variant::element_kind::str:
      o << obj.get_string();
      break;
    case variant::element_kind::dbl:
      o << obj.get_double();
      break;
    case variant::element_kind::bln:
      o << obj.get_bool();
      break;
    case variant::element_kind::null:
      o << L"<null>";
      break;
    case variant::element_kind::err: {
      auto& error = obj.get_error();
      o << L"(error: " << error.file << error.line << error.msg << L")";
      break;
    }
    }
    return o;
  }
}