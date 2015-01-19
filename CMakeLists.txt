cmake_minimum_required(VERSION 2.8)

SET(COMPILE_DEFINITIONS -Werror)

include_directories(/opt/vc/include)
include_directories(/opt/vc/include/interface/vcos/pthreads)
include_directories(/opt/vc/include/interface/vmcs_host)
include_directories(/opt/vc/include/interface/vmcs_host/linux)
include_directories(/usr/local/include)
include_directories(/opt/vc/userland)
include_directories(/usr/include/opencv2)
include_directories(/opt/vc/src/hello_pi/libs/vgfont)

link_directories(/home/pi/git/robidouille/raspicam_cv)
link_directories(/opt/vc/lib)
link_directories(/opt/vc/lib)
link_directories(/opt/vc/src/hello_pi/libs/vgfont)
add_executable(mmal_opencv_demo control.c compass.c source.cpp opencv_demo.c)
find_package( OpenCV REQUIRED )
target_link_libraries(mmal_opencv_demo mmal_core mmal_util mmal_vc_client vcos bcm_host ${OpenCV_LIBS} vgfont openmaxil EGL -lwiringPi -lpthread -lrt -lfreetype -lraspicamcv -lboost_thread -lboost_system)


