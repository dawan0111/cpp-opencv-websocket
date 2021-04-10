#include <iostream>
#include <opencv2/opencv.hpp>
#include <libwebsockets.h>
#include <pthread.h>
#include <chrono>
#include <thread>
#include <string>

#include "header/webcam.h"
#include "header/socketServer.h"
#include "header/sensorData.h"
#include "header/base64.h"


static std::vector<lws *> connectLWS;
static pthread_mutex_t mutex;

extern SensorData sensorData;

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	return 0;
}

static int callback_dumb_increment(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    using namespace std;
    cout << "reason:" << reason << endl;
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // connect event
            connectLWS.push_back(wsi);
            lws_callback_on_writable(wsi);
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: {
            unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
                                                         LWS_SEND_BUFFER_POST_PADDING);
            
            int i;
            for (i=0; i < len; i++) {
                buf[LWS_SEND_BUFFER_PRE_PADDING + (len - 1) - i ] = ((char *) in)[i];
            }
            printf("received data: %s, replying: %.*s\n", (char *) in, (int) len, buf + LWS_SEND_BUFFER_PRE_PADDING);
            lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
            
            free(buf);
            break;
        }
        case 11: {
             pthread_mutex_lock(&mutex);

            std::string encoded_png;
            std::vector<uchar> buf;
            cv::imencode(".png", sensorData.image, buf);
            auto base64_png = reinterpret_cast<const unsigned char*>(buf.data());
            encoded_png = "data:image/jpeg;base64," + base64_encode(base64_png, buf.size());

            const int len = encoded_png.size();
            std::cout << "lws: " << wsi << " sendData: " << len / 1000 << "KB" << std::endl;

            unsigned char* buffer = new unsigned char[LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING];
            for (int i = 0; i < len; i++) {
                buffer[LWS_SEND_BUFFER_PRE_PADDING + i] = encoded_png[i];
            }
            lws_write(wsi, &buffer[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);

            delete buffer;
            pthread_mutex_unlock(&mutex);
            lws_callback_on_writable(wsi);
            break;
        }
        case LWS_CALLBACK_CLIENT_CLOSED:
            break;
        default:
            break;
    }
    
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "http-only",
        callback_http,
        0
    },
    {
        "dumb-increment-protocol",
        callback_dumb_increment,
        0
    },
    {
        NULL, NULL, 0
    }
};

static void *send(void *)
{

}

int main()
{
    pthread_t webcamThread, socketThread, sendThread;
    int result;
    int result2;
    int result3;

    pthread_mutex_init(&mutex,NULL);

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = 9000;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    info.pt_serv_buf_size = 1024 * 1024 * 3; 

    sensorData.image = cv::Mat(cv::Size(500, 500), CV_8UC3);

    Webcam myWebCam(&sensorData.image);
    SocketServer mySocketServer(info, sensorData);

    pthread_create(&webcamThread, NULL, myWebCam.run, &myWebCam);
    pthread_create(&socketThread, NULL, mySocketServer.run, &mySocketServer);
    // pthread_create(&sendThread, NULL, send, NULL);

    pthread_join(webcamThread, (void **)&result);
    pthread_join(socketThread, (void **)&result2);
    // pthread_join(sendThread, (void **)&result3);

    pthread_mutex_destroy(&mutex);

    std::cout << "end!" << std::endl;

    return 0;
}