/*
 * Consistent Overhead Byte Stuffing (COBS) utilities.
 *
 * Minimal encode/decode helpers suitable for framing over a byte stream.
 */

#include "dev/cobs.h"

size_t
cobs_encode(const uint8_t *input, size_t length, uint8_t *output, size_t output_size)
{
  if(input == NULL || output == NULL) {
    return 0;
  }

  /* Worst-case expansion is length/254 + 1 bytes. */
  if(output_size < length + length / 254 + 1) {
    return 0;
  }

  const uint8_t *end = input + length;
  uint8_t *out = output;
  uint8_t *code_ptr = out++; /* Reserve first byte for the code. */
  uint8_t code = 1;

  while(input < end) {
    if(*input == 0) {
      *code_ptr = code;
      code_ptr = out++;
      code = 1;
      input++;
    } else {
      *out++ = *input++;
      code++;
      if(code == 0xFF) {
        *code_ptr = code;
        code_ptr = out++;
        code = 1;
      }
    }
  }

  *code_ptr = code;
  return (size_t)(out - output);
}

size_t
cobs_decode(const uint8_t *input, size_t length, uint8_t *output, size_t output_size)
{
  if(input == NULL || output == NULL) {
    return 0;
  }

  const uint8_t *in_end = input + length;
  uint8_t *out_start = output;
  uint8_t *out_end = output + output_size;

  while(input < in_end) {
    uint8_t code = *input++;
    if(code == 0 || output + code - 1 > out_end || input + code - 1 > in_end) {
      return 0;
    }
    for(uint8_t i = 1; i < code; i++) {
      *output++ = *input++;
    }
    if(code != 0xFF && input < in_end) {
      *output++ = 0;
    }
  }

  return (size_t)(output - out_start);
}
