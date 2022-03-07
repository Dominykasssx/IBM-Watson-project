#include <iotp_device.h>
#include <syslog.h>
#include <stdlib.h>
#include <signal.h>
#include "invoke.h"
#include <argp.h>
#include "arguments_parse.h"
#include "ibm_watson.h"

volatile int interrupt = 0;
volatile sig_atomic_t deamonize = 1;

void sendMessageLoop(IoTPDevice *device)
{
    struct memory memory;
    int rc = 0;
    IoTPEventCallbackHandler cb;

    while (!interrupt)
    {
        getMemoryUse(&memory);
        char data[1024];
        sprintf(data, "{\"totalMemory\": \"%ld\", \"freeMemory\":\"%ld\", \"sharedMemory\":\"%ld\",\"bufferedMemory\":\"%ld\"}",
                memory.totalMemory, memory.freeMemory, memory.sharedMemory, memory.bufferedMemory);
        rc = IoTPDevice_sendEvent(device, "status", data, "json", QoS2, NULL);

        int rc1 = IoTPDevice_setEventCallback(device, cb);
        

        if (rc != IOTPRC_SUCCESS || rc1 != IOTPRC_SUCCESS)
        {
            syslog(LOG_ERR, "Failed to send data");
        }
        else
        {
            syslog(LOG_INFO, "Data sent successfully");
        }
        sleep(5);
    }
}

void sigHandler(int signo)
{
    signal(SIGINT, NULL);
    fprintf(stdout, "Received signal: %d\n", signo);
    interrupt = 1;
}

void term_proc(int sigterm)
{
    deamonize = 0;
}

void MQTTTraceCallback(int level, char *message)
{
    if (level > 0){
        syslog(LOG_ERR, "MQTT callback send: %s\n", message ? message : "NULL");
    }
}

void setConfig(struct arguments arguments, IoTPConfig **config)
{
    syslog(LOG_DEBUG, "Set config start");
    IoTPConfig_setProperty(*config, "identity.orgId", arguments.organization);
    IoTPConfig_setProperty(*config, "identity.typeId", arguments.type);
    IoTPConfig_setProperty(*config, "identity.deviceId", arguments.device);
    IoTPConfig_setProperty(*config, "auth.token", arguments.token);
}

void createConfig(IoTPConfig *config)
{
    /* Set IoTP Client log handler */
    int rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, stdout);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to set Log Handler");
    }

    /* Create IoTPConfig object using configuration options defined in the configuration file. */
    rc = IoTPConfig_create(&config, NULL);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to create config object");
    }
}
int watsonInit(IoTPConfig **config, IoTPDevice **device, struct arguments arguments)
{
    /* Set signal handlers */
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    syslog(LOG_DEBUG, "Log handler");
    /* Set IoTP Client log handler */
    int rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, stdout);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to set Log Handler");
        closelog();
        exit(1);
    }
    syslog(LOG_DEBUG, "Config object");
    /* Create IoTPConfig object using configuration options defined in the configuration file. */
    rc = IoTPConfig_create(config, NULL);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to create config object");
        return 1;
    }
    syslog(LOG_DEBUG, "Set config");
    setConfig(arguments, config);

    syslog(LOG_DEBUG, "Create device object");
    /* Create IoTPDevice object */
    rc = IoTPDevice_create(device, *config);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to create device object");
        return 1;
    }
    syslog(LOG_DEBUG, "MQTT handler");
    /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(*device, &MQTTTraceCallback);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to set MQTT trace handler");
        return 1;
    }
    syslog(LOG_DEBUG, "Watson api connect");
    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(*device);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to connect to Watson IoT API");
        return 1;
    }
}

int watsonDisconnect(IoTPDevice *device)
{
    syslog(LOG_DEBUG, "Disconnecting device");
    int rc = IoTPDevice_disconnect(device);
    if (rc != IOTPRC_SUCCESS)
    {
        syslog(LOG_ERR, "Failed to disconnect from IoT Device");
        return 1;
    }
    return 0;
}