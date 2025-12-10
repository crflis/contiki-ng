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

void set_prefix_64(uip_ipaddr_t *);

void
request_prefix(void)
{
  uint8_t buf[2 + 8];
  size_t len = 2;

  buf[0] = '?';
  buf[1] = 'P';

#if UIP_LLADDR_LEN >= 8
  memcpy(&buf[len], &uip_lladdr.addr[UIP_LLADDR_LEN - 8], 8);
  len += 8;
#endif

  br_link_write(buf, len);
}

static void
cobs_input_callback(void)
{
  uint8_t decoded[UIP_BUFSIZE];
  size_t dec_len = cobs_decode(uip_buf, uip_len, decoded, sizeof(decoded));
  if(dec_len == 0) {
    uipbuf_clear();
    return;
  }

  if(dec_len > 0 && decoded[0] == '!') {
    if(br_side_in_dispatch('!', &decoded[1], dec_len > 1 ? dec_len - 1 : 0)) {
      uipbuf_clear();
      return;
    }
    if(dec_len > 1 && decoded[1] == 'P' && dec_len >= 10) {
      uip_ipaddr_t prefix;
      memset(&prefix, 0, 16);
      memcpy(&prefix, &decoded[2], 8);
      LOG_INFO("Setting prefix ");
      LOG_INFO_6ADDR(&prefix);
      LOG_INFO_("\n");
      set_prefix_64(&prefix);
    }
    uipbuf_clear();
    return;
  }

  if(dec_len > 0 && decoded[0] == '?') {
    if(br_side_in_dispatch('?', &decoded[1], dec_len > 1 ? dec_len - 1 : 0)) {
      uipbuf_clear();
      return;
    }
    /* No default handlers for other requests; drop. */
    uipbuf_clear();
    return;
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
