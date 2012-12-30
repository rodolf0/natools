add_library(na SHARED
  function.c
  combinatorics.c
  calculus.c
  root_finding.c
  interpolation.c
  natools.c
)

install(TARGETS na LIBRARY DESTINATION lib)
install(FILES
  na/function.h
  na/combinatorics.h
  na/calculus.h
  na/natools.h
  na/root_finding.h
  na/interpolation.h
DESTINATION include/na)
