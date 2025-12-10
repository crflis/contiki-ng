/*
 * COBS side-channel bridge over the SLIP byte stream.
 *
 * Uses SLIP transport for byte movement but treats payloads as COBS-framed
 * side-channel messages for !/? tags.
 */

#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "dev/slip.h"
#include "dev/cobs.h"
#include "os/services/rpl-border-router/embedded/br-link.h"
#include "os/services/rpl-border-router/embedded/br-side-channel.h"

#include "sys/log.h"
#define LOG_MODULE "COBS"
#define LOG_LEVEL LOG_LEVEL_NONE

#if BR_CONF_LINK_FRAMING == BR_LINK_FRAMING_COBS

static void
cobs_input_callback(void)
{
  uint8_t decoded[UIP_BUFSIZE];
  size_t dec_len = cobs_decode(uip_buf, uip_len, decoded, sizeof(decoded));
  if(dec_len == 0) {
    uipbuf_clear();
    return;
  }

  if(dec_len > 0) {
    char tag = (char)decoded[0];
    if(tag == '!' || tag == '?') {
      if(br_side_in_dispatch(tag, &decoded[1], dec_len - 1)) {
        uipbuf_clear();
        return;
      }
    }
  }

  /* Not consumed: drop. */
  uipbuf_clear();
}

static void
init(void)
{
  slip_arch_init();
  process_start(&slip_process, NULL);
  slip_set_input_callback(cobs_input_callback);
}

static int
output(void)
{
  return 0;
}

const struct uip_fallback_interface rpl_interface = {
  init, output
};

#endif /* BR_CONF_LINK_FRAMING == BR_LINK_FRAMING_COBS */
