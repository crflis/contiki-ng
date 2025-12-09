/*
 * RPL event hooks for the border router side-channel.
 *
 * Overrides weak hooks in rpl-classic to emit compact !R messages over
 * the selected BR link framing.
 */

#include "contiki.h"
#include "os/services/rpl-border-router/embedded/br-link.h"
#include "os/services/rpl-border-router/embedded/br-rpl-hooks.h"
#include "net/routing/rpl-classic/rpl.h"
#include <string.h>

#if BR_RPL_HOOKS_ENABLE

/* Helper: write two-byte suffix of an IPv6 address (network order). */
static void
append_ip_suffix(uint8_t *buf, size_t *pos, const uip_ipaddr_t *addr)
{
  uint16_t suffix = 0;
  if(addr != NULL) {
    suffix = UIP_HTONS(addr->u16[7]);
  }
  buf[(*pos)++] = (uint8_t)(suffix >> 8);
  buf[(*pos)++] = (uint8_t)(suffix & 0xff);
}

/* Send an !R event with provided payload. */
static void
send_rpl_event(uint8_t evt, const uint8_t *payload, size_t len)
{
  uint8_t buf[1 + 1 + 16];
  size_t pos = 0;

  buf[pos++] = '!';
  buf[pos++] = 'R';
  buf[pos++] = evt;

  if(len > sizeof(buf) - pos) {
    len = sizeof(buf) - pos;
  }
  if(payload != NULL && len > 0) {
    memcpy(&buf[pos], payload, len);
    pos += len;
  }

  br_link_write(buf, pos);
}

/* Store-mode DAO route event */
void
ha_rpl_dao_route_event(const rpl_dag_t *dag,
                       const uip_ipaddr_t *target,
                       uint8_t prefixlen,
                       const uip_ipaddr_t *sender,
                       uint8_t lifetime,
                       int is_nopath)
{
  uint8_t payload[1 + 1 + 4];
  size_t pos = 0;

  payload[pos++] = prefixlen;
  payload[pos++] = (uint8_t)((is_nopath ? 0x80 : 0x00) | (lifetime & 0x7f));

  append_ip_suffix(payload, &pos, target);
  append_ip_suffix(payload, &pos, sender);

  send_rpl_event(BR_RPL_EVT_DAO_ROUTE, payload, pos);
}

/* Non-storing DAO SR event */
void
ha_rpl_dao_sr_event(const rpl_dag_t *dag,
                    const uip_ipaddr_t *target,
                    uint8_t prefixlen,
                    const uip_ipaddr_t *sender,
                    const uip_ipaddr_t *parent,
                    uint8_t lifetime,
                    int is_nopath)
{
  uint8_t payload[1 + 1 + 6];
  size_t pos = 0;

  payload[pos++] = prefixlen;
  payload[pos++] = (uint8_t)((is_nopath ? 0x80 : 0x00) | (lifetime & 0x7f));

  append_ip_suffix(payload, &pos, target);
  append_ip_suffix(payload, &pos, sender);
  append_ip_suffix(payload, &pos, parent);

  send_rpl_event(BR_RPL_EVT_DAO_SR, payload, pos);
}

/* Preferred parent change / detach */
void
ha_rpl_parent_switch(const rpl_dag_t *dag,
                     const rpl_parent_t *old_parent,
                     const rpl_parent_t *new_parent)
{
  uint8_t payload[4];
  size_t pos = 0;

  append_ip_suffix(payload, &pos, old_parent ? rpl_parent_get_ipaddr(old_parent) : NULL);
  append_ip_suffix(payload, &pos, new_parent ? rpl_parent_get_ipaddr(new_parent) : NULL);

  send_rpl_event(BR_RPL_EVT_PARENT, payload, pos);
}

#endif /* BR_RPL_HOOKS_ENABLE */
