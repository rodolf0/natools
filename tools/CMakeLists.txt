include_directories(BEFORE ../libna ../libbaas ../libparser)

add_executable(aparser
  aparser.c
)
target_link_libraries(aparser parser na readline)

install(TARGETS aparser RUNTIME DESTINATION bin)
