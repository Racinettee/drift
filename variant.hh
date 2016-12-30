#pragma once
#include <new>
#include <string>
#include <memory>
namespace drift
{
  struct variant
  {
    struct null_t {};
    union
    {
      double num;
      std::wstring str;
      null_t nul;
    }; 
    enum class element_kind
    {
      dbl,
      str,
      null
    } kind;
    variant(double d): num(d), kind(element_kind::dbl) { }
    variant(std::wstring s): kind(element_kind::str) { new(&str) std::wstring(s); }
    variant(null_t n): nul(n), kind(element_kind::null) { }
    variant(const variant& o): kind(o.kind)
    {
      switch(kind)
      {
        case element_kind::dbl:
          num = o.num;
          break;
        case element_kind::str:
          new(&str) std::wstring(o.str);
          break;
      }
    }
    ~variant()
    {
      using namespace std;
      switch(kind)
      {
        case element_kind::str:
          str.~wstring();
          break;
        default:
          break;
      }
    }
  };
  inline variant::null_t null_variant() { return variant::null_t(); }
  template <typename... Args>
	auto shared_variant(Args&&... args) ->
	decltype(std::make_shared<variant>(std::forward<Args>(args)...))
	{
		return std::make_shared<variant>(std::forward<Args>(args)...);
	}
  typedef std::shared_ptr<variant> variant_ptr;

  inline variant operator+(const variant& l, const variant& r)
  {
    using namespace std;
    switch(l.kind)
    {
      case variant::element_kind::dbl:
        switch(r.kind)
        {
          case variant::element_kind::dbl:
            return variant(l.num+r.num);
          case variant::element_kind::str:
            return variant(l.num+stod(r.str));
        }
        break;
      case variant::element_kind::str:
        switch(r.kind)
        {
          case variant::element_kind::dbl:
            return variant(l.str+to_wstring(r.num));
          case variant::element_kind::str:
            return variant(l.str+r.str);
        }
        break;
    }
  }
  inline variant operator-(const variant& l, const variant& r)
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
        }
        break;
      case variant::element_kind::str:
        throw runtime_error("Unsupported operation: - attempted with lhand string object");
    }
  }
  inline variant operator-(const variant& r)
  {
    using namespace std;
    switch(r.kind)
    {
      case variant::element_kind::dbl:
        return variant(-r.num);
      case variant::element_kind::str:
        throw runtime_error("Unsupported operation: unary - on rhand of string");
    }
  }
  inline variant operator/(const variant& l, const variant& r)
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
        }
        break;
      case variant::element_kind::str:
        throw std::runtime_error("Unsupported operation: / attempted with lhand string object");
    }
  }
  inline variant operator*(const variant& l, const variant& r)
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
        }
        break;
      case variant::element_kind::str:
        throw std::runtime_error("Unsupported operation at this time: * attempted with lhand string object");
    }
  }
}