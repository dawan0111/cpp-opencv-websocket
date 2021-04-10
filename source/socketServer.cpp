#include <libwebsockets.h>

#include "../header/socketServer.h"
#include "../header/sensorData.h"

SocketServer::SocketServer(lws_context_creation_info &_info, const SensorData &_sensorData)
{
    info = _info;
    sensorData = _sensorData;
};

void *SocketServer::run(void* socketServer)
{
    struct lws_context *context = lws_create_context(&((SocketServer *) socketServer)->info);

    printf("starting server...\n");   

    while (1)
    {
        lws_service(context, 1000000);
    }

    lws_context_destroy( context );
}
