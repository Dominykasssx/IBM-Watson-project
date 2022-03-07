
void sendMessageLoop(IoTPDevice *device);
void createConfig(IoTPConfig *config);
int watsonInit(IoTPConfig **config, IoTPDevice **device, struct arguments arguments);
int watsonDisconnect(IoTPDevice *device);