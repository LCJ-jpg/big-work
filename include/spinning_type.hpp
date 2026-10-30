#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class spinning_type : public the_base
{
public :
    spinning_type(int base_id,TargetType _armor) : the_base(base_id , _armor)
    {}
    bool track()//我想在这里计数，lost超过5，返回true，然后在targetmanager里把这个指针清空，然后判断指针指向的数目是否为0，为零就把数组里给清除
    {
        static int cnt = 0;
        int i = add();
        if(i >= 5)
        {
            clear_shot();
            cnt++;
            if(cnt >= 5)
            {
                return true;
            }
        }   
        return false;
    }
};