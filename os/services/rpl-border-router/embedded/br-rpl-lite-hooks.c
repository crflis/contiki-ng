/*
 * RPL-lite event hooks for the border router side-channel.
 *
 * Overrides weak hooks in RPL-lite to emit compact !R messages over
 * the selected BR link framing.
 */

#include "contiki.h"
#include "os/services/rpl-border-router/embedded/br-link.h"
#include "os/services/rpl-border-router/embedded/br-rpl-hooks.h"
#include "net/routing/rpl-lite/rpl.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"
#include "net/routing/rpl-lite/rpl-icmp6.h"
#include <string.h>

#if BR_RPL_HOOKS_ENABLE

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

void
ha_rpl_lite_dao_event(const rpl_dao_t *dao, const uip_ipaddr_t *from)
{
  uint8_t payload[1 + 1 + 6];
  size_t pos = 0;
  uint8_t flags = (dao->lifetime == 0 ? 0x80 : 0x00) | (dao->lifetime & 0x7f);

  payload[pos++] = dao->prefixlen;
  payload[pos++] = flags;

  append_ip_suffix(payload, &pos, &dao->prefix);
  append_ip_suffix(payload, &pos, from);
  append_ip_suffix(payload, &pos, &dao->parent_addr);

  send_rpl_event(BR_RPL_EVT_DAO_SR, payload, pos);
}

void
ha_rpl_lite_parent_switch(const rpl_nbr_t *old_parent,
                          const rpl_nbr_t *new_parent)
{
  uint8_t payload[4];
  size_t pos = 0;

  append_ip_suffix(payload, &pos, old_parent ? rpl_neighbor_get_ipaddr(old_parent) : NULL);
  append_ip_suffix(payload, &pos, new_parent ? rpl_neighbor_get_ipaddr(new_parent) : NULL);

  send_rpl_event(BR_RPL_EVT_PARENT, payload, pos);
}

#endif /* BR_RPL_HOOKS_ENABLE */
