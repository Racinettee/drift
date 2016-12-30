#pragma once
// A frame represents a stack frame. The topmost frame is owned by the context, and is the global frame
#include "types.hh"
namespace drift
{
    class frame
    {
        frame* parent = nullptr;
        variable_storage var_storage;
    public:
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
        }
        void set_var(var_index index, variant_ptr val)
        {
            var_storage[index] = val;
        }
    };
}