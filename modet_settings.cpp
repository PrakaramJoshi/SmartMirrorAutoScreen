#include "modet_settings.h"
#include "app_settings.h"
#include "logger.h"

MoDetSettings::MoDetSettings() {
	m_name = "";
	m_min_motion = 0;
	m_fps = 0.0;
}

bool MoDetSettings::__read(const std::string &_path) {
	INIReader reader(_path);

	if (reader.ParseError() < 0) {
		AceLogger::LogErr("Can't load " + _path);
		return false;
	}

	m_name = reader.Get("link", "name", "UNKNOWN");
	m_min_motion = reader.GetInteger("motion_detection", "minimum_motion", 0);
	m_min_motion_percent = reader.GetReal("motion_detection", "min_motion_percent", 2.0);
	m_fps = static_cast<double>(reader.GetInteger("fps", "limit", 0));
	m_max_on_time = static_cast<int>(reader.GetInteger("mirror_settings","max_on_time",1200));
	m_shut_off_delay = static_cast<int>(reader.GetInteger("mirror_settings","shut_off_delay",300));
	m_turn_on_delay = static_cast<int>(reader.GetInteger("mirror_settings","turn_on_delay",600));
	return true;
}

void MoDetSettings::__print()const {
	std::string str ;
	str += ("\nname\t\t\t:\t" + m_name + "\n");
	str += ("min motion seconds\t:\t" + std::to_string(m_min_motion) + "\n");
	str += ("fps\t\t\t:\t" + std::to_string(m_fps) + "\n");
	str += ("min_motion_percent\t:\t" + std::to_string(m_min_motion_percent) + "\n");
	str += ("max_on_time\t\t:\t" + std::to_string(m_max_on_time) + "\n");
	str += ("shut_off_delay\t\t:\t" + std::to_string(m_shut_off_delay) + "\n");
	str += ("turn_on_delay\t\t:\t" + std::to_string(m_turn_on_delay) + "\n");
	AceLogger::Log(str);
}