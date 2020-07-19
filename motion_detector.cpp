#include "motion_detector.h"
#include "MotionEvent.h"
#include "logger.h"

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
	while (m_inq->Remove(&mat)) {
		if (mat) {
			cv::cvtColor(*mat, gray, cv::COLOR_RGB2GRAY);
			cv::GaussianBlur(gray,
				diffused_frame,
				cv::Size(5, 5),
				2.2);
			if(!MotionEvent::is_motion_detection_enabled()){
				// if motion detection is disabled,
				// update the prev frame and continue.
				diffused_frame.copyTo(prev_frame);
				delete mat;
				mat = nullptr;
				continue;
			}
			bool motion = false;
			if (prev_frame.size().area()!=0) {
				
				cv::subtract(prev_frame, diffused_frame, diff_frame);
				
				double area = diff_frame.size().area();
				cv::threshold(diff_frame, diff_threshold, 10, 255, cv::THRESH_TOZERO);
				
				cv::findNonZero(diff_threshold, non_Zero);
				double non_zero_area = non_Zero.size().area();
				double motion_percent = non_zero_area*100.0 / area;
				
#ifdef DEBUG_LOGS
				AceLogger::Log("motion : " + std::to_string(motion_percent));
#endif
				if (motion_percent> 2.0) {
					motion = true;
#ifdef DEBUG_LOGS
					AceLogger::Log(" Has motion   : " + std::string(m_v.has_motion() ? "Yes" : "No"));
					AceLogger::Log(" Motion count : " + std::to_string(m_v.get_motion_count()));
#endif
				}
			}
			if(motion){
				motion_count++;
			}
			else{
				motion_count = 0;
			}
			if(motion>=min_motion){
				MotionEvent::set_motion();
			}
			
			diffused_frame.copyTo(prev_frame);
		}
		delete mat;
		mat = nullptr;
		
	}
}