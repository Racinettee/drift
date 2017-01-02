#pragma once
#include <new>
#include <string>
#include <memory>
namespace drift
{
  struct variant
  {
    struct null_t {};
    struct error_t
    {
      int line;
      const char* file;
      const wchar_t* msg;
    };
    union
    {
      bool boolean;
      double num;
      std::wstring str;
      null_t nul;
      error_t error;
    }; 
    enum class element_kind
    {
      bln,
      dbl,
      str,
      null,
      err
    } kind;
    variant(bool b): boolean(b), kind(element_kind::bln) { }
    variant(double d): num(d), kind(element_kind::dbl) { }
    variant(std::wstring s): kind(element_kind::str) { new(&str) std::wstring(s); }
    variant(null_t n): nul(n), kind(element_kind::null) { }
    variant(error_t e): error(e), kind(element_kind::err) { }
    variant(const variant& o): kind(o.kind)
    {
      switch(kind)
      {
        case element_kind::bln:
          boolean = o.boolean;
          break;
        case element_kind::dbl:
          num = o.num;
          break;
        case element_kind::str:
          new(&str) std::wstring(o.str);
          break;
        case element_kind::null:
          nul = null_t();
          break;
        case element_kind::err:
          error = o.error;
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
        case element_kind::null:
          nul.~null_t();
          break;
        case element_kind::err:
          error.~error_t();
        default:
          break;
      }
    }
  };
  typedef std::shared_ptr<variant> variant_ptr;
  std::wstring kind_string(variant_ptr);
  inline variant::null_t null_variant() { return variant::null_t(); }
  inline variant error_variant(int line, const char* file, const wchar_t* msg) { return variant::error_t{line, file, msg}; }
  template <typename... Args>
	auto shared_variant(Args&&... args) ->
	decltype(std::make_shared<variant>(std::forward<Args>(args)...))
	{
		return std::make_shared<variant>(std::forward<Args>(args)...);
	}

  variant operator+(const variant&, const variant&);
  variant operator-(const variant&, const variant&);
  variant operator-(const variant&);
  variant operator/(const variant&, const variant&);
  variant operator*(const variant&, const variant&);
  inline variant operator <(const variant& l, const variant& r)
  {
    using namespace std;
    switch(l.kind)
    {
      case variant::element_kind::bln:
        switch(r.kind)
        {
          case variant::element_kind::bln:
            return variant(l.boolean < r.boolean);
          case variant::element_kind::dbl:
            return variant((int)l.boolean < r.num);
          default:
            throw runtime_error("Attempted a < comparison against incompatible types");
        }
        break;
      case variant::element_kind::dbl:
        switch(r.kind)
        {
          case variant::element_kind::bln:
            return variant(l.num < (int)r.boolean);
          case variant::element_kind::dbl:
            return variant(l.num < r.num);
          default:
            throw runtime_error("Attempted a < comparison against incompatible types");
        }
        break;
      default:
        throw runtime_error("Invalid lhand oprand for <");
    }
    return error_variant(__LINE__, __FILE__, L"Error handling < for variant");
  }
}