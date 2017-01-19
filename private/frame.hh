#pragma once
// A frame represents a stack frame. The topmost frame is owned by the context, and is the global frame
#include "types.hh"
namespace drift
{
  class frame
  {
    var_index return_idx;
    frame* parent;
    variable_storage var_storage;
  public:
    frame(var_index rtrn_idx = 0U, frame* prnt = nullptr) : return_idx(rtrn_idx), parent(prnt) { }
    variant_ptr get_var(var_index index)
    {
      try {
        return var_storage.at(index);
      }
      catch(...)
      {
        if(parent)
          return parent->get_var(index);
      }
      return shared_variant(null_variant());
    }
    var_index return_index() const
    {
      return return_idx;
    }
    void set_var(var_index index, variant_ptr val)
    {
      var_storage[index] = val;
    }
  };
}