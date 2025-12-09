/*
 * Border router link framing selector.
 *
 * Provides a single write helper that can emit either SLIP or COBS framed
 * messages depending on BR_CONF_LINK_FRAMING.
 */
#ifndef BR_LINK_H_
#define BR_LINK_H_

#include "contiki.h"
#include <stddef.h>
#include <stdint.h>

#define BR_LINK_FRAMING_SLIP 0
#define BR_LINK_FRAMING_COBS 1

#ifndef BR_CONF_LINK_FRAMING
#define BR_CONF_LINK_FRAMING BR_LINK_FRAMING_SLIP
#endif

/* Write a framed payload to the underlying link. */
void br_link_write(const uint8_t *payload, size_t len);

#endif /* BR_LINK_H_ */
