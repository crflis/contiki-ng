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
append_iid64(uint8_t *buf, size_t *pos, const uip_ipaddr_t *addr)
{
  if(addr == NULL) {
    memset(&buf[*pos], 0, 8);
    *pos += 8;
    return;
  }
  memcpy(&buf[*pos], &addr->u16[4], 8);
  *pos += 8;
}

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

void
ha_rpl_lite_dao_event(const rpl_dao_t *dao, const uip_ipaddr_t *from)
{
  uint8_t payload[8 + 8 + 8 + 2];
  size_t pos = 0;
  uint8_t flags = (dao->lifetime == 0 ? 0x80 : 0x00) | (dao->lifetime & 0x7f);

  append_iid64(payload, &pos, &dao->prefix);
  append_iid64(payload, &pos, from);
  append_iid64(payload, &pos, &dao->parent_addr);
  payload[pos++] = dao->prefixlen;
  payload[pos++] = flags;

  send_rpl_event(BR_RPL_EVT_DAO_SR, payload, pos);
}

void
ha_rpl_lite_parent_switch(const rpl_nbr_t *old_parent,
                          const rpl_nbr_t *new_parent)
{
  uint8_t payload[16];
  size_t pos = 0;

  append_iid64(payload, &pos, old_parent ? rpl_neighbor_get_ipaddr((rpl_nbr_t *)old_parent) : NULL);
  append_iid64(payload, &pos, new_parent ? rpl_neighbor_get_ipaddr((rpl_nbr_t *)new_parent) : NULL);

  send_rpl_event(BR_RPL_EVT_PARENT, payload, pos);
}

#endif /* BR_RPL_HOOKS_ENABLE */
