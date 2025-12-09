/*
 * SLIP log output helper for the embedded RPL border router.
 *
 * Collects log output into a line buffer, then emits it as a SLIP frame
 * prefixed with "!L" so the host can distinguish log traffic from data.
 */

#include "dev/slip.h"
#include "os/services/rpl-border-router/embedded/br-log-conf.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Simple line buffer; flushed when we see '\n' or the buffer is full. */
static char line_buf[BR_LOG_SLIP_BUF_LEN];
static size_t line_len;

static void
flush_line(void)
{
  if(line_len == 0) {
    return;
  }

  /* Prefix with "!L" to tag frames as logs. */
  uint8_t frame[2 + BR_LOG_SLIP_BUF_LEN];
  frame[0] = '!';
  frame[1] = 'L';

  memcpy(&frame[2], line_buf, line_len);
  slip_write(frame, line_len + 2);

  line_len = 0;
}

void
br_log_slip_output(const char *fmt, ...)
{
  char scratch[BR_LOG_SLIP_BUF_LEN];
  va_list ap;

  va_start(ap, fmt);
  int written = vsnprintf(scratch, sizeof(scratch), fmt, ap);
  va_end(ap);

  if(written < 0) {
    return;
  }

  size_t used = (written < (int)sizeof(scratch)) ? (size_t)written : (sizeof(scratch) - 1);

  /* Accumulate into the line buffer and flush on newline or overflow. */
  for(size_t i = 0; i < used; i++) {
    char c = scratch[i];
    if(c == '\n') {
      flush_line();
    } else {
      if(line_len < sizeof(line_buf)) {
        line_buf[line_len++] = c;
      }
      if(line_len == sizeof(line_buf)) {
        flush_line();
      }
    }
  }

#if BR_LOG_CONF_ECHO_STDOUT
  /* Keep the original stdout behavior for local debugging. */
  fwrite(scratch, 1, used, stdout);
#endif
}
