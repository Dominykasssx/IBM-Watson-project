#include <argp.h>
#include "arguments_parse.h"

void arguments_init(struct arguments *arguments)
{
    arguments->organization = "";
    arguments->type = "";
    arguments->device = "";
    arguments->token = "";
}

error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key){
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
