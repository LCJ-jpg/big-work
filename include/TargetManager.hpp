#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"

using namespace rm;
class TargetManager 
{
private:
    InputFrame next_frame;
    InputFrame suc_frame = {0,0,{}};
    Result _res;
    double t_ns = 0, t_prev_ns, dt;   //当前，上一个成功，时间差
    std::vector<int> all_id;
    std::vector<the_base> targets;
public:
    void get_frame(const InputFrame &a_frame)//录入帧
    {
        this->next_frame = a_frame;
    }
    bool find_target()//找目标，顺便判断目标是否重复
    {
        if(base_judge())
        {
           for (const auto &the_see : next_frame.observations)
           {
            for (int &the_id : all_id)
            {
                if(the_see.id == the_id)
                {
                    if(1)
                    {
                        return false;
                    }
                }
                else if(the_see.id >= 0)
                {
                    the_base i(the_see.type,the_see);
                    targets.push_back(i);
                    return true;
                }
                
            }
           }
        }
        return false;
    }
    bool base_judge()//判断时间与帧id
    {
        if(next_frame.frame_id > suc_frame.frame_id)
        {
            if(next_frame.timestamp_ns > suc_frame.timestamp_ns)
            {
                return true;
            }
            
        }
        return false;
    }
    // void frameTimeNs()
    // {
    //     this->t_ns = this->_frame.timestamp_ns;
    //     if(this->_res.accepted == true)
    //     {
    //         this->t_prev_ns = this->t_ns;
    //     }
    //     dt = (t_ns - t_prev_ns) / 1e9;
    // }

    void change_res()//对将要输出的_res做更改
    {
        if(base_judge() && find_target())
        {
            
            this->_res = {true,suc_frame.frame_id,{},0,0,0,0};
        }
        else
        {
            this->_res.accepted = false;
        }
    }
    Result onFrame(const InputFrame &frame)
    {
    // 占位行为：不做处理，返回"本帧未接受"
    return _res;
    }
};
