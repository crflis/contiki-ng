/*
 * Side-channel hook registration for the border router.
 *
 * Allows applications to register handlers for inbound side-channel frames
 * (tag + payload) and to tap outbound frames before framing (SLIP/COBS).
 */
#ifndef BR_SIDE_CHANNEL_H_
#define BR_SIDE_CHANNEL_H_

#include <stddef.h>
#include <stdint.h>

typedef int (*br_side_in_handler_t)(char tag, const uint8_t *data, size_t len);
typedef void (*br_side_out_hook_t)(const uint8_t *payload, size_t len);

/* Register an inbound handler for a given tag (e.g., '!P', '?X').
 * Return 0 on success, -1 if no slots left. */
int br_side_in_register(char tag, br_side_in_handler_t handler);

/* Dispatch an inbound frame; returns 1 if handled, 0 otherwise. */
int br_side_in_dispatch(char tag, const uint8_t *data, size_t len);

/* Register an outbound hook (single hook for now). */
void br_side_out_register(br_side_out_hook_t hook);

/* Notify outbound hook with raw payload (before framing). */
void br_side_out_notify(const uint8_t *payload, size_t len);

#endif /* BR_SIDE_CHANNEL_H_ */
