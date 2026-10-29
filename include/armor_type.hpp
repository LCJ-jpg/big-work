#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class armor_type : public the_base
{
public :
    armor_type(int base_id,TargetType _armor) : the_base(base_id , _armor)
    {}
    void track()
    {
        int i = add();
        if(i >= 3)
        {
            clear_shot();
        }   
    }
};