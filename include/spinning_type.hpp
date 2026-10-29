#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class spinning_type : public the_base
{
public :
    spinning_type(int base_id,TargetType _armor) : the_base(base_id , _armor)
    {}
    void track()
    {
        int i = add();
        if(i >= 5)
        {
            clear_shot();
        }   
    }
};