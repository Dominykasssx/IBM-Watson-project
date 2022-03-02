#include <stdio.h>
#include <signal.h>
#include <uci.h>
#include <stdlib.h>
#include <argp.h>
#include <iotp_device.h>
#include <memory.h>
#include <unistd.h>
#include <syslog.h>
#include "invoke.h"

volatile sig_atomic_t deamonize = 1;

char *configFilePath = NULL;
volatile int interrupt = 0;
char *progname = "program";
int useEnv = 0;
int testCycle = 0;

void usage(void) {
    fprintf(stderr, "Usage: %s --config config_file_path\n", progname);
    exit(1);
}

void sigHandler(int signo) {
    signal(SIGINT, NULL);
    fprintf(stdout, "Received signal: %d\n", signo);
    interrupt = 1;
}

void  deviceCommandCallback (char* type, char* id, char* commandName, char *format, void* payload, size_t payloadSize)
{
    fprintf(stdout, "Received device command:\n");
    fprintf(stdout, "Type=%s ID=%s CommandName=%s Format=%s Len=%d\n", 
        type?type:"", id?id:"", commandName?commandName:"", format?format:"", (int)payloadSize);
    fprintf(stdout, "Payload: %s\n", payload?(char *)payload:"");
}

void logCallback (int level, char * message)
{
    if ( level > 0 )
        fprintf(stdout, "%s\n", message? message:"NULL");
    fflush(stdout);
}


void MQTTTraceCallback (int level, char * message)
{
    if ( level > 0 )
        fprintf(stdout, "%s\n", message? message:"NULL");
    fflush(stdout);
}

void term_proc(int sigterm) 
{
	deamonize = 0;
}

struct arguments{ char *organization, *type, *device, *token;};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'o':
      arguments->organization = arg;
      break;
    case 't':
      arguments->type = arg;
      break;
    case 'd':
      arguments->device = arg;
      break;
	  case 'k':
      arguments->token = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num > 5)
        argp_usage (state);
      break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp_option options[] = {
    {"organization", 'o', "ORGANIZATION", 0, "Organization ID"},
    {"type", 't', "TYPE", 0, "Type ID"},
    {"device", 'd', "DEVICE", 0, "Device ID"},
    {"token", 'k', "TOKEN", 0, "Authentication token"}};

static char doc[] ="IBM Watson program";
static char args_doc[] = "Organization Type Device Token";

static struct argp argp = {options, parse_opt, args_doc, doc};

void send_message(IoTPDevice *device)
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
        rc = IoTPDevice_sendEvent(device, "status", &data, "json", QoS0, NULL);

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

int main(int argc, char *argv[])
{
	struct arguments arguments;

	  arguments.organization = "";
    arguments.type = "";
    arguments.device = "";
    arguments.token = "";
    int rc = 0;
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;
    openlog("IBM_watson_program",LOG_PID, LOG_USER);

	  argp_parse(&argp, argc, argv, 0, 0, &arguments);

	  //printf ("ORG = %s\nTYPE = %s\nDEVICE = %s\nTOKEN= %s\n", 
		  //arguments.organization, arguments.type, arguments.device, arguments.token);


    /* Set signal handlers */
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    /* Set IoTP Client log handler */
    rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, stdout);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to set Log Handler");
        goto end;
    }

    /* Create IoTPConfig object using configuration options defined in the configuration file. */
    rc = IoTPConfig_create(&config, NULL);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to create config object");
        goto end;
    }
    
    IoTPConfig_setProperty(config, "identity.orgId", arguments.organization);
    IoTPConfig_setProperty(config, "identity.typeId", arguments.type);
    IoTPConfig_setProperty(config, "identity.deviceId", arguments.device);
    IoTPConfig_setProperty(config, "auth.token", arguments.token);


    /* Create IoTPDevice object */
    rc = IoTPDevice_create(&device, config);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to create device object");
        goto end;
    }

    /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(device, &MQTTTraceCallback);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to set MQTT trace handler");
        goto end;
    }

    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(device);
    if (rc != 0)
    {
        syslog(LOG_ERR, "Failed to connect to Watson IoT API");
        goto end;
    }

    send_message(device);


    rc = IoTPDevice_disconnect(device);
    if (rc != IOTPRC_SUCCESS)
    {
      syslog(LOG_ERR, "Failed to disconnect from IoT Device");
        goto end;
    }

end:
    IoTPDevice_destroy(device);
    IoTPConfig_clear(config);
    closelog();

	return 0;
}
