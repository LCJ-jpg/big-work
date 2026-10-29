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
    InputFrame suc_frame = {-1,-1,{}};
    Result _res = {false,suc_frame.frame_id,{},0,0,0,0};
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
        if(!base_judge())
        {return false;}
        std::vector<int> tem;
        for (const auto &the_see : next_frame.observations)
        {
            if(the_see.id < 0)
            {return false ;}
            tem.push_back(the_see.id);
        }//判断编号大于0
        std::sort(tem.begin(),tem.end());
        for (int i = 1;i < tem.size();i++)
        {
            if (tem[i - 1] == tem[i])
            {
                return false;
            }
        }//重复编号
        for (const auto &the_see : next_frame.observations)
        {
            for (the_base* t : all_targets)
            {
                if(t->get_base_id() == the_see.id)
                {
                    if(t->get_armor() != the_see.type)
                    {
                        return false;
                    }
                }                 
            }
        }//类型不匹配
        for (const auto &the_see : next_frame.observations)
        {
            bool judge = true;
            for (the_base* t : all_targets)
            {
                if(t->get_base_id() == the_see.id)
                {
                    t->update(the_see,next_frame.timestamp_ns);
                    judge = false;
                    break;
                }                 
            }//真没有
            if(judge)
            {
                if(the_see.type == rm::TargetType::ARMOR)
                {
                    armor_type* i = new armor_type(the_see.id,the_see.type);
                    i->update(the_see , next_frame.timestamp_ns);
                    all_targets.push_back(i);
                    all_armor.push_back(i);
                }
                if(the_see.type == rm::TargetType::SPINNING)
                {
                    spinning_type* i = new spinning_type(the_see.id,the_see.type);
                    i->update(the_see , next_frame.timestamp_ns);
                    all_targets.push_back(i);
                    all_spin.push_back(i);
                }            
            }
        }//看到的更新，没看到在下方
        double dt = ( next_frame.timestamp_ns - suc_frame.timestamp_ns ) / 1e9;
        for (the_base* t : all_targets)
        {
            t->compute(dt);
            bool whether_see = false; 
            for(int i = 0;i < tem.size();i++)
            {
                if(tem[i] == t->get_base_id())
                {
                    whether_see = true;
                    break;
                }
            }
            if(!whether_see)
            {
                t->track();
            }
        }

        return true;
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
            for(the_base* t : all_targets)
            {
                if(t->get_status() == rm::TargetStatus::LOST)
                {
                    lost_cnt++;
                }
                else
                {
                    track_cnt++;
                }
                _res.targets.push_back(t->get_shot());
            }
            _res.accepted = true;
            _res.frame_id = suc_frame.frame_id;
            _res.tracking_target_count = track_cnt;
            _res.lost_target_count = lost_cnt;
            _res.armor_target_count = armor_cnt;
            _res.spinning_target_count = spin_cnt ;
        }
        else
        {
            this->_res.accepted = false;
        }
    }
    Result onFrame(const InputFrame &frame)
    {
        get_frame(frame);
        change_res();
    // 占位行为：不做处理，返回"本帧未接受"
    return _res;
    }
};
