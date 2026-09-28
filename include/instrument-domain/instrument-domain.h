#pragma once

/** Maximum length (including null terminator) for each string field */
#define INST_DOMAIN_MAX_STRING_LEN 64
#define INST_DOMAIN_SERIALIZED_MAX_SIZE (2 * INST_DOMAIN_MAX_STRING_LEN)

#ifdef __cplusplus
extern "C" {
#endif

#include "instrument-domain/instrument-domain-export.h"

/**
 * @brief Opaque handle representing an instrument target.
 *
 * @details The internal structure is hidden from users of the API.
 * Instances must be created and destroyed using the provided API functions.
 */
typedef struct InstrumentDomain InstrumentDomain;

/**
 * @brief Creates a new instrument target instance.
 *
 * @param min The minimum of the domain
 * @pram max The maximum of the domain
 *
 * @return Pointer to a newly allocated InstrumentDomain instance, or NULL on
 * failure.
 */
INSTRUMENT_DOMAIN_EXPORT InstrumentDomain *instrument_domain_create(double min,
                                                                    double max);

/**
 * @brief Frees a InstrumentDomain instance.
 *
 * @param domain Pointer to the InstrumentDomain to free. Safe to pass NULL.
 */
INSTRUMENT_DOMAIN_EXPORT void instrument_domain_free(InstrumentDomain *domain);

/**
 * @brief Retrieves the min of the domain.
 */
INSTRUMENT_DOMAIN_EXPORT double
instrument_domain_get_min(const InstrumentDomain *domain);

/**
 * @brief Retrieves the max of the domain.
 */
INSTRUMENT_DOMAIN_EXPORT double
instrument_domain_get_max(const InstrumentDomain *domain);

/**
 * @brief Serializes the call domain into a portable representation.
 *
 * @details The returned buffer contains a self-contained representation of the
 * InstrumentDomain that can be transmitted or stored and later reconstructed
 * using instrument_domain_deserialize().
 *
 * @param domain InstrumentDomain instance.
 *
 * @return Pointer to a serialized buffer (null-terminated or binary-safe
 * depending on implementation).
 *
 * @note The returned memory is owned by the library and must not be modified.
 *       If dynamic allocation is used, a matching free API should also be
 * provided.
 */
INSTRUMENT_DOMAIN_EXPORT char *
instrument_domain_serialize(const InstrumentDomain *domain);

/**
 * @brief Deserializes a InstrumentDomain from a serialized representation.
 *
 * @param buffer Serialized data.
 *
 * @return Pointer to a newly allocated InstrumentDomain instance, or NULL on
 * failure.
 */
INSTRUMENT_DOMAIN_EXPORT InstrumentDomain *
instrument_domain_deserialize(const char *buffer);

/**
 * @brief Implements a deep copy of a InstrumentDomain instance.
 */
INSTRUMENT_DOMAIN_EXPORT InstrumentDomain *
instrument_domain_clone(const InstrumentDomain *domain);

#ifdef __cplusplus
}
#endif
