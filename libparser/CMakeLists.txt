add_library(parser SHARED
  scanner.c
  lexer.c lexer-ids.c lexer-operators.c
  parser.c semanter.c functions.c
)

install(TARGETS parser LIBRARY DESTINATION lib)
install(FILES
  parser/scanner.h
  parser/lexer.h
  parser/parser.h
DESTINATION include/parser)
