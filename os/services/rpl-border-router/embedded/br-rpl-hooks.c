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
append_iid64(uint8_t *buf, size_t *pos, const uip_ipaddr_t *addr)
{
  if(addr == NULL) {
    memset(&buf[*pos], 0, 8);
    *pos += 8;
    return;
  }
  /* Copy last 64 bits (IID) */
  memcpy(&buf[*pos], &addr->u16[4], 8);
  *pos += 8;
}

/* Send an !R event with provided payload. */
static void
send_rpl_event(uint8_t evt, const uint8_t *payload, size_t len)
{
  uint8_t buf[1 + 1 + 40];
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

static const uip_ipaddr_t *
parent_ipaddr_const(const rpl_parent_t *p)
{
  return p ? rpl_parent_get_ipaddr((rpl_parent_t *)p) : NULL;
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
  uint8_t payload[8 + 8 + 2];
  size_t pos = 0;

  append_iid64(payload, &pos, target);
  append_iid64(payload, &pos, sender);
  payload[pos++] = prefixlen;
  payload[pos++] = (uint8_t)((is_nopath ? 0x80 : 0x00) | (lifetime & 0x7f));

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
  uint8_t payload[8 + 8 + 8 + 2];
  size_t pos = 0;

  append_iid64(payload, &pos, target);
  append_iid64(payload, &pos, sender);
  append_iid64(payload, &pos, parent);
  payload[pos++] = prefixlen;
  payload[pos++] = (uint8_t)((is_nopath ? 0x80 : 0x00) | (lifetime & 0x7f));

  send_rpl_event(BR_RPL_EVT_DAO_SR, payload, pos);
}

/* Preferred parent change / detach */
void
ha_rpl_parent_switch(const rpl_dag_t *dag,
                     const rpl_parent_t *old_parent,
                     const rpl_parent_t *new_parent)
{
  uint8_t payload[16];
  size_t pos = 0;

  append_iid64(payload, &pos, parent_ipaddr_const(old_parent));
  append_iid64(payload, &pos, parent_ipaddr_const(new_parent));

  send_rpl_event(BR_RPL_EVT_PARENT, payload, pos);
}

#endif /* BR_RPL_HOOKS_ENABLE */
