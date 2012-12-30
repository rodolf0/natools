add_executable(aparser
  aparser.c
)

target_link_libraries(aparser
  parser baas na
  readline m
)

install(TARGETS aparser RUNTIME DESTINATION bin)
