This is a border router that runs embedded in a node. The node runs a full
6LoWPAN stack, and acts as a DAG root. It interfaces to the outside world
via a serial line. On the host Operating System, `tunslip6` is used to create
a tun interface and bridge it to the RPL border router. This is achieved with
makefile targets `connect-router` and `connect-router-cooja`.

## SLIP log export

The embedded variant can ship Contiki-NG logs over the SLIP control channel.

- The hook lives in `br-log-conf.h` / `br-log.c` and is opt-in:
  set `BR_LOG_CONF_ENABLE` to `1` (see `examples/rpl-border-router/project-conf.h`).
- Each flushed log line is sent as a SLIP frame prefixed with `!L`, followed
  by the normal log prefix/text. The host can filter on `!L` to separate logs
  from data/control (`!P`, `?M`, etc.).
- Defaults: 192-byte line buffer, mirrors to stdout for local debugging.
  Tunables: `BR_LOG_SLIP_BUF_LEN`, `BR_LOG_CONF_ECHO_STDOUT`.

If you build your own project, include `br-log-conf.h` after setting the
`BR_LOG_CONF_*` macros so `LOG_CONF_OUTPUT` is redirected before including
`sys/log.h`.

## RPL side-channel hooks

- Border-router builds can export RPL-classic events over the side channel
  (using `br_link_write` framing: SLIP default, COBS optional).
- Enable with `BR_RPL_HOOKS_ENABLE` set to `1` in `project-conf.h`. The module
  overrides weak hooks in RPL-classic to send `!R` messages for:
  - DAO route changes (storing) with target/sender suffix and lifetime.
  - DAO SR updates (non-storing) with target/sender/parent suffix.
  - Preferred-parent changes (detach/attach) with old/new suffix.
- Payloads are compact (`!R` + event code + 16-bit IPv6 suffixes) and share the
  same framing as logs.

Example project snippet:
```c
#define BR_CONF_LINK_FRAMING BR_LINK_FRAMING_COBS /* optional */
#define BR_LOG_CONF_ENABLE 1
#define BR_RPL_HOOKS_ENABLE 1
#include "services/rpl-border-router/embedded/br-log-conf.h"
#include "services/rpl-border-router/embedded/br-rpl-hooks.h"
```

## Side-channel dispatcher

Applications can tap side-channel traffic (commands/events) without changing
core behavior:

- Inbound: register a handler via `br_side_in_register(tag, handler)`; if the
  handler returns non-zero the frame is consumed and core handling is skipped.
- Outbound: register a hook via `br_side_out_register(hook)` to observe raw
  payloads (before SLIP/COBS framing), e.g., `!L`/`!R` frames.
- Tags use the first byte of the side-channel payload (e.g., `!P`, `?X`).

Include `br-side-channel.h` from your app to register handlers/hooks.

## COBS link option

The border-router link framing is selectable at build time:

- Default: SLIP framing (unchanged IPv6 behavior).
- Optional: COBS framing for all link traffic by defining
  `BR_CONF_LINK_FRAMING` to `BR_LINK_FRAMING_COBS` in your project config
  before including `br-log-conf.h`. This requires a COBS-aware host peer that
  reads COBS frames with a trailing 0x00 delimiter (logs remain `!L`-prefixed
  inside those frames).

Example project snippet (in `project-conf.h`), placed before including
`br-log-conf.h`:
```c
#define BR_CONF_LINK_FRAMING BR_LINK_FRAMING_COBS
#define BR_LOG_CONF_ENABLE 1
#include "services/rpl-border-router/embedded/br-log-conf.h"
```

When SLIP framing is selected, `slip-bridge.c` handles ingress/egress. When
COBS framing is selected, `cobs-bridge.c` is built instead to decode COBS
payloads carried over the serial link (still using the SLIP byte pipe by
default).
