include_directories(BEFORE ../libbaas ../libparser)

add_library(na SHARED
  function.c
  combinatorics.c
  calculus.c
  root_finding.c
  interpolation.c
  natools.c
)
target_link_libraries(na parser)
set_target_properties(na PROPERTIES COMPILE_FLAGS "-DNDEBUG -O2")

install(TARGETS na LIBRARY DESTINATION lib)
install(FILES
  na/function.h
  na/combinatorics.h
  na/calculus.h
  na/natools.h
  na/root_finding.h
  na/interpolation.h
DESTINATION include/na)


######### testing #########
enable_testing()
include_directories(BEFORE .)

add_executable(test_calculus test/test_calculus.c)
target_link_libraries(test_calculus na)

add_executable(test_combinatronics test/test_combinatronics.c)
target_link_libraries(test_combinatronics na)

set_target_properties(
  test_calculus
  test_combinatronics
  PROPERTIES COMPILE_FLAGS "-DDEBUG -ggdb -O0")

add_test(calculus test_calculus)
add_test(combinatorics test_combinatronics)
