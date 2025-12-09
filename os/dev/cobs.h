/*
 * Consistent Overhead Byte Stuffing (COBS) utilities.
 *
 * Provides simple encode/decode helpers for framing data over a byte stream.
 */
#ifndef COBS_H_
#define COBS_H_

#include <stddef.h>
#include <stdint.h>

/* Maximum extra bytes added by COBS for a payload of length 'len' is
 * len/254 + 1. Callers must provide enough space in the output buffer. */
size_t cobs_encode(const uint8_t *input, size_t length, uint8_t *output, size_t output_size);

/* Returns decoded length, or 0 on error (e.g., malformed input or short buffer). */
size_t cobs_decode(const uint8_t *input, size_t length, uint8_t *output, size_t output_size);

#endif /* COBS_H_ */
