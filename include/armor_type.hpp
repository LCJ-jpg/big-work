#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class armor_type : public the_base
{
public :
    armor_type(int base_id,TargetType _armor) : the_base(base_id , _armor)
    {}
    bool track()//我想在这里计数，lost超过10，返回true，然后在targetmanager里把这个指针清空，然后判断指针指向的数目是否为0，为零就把数组里给清除
    {
        static int cnt = 0;
        int i = add();
        if(i >= ar_lostline)
        {
            clear_shot();
            cnt++;
            if(cnt >= delete_line)
            {
                return true;
            }
        }   
        return false;
    }
};