#pragma once

#include "test_frames.hpp"
// #include "spinning_type.hpp"
// #include "armor_type.hpp"

using namespace rm;
class the_base
{
private:
    int base_id;
    TargetType _armor;
    TargetStatus _status = rm::TargetStatus::TRACK;
    Observation _see;
    Observation last_see;
    TargetSnapshot _pre;
    double last_time = -1;

public:
    the_base(int base_id,TargetType _armor)
    {
        this->base_id = base_id;
        this->_armor = _armor;
        _pre.id = base_id;
        _pre.type = _armor;
        _pre.status = _status;
        _pre.missed_count = 0;
        _pre.velocity_x = 0; _pre.velocity_y = 0; _pre.angular_velocity = 0;
        _pre.predicted_x = 0; _pre.predicted_y = 0; _pre.predicted_angle = 0;
    }
    void update(Observation _see , double t_ns)
    {//前面只是更新               
        this->last_see = this->_see;
        this->_see = _see;
        _status = rm::TargetStatus::TRACK; 
        _pre.status = _status;
        _pre.x = _see.x;
        _pre.y = _see.y;
        _pre.angle = _see.angle;  
        last_time = t_ns;     
        //这里在有上次观测后才会开始计算 
        double dt_obs;
        if(last_time > 0)
        {
            dt_obs = t_ns - last_time;
            _pre.velocity_x = (_pre.x - last_see.x)/dt_obs;
            _pre.velocity_y = (_pre.y - last_see.y)/dt_obs;
            _pre.angular_velocity = (_pre.angle - last_see.angle)/dt_obs;
        } 
        c0();
    }
    virtual ~the_base() = default;
    virtual void track() {};
    //这个函数叫预测函数更好
    void compute (double dt)//输入观测时间，
    {
        this->_pre.predicted_x = this->_pre.velocity_x * dt + this->_pre.x;
        this->_pre.predicted_y = this->_pre.velocity_y * dt + this->_pre.y;
        this->_pre.predicted_angle = this->_pre.angle + dt * this->_pre.angular_velocity;
    }
    TargetSnapshot get_shot()
    {
        return _pre;
    }
    TargetStatus get_status()
    {
        return _status;
    }
    TargetType get_armor()
    {
        return _armor;
    }
    int get_base_id()
    {
        return base_id;
    }
    //下方函数都是为了lost和track状态服务
    void clear_shot()
    {
        _pre.x = 0.0; 
        _pre.y = 0.0;
        _pre.velocity_x = 0.0;
        _pre.velocity_y = 0.0;
        _pre.predicted_x = 0.0;
        _pre.predicted_y = 0.0;
        _pre.angle = 0.0;
        _pre.angular_velocity = 0.0;
        _pre.predicted_angle = 0.0;
        this->_status = rm::TargetStatus::LOST;
        _pre.status = this->_status;
    }//数据清零
    int add()
    {
        _pre.missed_count++;
        return _pre.missed_count;
    }//丢失++
    void c0()
    {
        _pre.missed_count = 0;
    }//看见的清零
    void change_status()
    {
        _status = rm::TargetStatus::LOST;
    }

};