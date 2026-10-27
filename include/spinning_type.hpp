#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class spinning_type : public the_base
{
public :
    void track()
    {
        if(get_status() == rm::TargetStatus::LOST)
        {
            if(add() >= 5)
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