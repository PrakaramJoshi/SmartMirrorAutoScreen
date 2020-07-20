#include "motion_detector.h"
#include "MotionEvent.h"
#include "logger.h"
//#define DEBUG_LOGS
motion_detector::motion_detector(BlockingQueue<cv::Mat> *_inq) {
	m_inq = _inq;
}

void motion_detector::run() {
	cv::Mat *mat=nullptr;
	cv::Mat diffused_frame;
	cv::Mat diff_frame;
	cv::Mat prev_frame;
	cv::Mat diff_threshold;
	cv::Mat non_Zero;
	cv::Mat gray;
	
	int motion_count = 0;
	int min_motion = MoDetSettings::get_minimum_motion();
	int min_motion_percent = MoDetSettings::get_min_motion_percent();
#ifdef DEBUG_LOGS
	std::size_t frame_id=0;
	std::size_t last_motion_frame = 0;
#endif
	while (m_inq->Remove(&mat)) {
		if (mat) {
#ifdef DEBUG_LOGS
			frame_id++;
#endif
			cv::cvtColor(*mat, gray, cv::COLOR_RGB2GRAY);
			cv::GaussianBlur(gray,
				diffused_frame,
				cv::Size(5, 5),
				2.2);
			if (prev_frame.size().area()!=0) {
				
				cv::subtract(prev_frame, diffused_frame, diff_frame);
				
				double area = diff_frame.size().area();
				cv::threshold(diff_frame, diff_threshold, 10, 255, cv::THRESH_TOZERO);
				
				cv::findNonZero(diff_threshold, non_Zero);
				double non_zero_area = non_Zero.size().area();
				double motion_percent = non_zero_area*100.0 / area;
				
#ifdef DEBUG_LOGS
				AceLogger::Log("frame : "+std::to_string(frame_id)+\
				" motion : " + std::to_string(motion_percent)+\
				" motion count : "+std::to_string(motion_count)+\
				" last motion frame : "+std::to_string(last_motion_frame));				
#endif
				if (motion_percent>min_motion_percent) {
					motion_count++;
#ifdef DEBUG_LOGS
					last_motion_frame = frame_id;
#endif
				}
				else{
					motion_count--;
					if(motion_count<0){
						motion_count = 0;
					}
				}
				if(motion_count>=min_motion){
					MotionEvent::set_motion();
					motion_count = 0;
				}
			}
			
			diffused_frame.copyTo(prev_frame);
		}
		delete mat;
		mat = nullptr;
		
	}
}