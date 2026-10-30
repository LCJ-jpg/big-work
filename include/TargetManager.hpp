#pragma once

#include "test_frames.hpp"
#include "the_base.hpp"
#include "armor_type.hpp"
#include "spinning_type.hpp"
#include "algorithm"
#include "memory"
static int all_armor = 0;//全局常数使用static关键字
static int all_spin = 0;
bool cmp (rm::TargetSnapshot a , rm::TargetSnapshot b)
{
    double ad = ((a.x)*(a.x) +(a.y)*(a.y));
    double bd = ((b.x)*(b.x) +(b.y)*(b.y));
    if(a.status != b.status)
    {
        return a.status > b.status;
    }
    if(ad != bd)
    {
        return ad < bd;
    }
    return a.id < b.id;
};

using namespace rm;
class TargetManager 
{
private:
    InputFrame next_frame;
    InputFrame suc_frame = {-1,-1,{}};
    Result _res = {false,suc_frame.frame_id,{},0,0,0,0};
    std::vector<std::shared_ptr<the_base>> all_targets;//智能指针防止new之后内存泄漏，不用unique——ptr因为不方便放入数组
    
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
        std::vector<int> to_delete;        
        for (const auto &the_see : next_frame.observations)
        {
            int x = -1;
            for (auto& t : all_targets)
            {
                x++;
                if(t->get_base_id() == the_see.id)
                {
                    if(t->get_armor() != the_see.type)
                    {
                        // return false;
                        to_delete.push_back(x);                        
                        switch (the_see.type)
                        {
                            case rm::TargetType::ARMOR:
                            all_armor--;
                            break;
                            case rm::TargetType::SPINNING:
                            all_spin--;
                            break;
                            default : break;
                        }
                        break;
                    }
                }          
            }
        }//类型不匹配
        double dt = ( next_frame.timestamp_ns - suc_frame.timestamp_ns ) / 1e9;
        int x = -1;
        for (auto& t : all_targets)
        {          
            x++;
            bool whether_see = false;             
            for(int i = 0;i < tem.size();i++)
            {
                if(tem[i] == t->get_base_id())
                {
                    whether_see = true;
                    break;
                }
            }
            if(!whether_see)//观测判断删除
            {
                if(t->track())
                {
                    to_delete.push_back(x);
                    switch (t->get_armor())
                        {
                            case rm::TargetType::ARMOR:
                            all_armor--;
                            break;
                            case rm::TargetType::SPINNING:
                            all_spin--;
                            break;
                            default : break;
                        }
                }
            }
            
        }
        for(int a : to_delete)
        {
            auto i = all_targets.begin() + a;
            all_targets.erase(i);
        }
        for (const auto &the_see : next_frame.observations)
        {
            bool judge = true;
            for (auto& t : all_targets)
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
                    std::shared_ptr<armor_type> i (new armor_type(the_see.id,the_see.type));//防止内存泄漏
                    i->update(the_see , next_frame.timestamp_ns);
                    all_targets.push_back(i);
                    all_armor++;
                }
                if(the_see.type == rm::TargetType::SPINNING)
                {
                    std::shared_ptr<spinning_type> i (new spinning_type(the_see.id,the_see.type));//防止内存泄漏
                    i->update(the_see , next_frame.timestamp_ns);
                    all_targets.push_back(i);
                    all_spin++;
                }            
            }
        }//预测 
        for (auto& t : all_targets)
        {
            t->compute(dt);
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
        if(find_target())
        {
            update_suc();
            _res.targets.clear();
            int track_cnt = 0;
            int lost_cnt = 0;
            for(auto& t : all_targets)
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
            std::sort(_res.targets.begin() , _res.targets.end(),cmp);
            _res.accepted = true;
            _res.frame_id = suc_frame.frame_id;
            _res.tracking_target_count = track_cnt;
            _res.lost_target_count = lost_cnt;
            _res.armor_target_count = all_armor;
            _res.spinning_target_count = all_spin;
        }
        else
        {
            this->_res.accepted = false;
            this->_res.frame_id = this->next_frame.frame_id;
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
