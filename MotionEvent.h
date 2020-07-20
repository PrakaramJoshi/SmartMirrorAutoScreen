#pragma once
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include "logger.h"
class MotionEvent{
	std::chrono::high_resolution_clock::time_point 	m_event_time;
	mutable std::mutex   							m_mutex;
	std::chrono::milliseconds 						m_motion_ttl;
	std::atomic<bool>                               m_detect_motion;

	MotionEvent(const MotionEvent&)                   =delete;
	MotionEvent(MotionEvent&&)                        =delete;
	MotionEvent& operator=(const MotionEvent&)        =delete;
	MotionEvent& operator=(MotionEvent&&)             =delete;

	MotionEvent(){
		AceLogger::Log_now("MotionEvent Created");
		m_motion_ttl = std::chrono::milliseconds(100);
		m_detect_motion = true;
		set_motion_internal();
	}

	static MotionEvent& getinstance(){
		static MotionEvent instance;
		return instance;
	}

	void set_motion_internal(){
		std::unique_lock<std::mutex> lock(m_mutex);
		m_event_time = std::chrono::high_resolution_clock::now();
	}

	void set_motion_detection_internal(bool _val){
		m_detect_motion = _val;
	}

	bool is_motion_detection_enabled_internal()const{
		return m_detect_motion.load();
	}

	bool motion_observed_internal()const{	
		std::unique_lock<std::mutex> lock(m_mutex);
		auto n = std::chrono::high_resolution_clock::now();
		auto d = std::chrono::duration_cast
					<std::chrono::milliseconds>(n- m_event_time);
		if(d<=m_motion_ttl){	
			return true;
		}
		return false;
	}

public:
	static void set_motion(){
		AceLogger::Log("motion logged");
		getinstance().set_motion_internal();
	}

	static bool is_motion_observed(){
		if(getinstance().motion_observed_internal()){
			AceLogger::Log("motion observed");
			return true;
		}
		return false;
	}

	static void turn_on_motion_detection(){
		AceLogger::Log("Turning motion detection on");
		getinstance().set_motion_detection_internal(true);
	}

	static void turn_off_motion_detection(){
		AceLogger::Log("Turning motion detection off");
		getinstance().set_motion_detection_internal(false);
	}

	static bool is_motion_detection_enabled(){
		return getinstance().is_motion_detection_enabled_internal();
	}
};

