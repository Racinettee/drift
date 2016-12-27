#pragma once
#include "private/types.hh"

namespace drift
{
  class context
  {
  public:
    context();
    ~context();
    std::shared_ptr<variant> operator()(const std::wstring&);
    // There is no data segment yet, but if there was this function would include it
    size_t code_size() const;
  private:
    value_stack stack;
    struct compile_context* cc;
    void exec(args_t);
  };
}