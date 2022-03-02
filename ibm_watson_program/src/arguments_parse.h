static char args_doc[] = "";
static char doc[] = "IBM cloud device daemon program";

static struct argp_option options[] = {
    {"organization", 'o', "ORGANIZATION", 0, "Organization ID"},
    {"type", 't', "TYPE", 0, "Type ID"},
    {"device", 'd', "DEVICE", 0, "Device ID"},
    {"token", 'a', "TOKEN", 0, "Authentication token"},
    {0}};

struct arguments
{
    char *organization, *type, *device, *token;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state);

void arguments_init( struct arguments *arguments);
