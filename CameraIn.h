#pragma once
#ifndef CAMERAIN_H
#define CAMERAIN_H
#include "BlockingQueue.h"
#include "opencv_include.h"
#include <atomic>

class CameraBase {
	CameraBase(const CameraBase&) = delete;
	CameraBase(CameraBase&&) = delete;
	CameraBase& operator=(const CameraBase&) = delete;
	CameraBase& operator=(CameraBase&&) = delete;
public:
	CameraBase() {

	}

	virtual ~CameraBase() {
	}

	virtual bool init() {
		return true;
	}

	//blocking call
	virtual void run() {
	}

	virtual void stop() {
	}
};
template<typename Device>
class CameraIn:public CameraBase
{
	BlockingQueue<cv::Mat> m_frames_read;
	BlockingQueue<cv::Mat> *m_frames_out;
	std::thread *m_thread;
	std::atomic<bool>m_keep_alive;
	std::mutex m_mutex;
	Device* m_devicecapture;

	void display_frame() {
		AceLogger::Log("Starting frame relay...");
		cv::Mat *frame;
		cv::Mat channel[3];
		while (m_frames_read.Remove(&frame)) {
			if (frame) {
				if (frame->empty()) {
					delete frame;
					continue;
				}
				m_frames_out->Insert(frame);
				
			}
		}
		m_frames_out->ShutDown();
		AceLogger::Log("Frame relay stopped!");

	}

	void capture_frame() {

		AceLogger::Log("Starting the frame capture...");
		while (true) {
			m_devicecapture->capture(&m_frames_read, &m_keep_alive);
			if (m_keep_alive) {
				init();
			}
			else {
				break;
			}
		}
		m_frames_read.ShutDown();
		AceLogger::Log("Frame capture complete!");

	}

	CameraIn();

	CameraIn(const CameraIn&)=delete;

	CameraIn*operator=(const CameraIn&)=delete;

	void init_devicecapture() {
		if (m_devicecapture) {
			delete m_devicecapture;
		}
		m_devicecapture = new Device();
		while (!m_devicecapture->init_capture()) {
			AceLogger::LogErr("Unable to initialize cam");
			AceLogger::LogErr("Retrying in 10 seconds.");
			std::this_thread::sleep_for(std::chrono::seconds(10));
			delete m_devicecapture;
			m_devicecapture = new Device();
		}
	
	}

public:
	CameraIn(BlockingQueue<cv::Mat> *_frames_out):m_frames_read("frames_read_to_relay",15) {
		m_frames_out	= _frames_out;
		m_keep_alive	= true;
		m_thread 		= nullptr;
		m_devicecapture = nullptr;
	}

	~CameraIn() {
		delete m_thread;
		delete m_devicecapture;
	}

	bool init() {
		init_devicecapture();
		return true;
	}

	//blocking call
	void run() {
		AceLogger::Log("Starting the frame relay thread...");
		m_thread = new std::thread(std::bind(std::mem_fn(&CameraIn::display_frame), this));
		AceLogger::Log("Started!");
		capture_frame();
		AceLogger::Log("Waiting for the frame relay thread to finish...");
		m_thread->join();
		AceLogger::Log("Camera operations finished...");
	}

	void stop() {
		m_keep_alive = false;
		AceLogger::Log("Camera input will be stopped...");
	}
};
#endif