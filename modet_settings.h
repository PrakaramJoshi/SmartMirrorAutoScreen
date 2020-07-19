#pragma once
#include <string>
class MoDetSettings {

	std::string m_name;
	int m_min_motion;
	double m_fps;

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

	static void print() {
		instance().__print();
	}
	
};
