#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"
#include "armor_type.hpp"
#include "spinning_type.hpp"
#include "algorithm"

using namespace rm;
class TargetManager 
{
private:
    InputFrame next_frame;
    InputFrame suc_frame = {0,0,{}};
    Result _res = {false,suc_frame.frame_id,{},0,0,0,0};
    double t_ns = 0, t_prev_ns, dt;   //当前，上一个成功，时间差
    std::vector<the_base*> all_targets;
    std::vector<armor_type*> all_armor;
    std::vector<spinning_type*> all_spin;
public:
    void get_frame(const InputFrame &a_frame)//录入帧
    {
        this->next_frame = a_frame;
    }
    bool find_target()//找目标，顺便判断目标是否重复,与类型变化
    {
        bool out = false;
        bool judge = false;
        if(base_judge())
        {
            std::vector<int> tem;
            for (const auto &the_see : next_frame.observations)
            {

                if(the_see.id < 0)
                {
                    return false;
                }
                tem.push_back(the_see.id);
            }
            std::sort(tem.begin(),tem.end());
            for (int i = 1;i < tem.size();i++)
            {
                if (tem[i - 1] == tem[i])
                {
                    return false;
                }
            }
            for (const auto &the_see : next_frame.observations)
            {
                for (the_base* t : all_targets)
                {
                    t->track();
                    if(t->get_base_id() == the_see.id)
                    {
                        if(t->get_armor() != the_see.type)
                        {
                            return false;
                        }
                        else
                        {
                            t->update(the_see);                        
                            out = true;
                        }
                    }
                    else if(the_see.id >= 0)
                    {
                        judge = true;
                    }                    
                }
            }
            for (const auto &the_see : next_frame.observations)
            {
                if(judge == true)
                {
                    if(the_see.type == 0)
                    {
                        armor_type* i = new armor_type(the_see.id,the_see.type);
                        i->update(the_see);
                        all_targets.push_back(i);
                        all_armor.push_back(i);
                        out = true;
                    }
                    if(the_see.type == 1)
                    {
                        spinning_type* i = new spinning_type(the_see.id,the_see.type);
                        i->update(the_see);
                        all_targets.push_back(i);
                        all_spin.push_back(i);
                        out = true;
                    }
                    
                }
            }
        }
        return out;
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
    void update_suc()//改变suc_frame
    {
        this->suc_frame = this->next_frame;
    }
    void change_res()//对将要输出的_res做更改
    {        
        if(base_judge() && find_target())
        {
            update_suc();
            _res.targets.clear();
            int track_cnt = 0;
            int lost_cnt = 0;
            int armor_cnt = all_armor.size();
            int spin_cnt = all_spin.size();
            for(const the_base* t : all_targets)
            {
                if(t.status == 0)
                {
                    lost_cnt++;
                }
                else
                {
                    track_cnt++;
                }
                _res.targets.push_back(t._pre)
            }
            _pre.accepted = true;
            _pre.frame_id = suc_frame.frame_id;
            _pre.tracking_target_count = track_cnt;
            _pre.lost_target_count = lost_cnt;
            _pre.armor_target_count = armor_cnt;
            _pre.spinning_target_count = spin_cnt ;

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
