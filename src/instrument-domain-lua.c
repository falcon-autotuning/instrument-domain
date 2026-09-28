#include <lauxlib.h>
#include <lua.h>

#include "instrument-domain/instrument-domain-lua.h"

/* =========================
   Lua wrapper struct
   ========================= */

typedef struct {
  InstrumentDomain *stack;
  int owned;
} lua_instrumentdomain;

/* =========================
   Helper
   ========================= */

static lua_instrumentdomain *check_instrumentdomain(lua_State *L) {
  return (lua_instrumentdomain *)luaL_checkudata(L, 1, "InstrumentDomain");
}

/* =========================
   Methods
   ========================= */

static int l_get_min(lua_State *L) {
  lua_instrumentdomain *cs = check_instrumentdomain(L);
  lua_pushnumber(L, instrument_domain_get_min(cs->stack));
  return 1;
}

static int l_get_max(lua_State *L) {
  lua_instrumentdomain *cs = check_instrumentdomain(L);
  lua_pushnumber(L, instrument_domain_get_max(cs->stack));
  return 1;
}

static int l_instrumentdomain_new(lua_State *L) {
  if (!lua_istable(L, 1)) {
    return luaL_error(L, "instrument_domain.new expects a table");
  }

  lua_getfield(L, 1, "min");
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    return luaL_error(L, "min is required");
  }
  double min = (double)luaL_checknumber(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 1, "max");
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    return luaL_error(L, "max is required");
  }
  double max = (double)luaL_checknumber(L, -1);
  lua_pop(L, 1);

  InstrumentDomain *cs = instrument_domain_create(min, max);

  if (!cs) {
    return luaL_error(L, "Failed to create InstrumentDomain");
  }

  push_domain(L, cs, 1);
  return 1;
}

static int l_instrumentdomain_clone(lua_State *L) {
  lua_instrumentdomain *cs = check_instrumentdomain(L);

  InstrumentDomain *copy =
      instrument_domain_create(instrument_domain_get_min(cs->stack),
                               instrument_domain_get_max(cs->stack));

  if (!copy) {
    return luaL_error(L, "Failed to clone InstrumentDomain");
  }

  push_domain(L, copy, 1); // new Lua-owned object
  return 1;
}

static int l_instrumentdomain_tostring(lua_State *L) {
  lua_instrumentdomain *cs = check_instrumentdomain(L);

  lua_pushfstring(L, "InstrumentDomain(%f,%f)",
                  instrument_domain_get_min(cs->stack),
                  instrument_domain_get_max(cs->stack));

  return 1;
}
static int l_instrumentdomain_to_string(lua_State *L) {
  return l_instrumentdomain_tostring(L);
}

/* =========================
   GC
   ========================= */

static int l_instrumentdomain_gc(lua_State *L) {
  lua_instrumentdomain *cs = check_instrumentdomain(L);

  if (cs->owned && cs->stack) {
    instrument_domain_free(cs->stack);
    cs->stack = NULL;
  }

  return 0;
}

/* =========================
   Methods table
   ========================= */

static const luaL_Reg instrumentdomain_methods[] = {
    {"get_min", l_get_min},
    {"get_max", l_get_max},
    {"clone", l_instrumentdomain_clone},
    {"to_string", l_instrumentdomain_to_string},
    {NULL, NULL}};

/* =========================
   Public API
   ========================= */

void push_domain(lua_State *L, InstrumentDomain *stack, int owned) {
  lua_instrumentdomain *cs =
      (lua_instrumentdomain *)lua_newuserdata(L, sizeof(lua_instrumentdomain));

  cs->stack = stack;
  cs->owned = owned;

  luaL_getmetatable(L, "InstrumentDomain");
  lua_setmetatable(L, -2);
}

void push_domain_global(lua_State *L, InstrumentDomain *stack, int owned,
                        const char *name) {
  push_domain(L, stack, owned);
  lua_setglobal(L, name);
}

void register_instrument_domain(lua_State *L) {
  luaopen_instrument_domain(L);
  lua_setglobal(L, "instrument_domain");
}

/* =========================
   Module Init
   ========================= */

static const luaL_Reg module_funcs[] = {{"new", l_instrumentdomain_new},
                                        {NULL, NULL}};

int luaopen_instrument_domain(lua_State *L) {
  luaL_newmetatable(L, "InstrumentDomain");

  lua_pushcfunction(L, l_instrumentdomain_tostring);
  lua_setfield(L, -2, "__tostring");

  lua_pushcfunction(L, l_instrumentdomain_gc);
  lua_setfield(L, -2, "__gc");

  lua_newtable(L);
  luaL_setfuncs(L, instrumentdomain_methods, 0);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);

  lua_newtable(L);
  luaL_setfuncs(L, module_funcs, 0);

  return 1;
}

InstrumentDomain *lua_check_domain(lua_State *L, int index) {
  lua_instrumentdomain *cs =
      (lua_instrumentdomain *)luaL_testudata(L, index, "InstrumentDomain");

  if (!cs || !cs->stack) {
    return NULL;
  }

  return cs->stack;
}
