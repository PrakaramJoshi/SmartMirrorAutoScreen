# SmartMirrorAutoScreen
This project uses camera attached to raspberry pi to turn on/off the SmartMirror screen based on motion. 
requires OpenCV to be compiled and installed.
Also enable camera via raspi-config and run sudo modprobe bcm2835-v4l2 before running. 
To run ./MoDet webcam_linux.ini

the webcam_linux.ini has basic configuration information. the fps parameter can be used to limit the cpu utilization.
