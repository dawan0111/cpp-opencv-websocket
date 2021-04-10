#include <iostream>
#include <map>
#include <chrono>
#include <thread>

#include "../header/webcam.h"
#include "../header/sensorData.h"
#include "../header/base64.h"

extern int IMAGE_WIDTH;
extern int IMAGE_HEIGHT;

Webcam::Webcam(SensorData* _sensorData)
{
    sensorData = _sensorData;
}

void *Webcam::run(void* cam)
{
    std::cout << "OpenCV Version : " << CV_VERSION << std::endl;
    cv::namedWindow("EXAMPLE02", 1);
    cv::VideoCapture cap;
    SensorData & camSensorData = *((Webcam *) cam)->sensorData;

    std::cout <<  "webcam: " << &camSensorData.image << std::endl;
    
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
    // cap.open("/dev/video4");

    cap.open(-1);

    std::cout << "frame Width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;

    while (cap.isOpened())
    {
        cap >> camSensorData.image;
        cv::Mat resizeImage;

        float ratio = camSensorData.image.cols / camSensorData.image.rows;
        cv::resize(camSensorData.image, resizeImage, cv::Size( 200, 200 * ratio ));

        std::string encoded_png;
        std::vector<uchar> buf;
        cv::imencode(".png", resizeImage, buf);
        auto base64_png = reinterpret_cast<const unsigned char*>(buf.data());
        encoded_png = "data:image/jpeg;base64," + base64_encode(base64_png, buf.size());

        camSensorData.imageQueueTiming[camSensorData.imageTiming] = encoded_png;
        camSensorData.imageQueueTiming.erase(camSensorData.imageTiming - 1000);

        camSensorData.imageTiming = camSensorData.imageTiming + 1;

        cv::imshow("EXAMPLE02", camSensorData.image);
        if (cv::waitKey(30)==27)
        {
            break;
        }
    }
    cv::destroyWindow("EXAMPLE02");  
}
