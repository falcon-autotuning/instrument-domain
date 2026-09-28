find_package(cmocka REQUIRED)

add_executable(test_instrument_domain
    test_instrument_domain.c
)

target_link_libraries(test_instrument_domain
    instrument-domain
    cmocka::cmocka
)

add_test(NAME instrument_domain_tests COMMAND test_instrument_domain)

if(BUILD_LUA)
  find_package(Lua REQUIRED)
  # Normalize Lua target across environments
  if(NOT TARGET Lua::Lua)
    add_library(Lua::Lua INTERFACE IMPORTED)
    set_target_properties(Lua::Lua PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${LUA_LIBRARIES}"
  )
  endif()

  add_executable(test_instrument_domain_lua
    test_instrument_domain_lua.c
  )

  target_link_libraries(test_instrument_domain_lua
    instrument-domain
    instrument-domain-lua
    Lua::Lua
    cmocka::cmocka
  )

  add_test(NAME instrument_domain_lua_tests COMMAND test_instrument_domain_lua)
endif()
