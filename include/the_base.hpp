#pragma once

#include "test_frames.hpp"

using namespace rm;
class the_base
{
private:
    TargetType _armor;
    TargetStatus _status;
    Observation _see;
    Observation last_see;
    TargetSnapshot _pre;
    int64_t t_ns , t_prev_ns , dt , dt_obs;
    int64_t last_time;

public:
    void update(TargetType _armor ,Observation _see)
    {
        this->last_see = this->_see;
        this->_armor = _armor;
        this->_see = _see;
        
    }
    virtual ~the_base() = default;
    void compute (int64_t t_ns,int64_t time = 0)//输入观测时间，
    {
        if(time != 0)
        {
            last_time = time;
        }   
        this->t_prev_ns = this->t_ns ;
        this->t_ns = t_ns;
        dt = (t_ns - t_prev_ns) / 1e9;
        dt_obs = (t_ns - last_time) / 1e9;
        _pre.velocity_x = (_see.x - last_see.x)/dt_obs;
        _pre.velocity_y = (_see.y - last_see.y)/dt_obs;
        _pre.angular_velocity = (_see.angle - last_see.angle)/dt_obs;
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
    }
    int add()
    {
        _pre.missed_count++;
        return _pre.missed_count;
    }
    void c0()
    {
        _pre.missed_count = 0;
    }

};