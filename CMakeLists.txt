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


###### build a CPack driven installer package
include (InstallRequiredSystemLibraries)
#set (CPACK_RESOURCE_FILE_LICENSE  
     #"${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${natools_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${natools_VERSION_MINOR}")
include (CPack)
