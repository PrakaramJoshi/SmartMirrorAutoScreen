#pragma once
#include <string>
class MoDetSettings {

	std::string m_name;
	int m_min_motion;
	int m_shut_off_delay;
	int m_turn_on_delay;
	int m_max_on_time;
	double m_fps;
	double m_min_motion_percent;

	MoDetSettings();
	MoDetSettings(const MoDetSettings&) = delete;
	MoDetSettings(MoDetSettings&&) = delete;
	MoDetSettings& operator=(const MoDetSettings&) = delete;
	MoDetSettings& operator=(MoDetSettings&&) = delete;

	static MoDetSettings &instance() {
		static MoDetSettings modet_setting;
		return modet_setting;
	}

	void __print()const;

	bool __read(const std::string &_path);

public:
	static bool read(const std::string &_path) {
		return instance().__read(_path);
	}

	static std::string get_name() {
		return instance().m_name;
	}

	static int get_minimum_motion() {
		return instance().m_min_motion;
	}

	static double get_fps() {
		return instance().m_fps;
	}

	static int get_shut_off_delay(){
		return instance().m_shut_off_delay;
	}

	static int get_turn_on_delay(){
		return instance().m_turn_on_delay;
	}
	
	static int get_max_on_time(){
		return instance().m_max_on_time;
	}
	static int get_min_motion_percent(){
		return instance().m_min_motion_percent;
	}

	static void print() {
		instance().__print();
	}
	
};
