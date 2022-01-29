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
 *
 * This file defines the functions crc_16_dnp_bbf_init(),
 * crc_16_dnp_bbf_update() and crc_16_dnp_bbf_finalize().
 *
 * The crc_16_dnp_bbf_init() function returns the inital \c crc value and must
 * be called before the first call to crc_16_dnp_bbf_update(). Similarly, the
 * crc_16_dnp_bbf_finalize() function must be called after the last call to
 * crc_16_dnp_bbf_update(), before the \c crc is being used. is being used.
 *
 * The crc_16_dnp_bbf_update() function can be called any number of times
 * (including zero times) in between the crc_16_dnp_bbf_init() and
 * crc_16_dnp_bbf_finalize() calls.
 *
 * This pseudo-code shows an example usage of the API:
 * \code{.c}
 * crc_16_dnp_bbf_t crc;
 * unsigned char data[MAX_DATA_LEN];
 * size_t data_len;
 *
 * crc = crc_16_dnp_bbf_init();
 * while ((data_len = read_data(data, MAX_DATA_LEN)) > 0) {
 *     crc = crc_16_dnp_bbf_update(crc, data, data_len);
 * }
 * crc = crc_16_dnp_bbf_finalize(crc);
 * \endcode
 */
#ifndef CRC_16_DNP_BBF_H
#define CRC_16_DNP_BBF_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The definition of the used algorithm.
 *
 * This is not used anywhere in the generated code, but it may be used by the
 * application code to call algorithm-specific code, if desired.
 */
#define CRC_ALGO_BIT_BY_BIT_FAST 1

/**
 * The type of the CRC values.
 *
 * This type must be big enough to contain at least 16 bits.
 */
typedef uint_fast16_t crc_16_dnp_bbf_t;

/**
 * Reflect all bits of a \a data word of \a data_len bytes.
 *
 * \param[in] data     The data word to be reflected.
 * \param[in] data_len The width of \a data expressed in number of bits.
 * \return             The reflected data.
 */
crc_16_dnp_bbf_t crc_16_dnp_bbf_reflect(crc_16_dnp_bbf_t data, size_t data_len);

/**
 * Calculate the initial crc value.
 *
 * \return     The initial crc value.
 */
static inline crc_16_dnp_bbf_t crc_16_dnp_bbf_init(void) { return 0x0000; }

/**
 * Update the crc value with new data.
 *
 * \param[in] crc      The current crc value.
 * \param[in] data     Pointer to a buffer of \a data_len bytes.
 * \param[in] data_len Number of bytes in the \a data buffer.
 * \return             The updated crc value.
 */
crc_16_dnp_bbf_t crc_16_dnp_bbf_update(crc_16_dnp_bbf_t crc, const void *data,
                                       size_t data_len);

/**
 * Calculate the final crc value.
 *
 * \param[in] crc  The current crc value.
 * \return     The final crc value.
 */
static inline crc_16_dnp_bbf_t crc_16_dnp_bbf_finalize(crc_16_dnp_bbf_t crc) {
  return crc_16_dnp_bbf_reflect(crc, 16) ^ 0xffff;
}

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* CRC_16_DNP_BBF_H */