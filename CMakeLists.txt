cmake_minimum_required (VERSION 2.6)
project (natools C)
# The version number.
set (natools_VERSION_MAJOR 0)
set (natools_VERSION_MINOR 20)
 
add_definitions(
  -std=c11
  -g -O2
  -Wall -Wextra -Werror 
  -Wshadow
  -Wstrict-prototypes
  -Wpointer-arith
  -Wcast-qual
  -Winline
)

add_subdirectory(libbaas)
add_subdirectory(libparser)
add_subdirectory(libna)
add_subdirectory(tools)

enable_testing()
