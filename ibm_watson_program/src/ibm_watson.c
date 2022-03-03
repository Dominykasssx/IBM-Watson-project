#include <iotp_device.h>
#include <syslog.h>
#include <stdlib.h>
#include "invoke.h"

volatile int interrupt = 0;
volatile sig_atomic_t deamonize = 1;

void sendMessageLoop(IoTPDevice *device)
{
    struct memory memory;
    int rc = 0;

    while (!interrupt)
    {
        getMemoryUse(&memory);
        char data[1024];
        //sprintf(data, "{\"Data_From_RUTX10\" : {\"Message1\": \"Test1\", \"Message2\": \"Test2\" }}");
         sprintf(data, "{\"totalMemory\": \"%ld\", \"freeMemory\":\"%ld\", \"sharedMemory\":\"%ld\",\"bufferedMemory\":\"%ld\"}",
                memory.totalMemory, memory.freeMemory, memory.sharedMemory, memory.bufferedMemory);
        rc = IoTPDevice_sendEvent(device, "status", data, "json", QoS0, NULL);

        if (rc != 0)
        {
            syslog(LOG_ERR, "Failed to send data");
        }
        else
        {
            syslog(LOG_INFO, "Data sent successfully");
        }
        sleep(10);
    }
}

void sigHandler(int signo) {
    signal(SIGINT, NULL);
    fprintf(stdout, "Received signal: %d\n", signo);
    interrupt = 1;
}

void term_proc(int sigterm) 
{
	deamonize = 0;
}

void MQTTTraceCallback (int level, char * message)
{
    if ( level > 0 )
        fprintf(stdout, "%s\n", message? message:"NULL");
    fflush(stdout);
}


void setConfig(char *orgId, char *typeId, char *deviceId, char *token, IoTPConfig *config)
{
    IoTPConfig_setProperty(config, "identity.orgId", orgId);
    IoTPConfig_setProperty(config, "identity.typeId", typeId);
    IoTPConfig_setProperty(config, "identity.deviceId", deviceId);
    IoTPConfig_setProperty(config, "auth.token", token);
}