include_directories(BEFORE ../libna ../libbaas ../libparser)

add_executable(aparser aparser.c)
target_link_libraries(aparser parser na readline)
set_target_properties(aparser PROPERTIES COMPILE_FLAGS "-DNDEBUG -O2")

install(TARGETS aparser RUNTIME DESTINATION bin)
