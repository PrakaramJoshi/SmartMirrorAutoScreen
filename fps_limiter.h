#ifndef FPS_LIMITER_H
#define FPS_LIMITER_H
#include <chrono>
#include <thread>
#include "logger.h"

class fps_limiter {
	std::chrono::milliseconds m_delay;
	double m_frames_skip;
	double m_frames_skipped;
	double m_camera_fps;
	double m_target_fps;
	bool m_enable;
public:

	fps_limiter() {
		m_enable = false;
		m_delay = std::chrono::milliseconds(static_cast<int>(0));
		m_camera_fps = 0.0;
		m_target_fps = 0.0;
		m_frames_skip = 0;
		m_frames_skipped = 0.0;
	}

	void set_camera_fps(double _fps) {
		m_camera_fps = _fps;
	}

	void set_target_fps(double _fps) {
		m_target_fps = _fps;
	}

	double init() {
		AceLogger::Log("calculating fps limiter...");
		if (m_target_fps < m_camera_fps) {
			m_enable = true;
			auto camera_tpf		= 1000.0/m_camera_fps;
			auto required_tpf 	= 1000.0/m_target_fps;
			auto delay 			= (required_tpf>camera_tpf)?required_tpf:0.0;
			
			m_frames_skip = static_cast<int>(m_camera_fps/m_target_fps);
			if(m_frames_skip==0){
				m_enable = false;
				AceLogger::Log("no frames skip need for fps limiter");
			}
			using namespace std::chrono_literals;
			m_delay = std::chrono::milliseconds(static_cast<int>(delay));
			if(m_delay<std::chrono::milliseconds(1)){
				m_enable=false;
				AceLogger::Log("no delay needed from fps limiter");
			}
			else{
				std::string msg ="Delay of "+std::to_string(m_delay.count())+ " milliseconds need to enforece fps";
				AceLogger::Log(msg);
			}
			return m_target_fps;
		}
		return m_camera_fps;
	}

	void enforce() {
		if (m_enable)
			std::this_thread::sleep_for(m_delay);
	}

	bool use_frame() {
		if (m_enable) {
			if (m_frames_skipped < m_frames_skip) {
				m_frames_skipped++;
				return false;
			}
			m_frames_skipped = 0;
		}
		return true;
	}
};
#endif