#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

#include "instrument-domain.h"
#include "instrument-domain/instrument-domain-export.h"

/**
 * @brief Push a InstrumentDomain into the Lua domain as userdata.
 *
 * @param L Lua state.
 * @param domain Pointer to an existing InstrumentDomain instance.
 * @param owned If non-zero, Lua takes ownership and will free it via GC.
 *
 * @note The userdata is pushed onto the Lua domain.
 *       The caller is responsible for assigning it (e.g. lua_setglobal).
 */
INSTRUMENT_DOMAIN_EXPORT void push_domain(lua_State *L,
                                          InstrumentDomain *domain, int owned);

/**
 * @brief Lua module initializer.
 *
 * @param L Lua state.
 * @return Number of values returned to Lua (module table).
 *
 * @note Allows usage via require("instrument_domain").
 */
INSTRUMENT_DOMAIN_EXPORT int luaopen_instrument_domain(lua_State *L);

/**
 * @brief Push a InstrumentDomain into Lua and assign it to a global variable.
 *
 * @param L Lua state.
 * @param domain InstrumentDomain pointer.
 * @param owned If non-zero, Lua owns lifetime.
 * @param name Name of the global variable.
 */
INSTRUMENT_DOMAIN_EXPORT void push_domain_global(lua_State *L,
                                                 InstrumentDomain *domain,
                                                 int owned, const char *name);

/**
 * @brief Register the InstrumentDomain module as a global Lua table.
 *
 * @param L Lua state.
 *
 * @note After calling this, Lua will have:
 *       instrument_domain = { ... }
 */
INSTRUMENT_DOMAIN_EXPORT void register_instrument_domain(lua_State *L);

/**
 * @brief Extract a InstrumentDomain pointer from a Lua userdata.
 *
 * @param L Lua state
 * @param index domain index
 * @return InstrumentDomain* or NULL if not valid
 */
INSTRUMENT_DOMAIN_EXPORT
InstrumentDomain *lua_check_domain(lua_State *L, int index);

#ifdef __cplusplus
}
#endif
