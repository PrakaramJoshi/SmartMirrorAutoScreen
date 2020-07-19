// MoDet.cpp : Defines the entry point for the console application.
//
#include "logger.h"
#include "DeviceCameraIn.h"
#include "motion_detector.h"
#include "CameraIn.h"
#include "MirrorState.h"
#include "modet_settings.h"
REGISTER_LOGGER("MoDet", "0.0.0.1");
void process_frames(BlockingQueue<cv::Mat> *_framesq) {
	motion_detector modet(_framesq);
	modet.run();
}

void camera_run(CameraBase *_camera) {
	_camera->run();
}

void MirrorManager_run(){
	MirrorStateManager msm;
	msm.run();
}

int main(int argc,char *argv[])
{
	if (argc != 2) {
		std::cout << "usage : app_config_file" << std::endl;
		return 1;
	}
	std::string app_config = argv[1];
	if (!MoDetSettings::read(app_config)) {
		return 1;
	}
	MoDetSettings::print();

	BlockingQueue<cv::Mat> frames_q("frames_q_to_motion_detector",15);

	CameraIn<DeviceCapture> camera(&frames_q);
		
	camera.init();
	
	std::thread frames_processor(std::bind(process_frames, &frames_q));
	std::thread camera_processor(std::bind(camera_run, &camera));
	std::thread mirror_processor(MirrorManager_run);
	camera_processor.join();
	frames_processor.join();
	mirror_processor.join();
	
    return 0;
}

