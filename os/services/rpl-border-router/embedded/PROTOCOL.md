# Border Router Side-Channel Protocol

This documents the side-channel messages emitted by the embedded border router (BR) and how to enable/parse them. It applies to both SLIP and COBS framing; only the outer framing changes.

## Framing & Transport
- Build-time select with `BR_CONF_LINK_FRAMING`:
  - `BR_LINK_FRAMING_SLIP` (default): SLIP framing, handled by `slip-bridge.c`.
  - `BR_LINK_FRAMING_COBS`: COBS framing with 0x00 delimiter, handled by `cobs-bridge.c` (still over the serial byte pipe).
- All side-channel payloads (`!L`, `!R`, `!P`, etc.) are passed to `br_link_write()` and framed per the selection. Host must match.

## Configuration knobs (project-conf.h / Makefile)
- Logging over link: `BR_LOG_CONF_ENABLE 1`, include `br-log-conf.h`.
- RPL event export: `BR_RPL_HOOKS_ENABLE 1`, include `br-rpl-hooks.h`.
- Optional COBS framing: `#define BR_CONF_LINK_FRAMING BR_LINK_FRAMING_COBS` before including `br-log-conf.h`.
- Force RPL-classic (Makefile preferred): `MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC`.

## Message types (outbound from BR)
- Logs: `!L<log-text>` (same prefixes as stdout), framed per SLIP/COBS.
- Prefix exchange:
  - BR request: `?P` + optional IID64 (last 8 bytes of link-layer address).
  - Host response: `!P` + 8-byte prefix (first 64 bits of /64). BR sets prefix and becomes root.
- RPL events: `!R<code><payload>` (IID64-based v2):
  - Codes: `D` (DAO storing), `S` (DAO SR/non-storing), `P` (parent switch).
  - `D` payload: `target_iid64` (8B) | `sender_iid64` (8B) | `prefixlen` (1B) | `flags` (1B: bit7 no-path, low7 lifetime).
  - `S` payload: `target_iid64` | `sender_iid64` | `parent_iid64` | `prefixlen` | `flags` (as above).
  - `P` payload: `old_parent_iid64` | `new_parent_iid64` (all zeros if none).
- Event emitters exist for both RPL-classic and RPL-lite (selected by Makefile routing choice).

## Side-channel dispatcher (application hooks)
- Inbound: `br_side_in_register(tag, handler)`; if handler returns non-zero, core handling is skipped. Tags are the first byte (`!`, `?`, etc.).
- Outbound tap: `br_side_out_register(hook)` to observe raw payloads (before framing), e.g., `!L` / `!R`.
- Include `br-side-channel.h` in your app to register.

## Host expectations
- Match framing (SLIP or COBS+0x00).
- Parse tags:
  - `!L`: text log.
  - `!R`: as above.
  - `?P`: prefix request + optional IID64; respond with `!P`.
- `!R` is not printed on BR stdout by default; use outbound tap if needed.

