#include <stdio.h>
#include <signal.h>
#include <uci.h>
#include <stdlib.h>
#include <argp.h>
#include <iotp_device.h>
#include <memory.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include "invoke.h"
#include "arguments_parse.h"
#include "ibm_watson.h"


#define LOCKFILE "/var/lock/ibm-watson.lock"

char *configFilePath = NULL;

char *progname = "main";

int isLocked(int *fd)
{
    syslog(LOG_INFO, "Trying to lock program");
    *fd = open(LOCKFILE, O_RDWR|O_CREAT);
    if (*fd < 0) {
        syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
        return 1;
    }
     if (flock(*fd, LOCK_EX | LOCK_NB) < 0){
        syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
        return 1;
     }
    return 0;
}

void unlock(int *fd)
{
  syslog(LOG_INFO, "Unlocking");
    if (flock(*fd, LOCK_UN) == -1){
        exit(1);
    }
    close(*fd);
}

void usage(void) 
{
    fprintf(stderr, "Usage: %s --config config_file_path\n", progname);
    exit(1);
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[])
{
    int ret = 0;
    openlog("IBM_watson_program",LOG_PID, LOG_USER);
    int fd;

    int rrc = isLocked(&fd);

    if (rrc != 0){
      syslog(LOG_ERR, "Program is already locked");
      ret = 0;
      goto end;
    }
    else{
      syslog(LOG_INFO, "Program locked successfully");
    }

    struct arguments arguments;
    
	arguments_init(&arguments);
    int rc = 0;
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

    rc = watsonInit(&config, &device, arguments);
    if (rrc != 0){
      syslog(LOG_ERR, "Watson init failed");
      ret = 1;
      goto end;
    }

    sendMessageLoop(device);

    
    rc = watsonDisconnect(device);
    if (rrc != 0){
      syslog(LOG_ERR, "Watson disconnect failed");
      ret = 1;
      goto end;
    }

end:
    IoTPDevice_destroy(device);
    IoTPConfig_clear(config);
    closelog();
    unlock(&fd);

	return ret;
}