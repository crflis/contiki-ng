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

## COBS link option

The border-router link framing is selectable at build time:

- Default: SLIP framing (unchanged IPv6 behavior).
- Optional: COBS framing for all link traffic by defining
  `BR_CONF_LINK_FRAMING` to `BR_LINK_FRAMING_COBS` in your project config
  before including `br-log-conf.h`. This requires a COBS-aware host peer.
