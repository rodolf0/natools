include_directories(BEFORE ../libbaas)

add_library(parser SHARED
  scanner.c
  lexer.c lexer-ids.c lexer-operators.c
  parser.c semanter.c functions.c
)
target_link_libraries(parser baas m)
# let the parser complain on errors
set_target_properties(parser PROPERTIES COMPILE_FLAGS "-DNDEBUG -O2 -D_VERBOSE_")

install(TARGETS parser LIBRARY DESTINATION lib)
install(FILES
  parser/scanner.h
  parser/lexer.h
  parser/parser.h
DESTINATION include/parser)


######### testing #########
enable_testing()
include_directories(BEFORE .)

add_executable(test_scanner test/test_scanner.c)
target_link_libraries(test_scanner parser)

add_executable(test_lexer test/test_lexer.c)
target_link_libraries(test_lexer parser)

add_executable(test_parser test/test_parser.c)
target_link_libraries(test_parser parser)

set_target_properties(
  test_scanner
  test_lexer
  test_parser
  PROPERTIES COMPILE_FLAGS "-DDEBUG -ggdb -O0 -D_VERBOSE_")


add_test(NAME scanner
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
         COMMAND test_scanner)
add_test(lexer test_lexer)
add_test(parser test_parser)
