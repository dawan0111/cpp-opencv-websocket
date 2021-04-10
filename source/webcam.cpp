#include <iostream>
#include "../header/webcam.h"

extern int IMAGE_WIDTH;
extern int IMAGE_HEIGHT;

Webcam::Webcam(cv::Mat* _image)
{
    image = _image;
}

void *Webcam::run(void* cam)
{
    std::cout << "OpenCV Version : " << CV_VERSION << std::endl;
    cv::namedWindow("EXAMPLE02", 1);
    cv::VideoCapture cap;

    std::cout <<  "webcam: " << ((Webcam *) cam)->image << std::endl;
    
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
    // cap.open("/dev/video4");

    cap.open(-1);

    std::cout << "frame Width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;

    while (cap.isOpened())
    {
        cap >> *((Webcam *) cam)->image;
        cv::imshow("EXAMPLE02", *((Webcam *) cam)->image);
        if (cv::waitKey(30)==27)
        {
            break;
        }
    }
    cv::destroyWindow("EXAMPLE02");  
}
