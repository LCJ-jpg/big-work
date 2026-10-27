#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class armor_type : public the_base
{
public :
    void track()
    {
        if(get_status() == rm::TargetStatus::LOST)
        {
            if(add() >= 3)
            {
                clear_shot();
            }
        }
        else
        {
            c0();
        }
    }
};