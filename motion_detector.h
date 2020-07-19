#pragma once
#include "BlockingQueue.h"
#include "opencv_include.h"
#include "input_property.h"
#include <chrono>
#include <list>
#include <functional>
#include "modet_settings.h"
#include "logger.h"


class motion_detector {
	BlockingQueue<cv::Mat> * m_inq;

	motion_detector()=delete;
	motion_detector(const motion_detector&) = delete;
	motion_detector(motion_detector &&) = delete;
	motion_detector& operator=(const motion_detector&) = delete;
	motion_detector& operator=(motion_detector&&) = delete;

public:
	motion_detector(BlockingQueue<cv::Mat> *_inq);
	void run();
};