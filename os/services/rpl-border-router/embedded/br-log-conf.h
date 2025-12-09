/*
 * Logging-over-SLIP configuration for the embedded RPL border router.
 *
 * This wires the Contiki-NG LOG_CONF_OUTPUT hook to a small helper that
 * frames log lines as SLIP control messages prefixed with "!L".
 */
#ifndef BR_LOG_CONF_H_
#define BR_LOG_CONF_H_

/* Enable/disable SLIP log redirection. Default off to avoid affecting
 * native builds unless explicitly requested by the project. */
#ifndef BR_LOG_CONF_SLIP_ENABLE
#define BR_LOG_CONF_SLIP_ENABLE 0
#endif

/* Size of the line buffer before flushing to SLIP (excluding "!L"). */
#ifndef BR_LOG_SLIP_BUF_LEN
#define BR_LOG_SLIP_BUF_LEN 192
#endif

/* Also mirror to stdout so existing terminal logging keeps working. */
#ifndef BR_LOG_CONF_ECHO_STDOUT
#define BR_LOG_CONF_ECHO_STDOUT 1
#endif

/* Custom output hook used by log.h */
void br_log_slip_output(const char *fmt, ...);

#if BR_LOG_CONF_SLIP_ENABLE
#define LOG_CONF_OUTPUT(...) br_log_slip_output(__VA_ARGS__)
#endif

#endif /* BR_LOG_CONF_H_ */
