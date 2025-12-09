/*
 * Border router link framing selector (SLIP or COBS).
 */

#include "os/services/rpl-border-router/embedded/br-link.h"

#if BR_CONF_LINK_FRAMING == BR_LINK_FRAMING_SLIP
#include "dev/slip.h"
#elif BR_CONF_LINK_FRAMING == BR_LINK_FRAMING_COBS
#include "dev/cobs.h"
#include "dev/slip.h"
#else
#error "Unsupported BR_CONF_LINK_FRAMING"
#endif

void
br_link_write(const uint8_t *payload, size_t len)
{
  if(payload == NULL || len == 0) {
    return;
  }

#if BR_CONF_LINK_FRAMING == BR_LINK_FRAMING_SLIP
  slip_write(payload, len);
#elif BR_CONF_LINK_FRAMING == BR_LINK_FRAMING_COBS
  /* COBS framing with trailing zero delimiter. */
  size_t max_out = len + len / 254 + 2;
  uint8_t encoded[max_out];

  size_t enc_len = cobs_encode(payload, len, encoded, sizeof(encoded));
  if(enc_len == 0) {
    return;
  }

  for(size_t i = 0; i < enc_len; i++) {
    slip_arch_writeb(encoded[i]);
  }
  slip_arch_writeb(0x00);
#endif
}
