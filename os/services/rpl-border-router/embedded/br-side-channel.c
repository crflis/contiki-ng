/*
 * Side-channel hook registration for the border router.
 */

#include "os/services/rpl-border-router/embedded/br-side-channel.h"

#define MAX_IN_HANDLERS 4

typedef struct {
  char tag;
  br_side_in_handler_t handler;
} in_entry_t;

static in_entry_t in_handlers[MAX_IN_HANDLERS];
static br_side_out_hook_t out_hook;

int
br_side_in_register(char tag, br_side_in_handler_t handler)
{
  if(handler == NULL) {
    return -1;
  }
  for(int i = 0; i < MAX_IN_HANDLERS; i++) {
    if(in_handlers[i].handler == NULL) {
      in_handlers[i].tag = tag;
      in_handlers[i].handler = handler;
      return 0;
    }
  }
  return -1;
}

int
br_side_in_dispatch(char tag, const uint8_t *data, size_t len)
{
  for(int i = 0; i < MAX_IN_HANDLERS; i++) {
    if(in_handlers[i].handler != NULL && in_handlers[i].tag == tag) {
      if(in_handlers[i].handler(tag, data, len)) {
        return 1;
      }
    }
  }
  return 0;
}

void
br_side_out_register(br_side_out_hook_t hook)
{
  out_hook = hook;
}

void
br_side_out_notify(const uint8_t *payload, size_t len)
{
  if(out_hook != NULL) {
    out_hook(payload, len);
  }
}
