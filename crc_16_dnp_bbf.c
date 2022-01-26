/**
 * \file
 * Functions and types for CRC checks.
 *
 * Generated on Mon Aug 24 06:27:49 2020
 * by pycrc v0.9.2, https://pycrc.org
 * using the configuration:
 *  - Width         = 16
 *  - Poly          = 0x3d65
 *  - XorIn         = 0x0000
 *  - ReflectIn     = True
 *  - XorOut        = 0xffff
 *  - ReflectOut    = True
 *  - Algorithm     = bit-by-bit-fast
 */
#include "crc_16_dnp_bbf.h" /* include the header file generated with pycrc */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

crc_16_dnp_bbf_t crc_16_dnp_bbf_reflect(crc_16_dnp_bbf_t data,
                                        size_t data_len) {
  unsigned int i;
  crc_16_dnp_bbf_t ret;

  ret = data & 0x01;
  for (i = 1; i < data_len; i++) {
    data >>= 1;
    ret = (ret << 1) | (data & 0x01);
  }
  return ret;
}

crc_16_dnp_bbf_t crc_16_dnp_bbf_update(crc_16_dnp_bbf_t crc, const void *data,
                                       size_t data_len) {
  const unsigned char *d = (const unsigned char *)data;
  unsigned int i;
  bool bit;
  unsigned char c;

  while (data_len--) {
    c = *d++;
    for (i = 0x01; i & 0xff; i <<= 1) {
      bit = crc & 0x8000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x3d65;
      }
    }
    crc &= 0xffff;
  }
  return crc & 0xffff;
}
