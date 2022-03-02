void sendMessageLoop(IoTPDevice *device);
void sigHandler(int signo);
void MQTTTraceCallback (int level, char * message);
void setConfig(char *orgId, char *typeId, char *deviceId, char *token, IoTPConfig *config);