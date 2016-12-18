#pragma once
#include "variant.hh"

namespace drift
{
  class context
  {
  public:
    std::shared_ptr<variant> operator()(const std::wstring&);
  };
}