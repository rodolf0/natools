add_library(parser SHARED
	scanner.c
	lexer.c lexer-ids.c lexer-operators.c
	parser.c semanter.c functions.c
)
