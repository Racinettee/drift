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
    object operator()(const std::wstring&);
    // Index the context for globals
    object operator[](const std::wstring&);
    // There is no data segment yet, but if there was this function would include it
    size_t code_size() const;
    // Load script from file
    object load_file(const wchar_t*);
    object load_file(const std::wstring&);
  private:
    value_stack stack;
    frame global_frame;
    struct compile_context* cc;
    void exec(args_t, size_t=0U);
    friend class object;
  };
}