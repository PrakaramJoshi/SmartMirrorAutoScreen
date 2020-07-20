#include "MirrorState.h"
#include "MotionEvent.h"
#include "logger.h"
constexpr static int SHUTOFF_DELAY = 5*60;
constexpr static int TURNON_DELAY  = 600;
constexpr static int MAX_ON_TIME   = 20*60; 

using namespace MirrorStateInternal;

bool motion_detected(){
    return MotionEvent::is_motion_observed();
}

void turn_on(){
    AceLogger::Log("turning on the mirror display");
    //std::system("tvservice --explicit=\"DMT 82 HDMI\" ");
    std::system("vcgencmd display_power 1");
}

void turn_off(){
AceLogger::Log("turning off the mirror display");
    //std::system("tvservice -o");
    std::system("vcgencmd display_power 0");
}

MirrorStateOn::MirrorStateOn(){    
    init();
}

void MirrorStateOn::init(){
    m_on_time = std::chrono::high_resolution_clock::now();
}

MirrorState* MirrorStateOn::wait_for_state()const{
    wait_for_minimum_shutoff_delay();
    while(!max_on_time_reached()){
        if(!motion_detected()){
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    turn_off();
    m_next->init();
    return m_next;
}

bool MirrorStateOn::max_on_time_reached()const{
    auto delta = std::chrono::high_resolution_clock::now()-m_on_time;
    if(std::chrono::duration_cast<std::chrono::seconds>(delta).count() >MAX_ON_TIME){
        return true;
    }
    return false;
}

void MirrorStateOn::wait_for_minimum_shutoff_delay()const{
    AceLogger::Log("wait for the minimum time the screen should be on before turning off");
    //turn of motion detection
    MotionEvent::turn_off_motion_detection();
    // wait for the minimum time the screen should be on before turning off
    std::this_thread::sleep_for(std::chrono::seconds(SHUTOFF_DELAY));
    // turn the motion detection on
    MotionEvent::turn_on_motion_detection();
    // give the motion detection time to resume and start detecting motion again
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


MirrorStateOff::MirrorStateOff(bool _enable_for_minimum_off_delay){
    m_enable_for_minimum_off_delay=_enable_for_minimum_off_delay;
}

void MirrorStateOff::init(){
    m_enable_for_minimum_off_delay = true;
}

MirrorState* MirrorStateOff::wait_for_state()const{
    if(m_enable_for_minimum_off_delay){
        wait_for_minimum_offdelay();
    }
    while(!motion_detected()){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    turn_on();
    m_next->init();
    return m_next;
}

void MirrorStateOff::wait_for_minimum_offdelay()const{
    AceLogger::Log("wait for the minimum amount of time screen should be off before turning on");
    //turn of motion detection
    MotionEvent::turn_off_motion_detection();
    // wait for the minimum amount of time screen should be off before turning on
    std::this_thread::sleep_for(std::chrono::seconds(TURNON_DELAY));
    // turn the motion detection on
    MotionEvent::turn_on_motion_detection();
    // give the motion detection time to resume and start detecting motion again
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void MirrorStateManager::run(){
    turn_off();
    MirrorStateOff *off_state = new MirrorStateOff(false);
    MirrorStateOn *on_state = new MirrorStateOn();
    off_state->set_next_state(on_state);
    on_state->set_next_state(off_state);
    MirrorState *mirror_state = off_state;
    while(true){
        mirror_state = mirror_state->wait_for_state();
    }
    delete off_state;
    delete on_state;
}
