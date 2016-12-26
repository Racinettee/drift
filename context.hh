#pragma once
#include "variant.hh"

namespace drift
{
  class context
  {
  public:
    context();
    std::shared_ptr<variant> operator()(const std::wstring&);
  private:
    struct compile_context* cc;
  };
}