#ifndef MSGPACK_ENCODER_H
#define MSGPACK_ENCODER_H

#include "gobu_colyseus.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode a Gobu Variant to msgpack bytes.
 * 
 * Converts native Gobu types to msgpack-encoded data:
 * - Gobu null -> msgpack nil
 * - Gobu bool -> msgpack bool
 * - Gobu int -> msgpack int
 * - Gobu float -> msgpack float64
 * - Gobu String -> msgpack str
 * - Gobu PackedByteArray -> msgpack bin
 * - Gobu Array -> msgpack array
 * - Gobu Dictionary -> msgpack map
 * 
 * @param variant Pointer to the Variant to encode
 * @param out_length Pointer to store the resulting buffer length
 * @return Allocated buffer containing msgpack data (caller must free), or NULL on error
 * 
 * @note The caller is responsible for freeing the returned buffer
 * @note Returns NULL and sets out_length to 0 on encoding error
 */
uint8_t* gobu_variant_to_msgpack(const Variant* variant, size_t* out_length);

#ifdef __cplusplus
}
#endif

#endif /* MSGPACK_ENCODER_H */
