/*
 * RPL hook configuration for the border router side-channel.
 *
 * Enable with BR_RPL_HOOKS_ENABLE set to 1 in project-conf.h.
 */
#ifndef BR_RPL_HOOKS_H_
#define BR_RPL_HOOKS_H_

#ifndef BR_RPL_HOOKS_ENABLE
#define BR_RPL_HOOKS_ENABLE 0
#endif

/* Event tags sent over the side-channel */
#define BR_RPL_EVT_DAO_ROUTE   'd'
#define BR_RPL_EVT_DAO_SR      's'
#define BR_RPL_EVT_PARENT      'p'

#endif /* BR_RPL_HOOKS_H_ */
