CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include

src/C_parser.tab.cpp src/C_parser.tab.hpp : src/C_parser.y
	bison -v -d src/C_parser.y -o src/C_parser.tab.cpp

src/C_lexer.yy.cpp : src/C_lexer.flex src/C_parser.tab.hpp
	flex -o src/C_lexer.yy.cpp  src/C_lexer.flex

bin/c_compiler : src/compiler.o src/C_parser.tab.o src/C_lexer.yy.o src/C_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^


clean :
	rm -f src/*.o
	rm -f bin/*
	rm -f src/*.tab.cpp
	rm -f src/*.yy.cpp
