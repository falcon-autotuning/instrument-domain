#include <cmocka.h>
#include <complex.h>
#include <stdarg.h>
#include <stddef.h>

#include <lauxlib.h>
#include <lualib.h>

#include "instrument-domain/instrument-domain-lua.h"
#include "instrument-domain/instrument-domain.h"

/* =========================
   Helpers
   ========================= */

static lua_State *create_lua(void) {
  lua_State *L = luaL_newstate();
  assert_non_null(L);

  luaL_openlibs(L);

  luaopen_instrument_domain(L);
  lua_pop(L, 1);

  return L;
}

/* =========================
   Tests
   ========================= */

static void test_lua_getters_basic(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *cs = instrument_domain_create(-1.0, 1.0);

  push_domain(L, cs, 0);
  lua_setglobal(L, "domain");

  assert_int_equal(luaL_dostring(L, "assert(domain:get_min() == -1.0)\n"
                                    "assert(domain:get_max() == 1.0)\n"),
                   LUA_OK);

  instrument_domain_free(cs);
  lua_close(L);
}

static void test_push_domain_global(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *cs = instrument_domain_create(-1.0, 1.0);

  push_domain_global(L, cs, 0, "global_domain");

  assert_int_equal(luaL_dostring(L, "assert(global_domain:get_min() == -1.0)"),
                   LUA_OK);

  instrument_domain_free(cs);
  lua_close(L);
}

static void test_register_module(void **state) {
  (void)state;

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  register_instrument_domain(L);

  assert_int_equal(luaL_dostring(L, "assert(instrument_domain ~= nil)"),
                   LUA_OK);

  lua_close(L);
}

static void test_lua_owned_gc(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *cs = instrument_domain_create(-1.0, 1.0);

  push_domain(L, cs, 1);
  lua_setglobal(L, "domain");

  luaL_dostring(L, "domain=nil; collectgarbage()");

  lua_close(L);
}

static void test_lua_not_owned_gc(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *cs = instrument_domain_create(-1.0, 1.0);

  push_domain(L, cs, 0);
  lua_setglobal(L, "domain");

  luaL_dostring(L, "domain=nil; collectgarbage()");

  assert_non_null(cs);

  instrument_domain_free(cs);
  lua_close(L);
}

static void test_lua_to_string_method(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *cs = instrument_domain_create(-1.0, 1.0);

  push_domain(L, cs, 0);
  lua_setglobal(L, "domain");

  assert_int_equal(luaL_dostring(L, "local s = domain:to_string()\n"
                                    "assert(type(s) == 'string')\n"
                                    "assert(s:match('InstrumentDomain%('))\n"
                                    "assert(string.find(s, -1))\n"
                                    "assert(string.find(s, 1))\n"),
                   LUA_OK);

  instrument_domain_free(cs);
  lua_close(L);
}

static void test_lua_tostring_metamethod(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *cs = instrument_domain_create(-1.0, 1.0);

  push_domain(L, cs, 0);
  lua_setglobal(L, "domain");

  assert_int_equal(luaL_dostring(L, "local s = tostring(domain)\n"
                                    "assert(type(s) == 'string')\n"
                                    "assert(s:match('InstrumentDomain%('))\n"
                                    "assert(string.find(s, -1))\n"
                                    "assert(string.find(s, 1))\n"),
                   LUA_OK);

  instrument_domain_free(cs);
  lua_close(L);
}

static void test_lua_constructor_basic(void **state) {
  (void)state;

  lua_State *L = create_lua();

  register_instrument_domain(L);

  assert_int_equal(luaL_dostring(L, "local cs = instrument_domain.new{\n"
                                    "  min = -1.0,\n"
                                    "  max = 1.0,\n"
                                    "}\n"
                                    "assert(cs:get_min() == -1.0)\n"
                                    "assert(cs:get_max() == 1.0)\n"),
                   LUA_OK);

  lua_close(L);
}

static void test_lua_clone_basic(void **state) {
  (void)state;

  lua_State *L = create_lua();

  register_instrument_domain(L);

  assert_int_equal(luaL_dostring(L, "local cs1 = instrument_domain.new{\n"
                                    "  min = -1.0,\n"
                                    "  max = 1.0,\n"
                                    "}\n"
                                    "local cs2 = cs1:clone()\n"
                                    "assert(cs2:get_min() == -1.0)\n"
                                    "assert(cs2:get_max() == 1.0)\n"),
                   LUA_OK);

  lua_close(L);
}

static void test_lua_clone_independence(void **state) {
  (void)state;

  lua_State *L = create_lua();

  register_instrument_domain(L);

  assert_int_equal(luaL_dostring(L, "local cs1 = instrument_domain.new{\n"
                                    "  min = -1.0,\n"
                                    "  max = 1.0,\n"
                                    "}\n"
                                    "local cs2 = cs1:clone()\n"
                                    "assert(cs1 ~= cs2)\n"),
                   LUA_OK);

  lua_close(L);
}

static void test_lua_clone_gc_owned(void **state) {
  (void)state;

  lua_State *L = create_lua();

  register_instrument_domain(L);

  assert_int_equal(luaL_dostring(L, "local cs1 = instrument_domain.new{\n"
                                    "  min = -1.0,\n"
                                    "  max = 1.0,\n"
                                    "}\n"
                                    "local cs2 = cs1:clone()\n"
                                    "cs1 = nil\n"
                                    "cs2 = nil\n"
                                    "collectgarbage()\n"),
                   LUA_OK);

  lua_close(L);
}

static void test_lua_constructor_missing_required(void **state) {
  (void)state;

  lua_State *L = create_lua();
  register_instrument_domain(L);

  assert_true(luaL_dostring(L, "local ok, err = pcall(function()\n"
                               "  instrument_domain.new{}\n"
                               "end)\n"
                               "assert(ok == false)\n") == LUA_OK);

  lua_close(L);
}

static void test_lua_check_domain_valid(void **state) {
  (void)state;

  lua_State *L = create_lua();

  InstrumentDomain *original = instrument_domain_create(-1.0, 1.0);

  push_domain(L, original, 0);

  InstrumentDomain *extracted = lua_check_domain(L, -1);

  assert_non_null(extracted);

  instrument_domain_free(original);
  lua_close(L);
}

static void test_lua_check_domain_non_userdata(void **state) {
  (void)state;

  lua_State *L = create_lua();

  lua_pushstring(L, "not a instrumentdomain");

  InstrumentDomain *extracted = lua_check_domain(L, -1);

  assert_null(extracted);

  lua_close(L);
}

static void test_lua_check_domain_wrong_metatable(void **state) {
  (void)state;

  lua_State *L = create_lua();

  // Create unrelated userdata
  void *ud = lua_newuserdata(L, sizeof(int));
  (void)ud;

  luaL_newmetatable(L, "NotInstrumentdomain");
  lua_setmetatable(L, -2);

  InstrumentDomain *extracted = lua_check_domain(L, -1);

  assert_null(extracted);

  lua_close(L);
}

static void test_lua_check_domain_null_internal(void **state) {
  (void)state;

  lua_State *L = create_lua();

  // Manually push userdata with NULL stack
  typedef struct {
    InstrumentDomain *stack;
    int owned;
  } lua_instrumentdomain;

  lua_instrumentdomain *cs =
      (lua_instrumentdomain *)lua_newuserdata(L, sizeof(lua_instrumentdomain));

  cs->stack = NULL;
  cs->owned = 0;

  luaL_newmetatable(L, "Instrumentdomain");
  lua_setmetatable(L, -2);

  InstrumentDomain *extracted = lua_check_domain(L, -1);

  assert_null(extracted);

  lua_close(L);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_lua_getters_basic),
      cmocka_unit_test(test_push_domain_global),
      cmocka_unit_test(test_register_module),
      cmocka_unit_test(test_lua_owned_gc),
      cmocka_unit_test(test_lua_not_owned_gc),
      cmocka_unit_test(test_lua_to_string_method),
      cmocka_unit_test(test_lua_tostring_metamethod),
      cmocka_unit_test(test_lua_constructor_basic),
      cmocka_unit_test(test_lua_clone_basic),
      cmocka_unit_test(test_lua_clone_independence),
      cmocka_unit_test(test_lua_clone_gc_owned),
      cmocka_unit_test(test_lua_constructor_missing_required),
      cmocka_unit_test(test_lua_check_domain_valid),
      cmocka_unit_test(test_lua_check_domain_non_userdata),
      cmocka_unit_test(test_lua_check_domain_wrong_metatable),
      cmocka_unit_test(test_lua_check_domain_null_internal),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
