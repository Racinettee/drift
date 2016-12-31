#pragma once
#include "private/types.hh"
#include "private/frame.hh"

namespace drift
{
  class context
  {
  public:
    context();
    ~context();
    // Evaluate an expression
    variant_ptr operator()(const std::wstring&);
    // Index the context for globals
    variant_ptr operator[](const std::wstring&);
    // There is no data segment yet, but if there was this function would include it
    size_t code_size() const;
  private:
    value_stack stack;
    frame global_frame;
    struct compile_context* cc;
    void exec(args_t);
  };
}