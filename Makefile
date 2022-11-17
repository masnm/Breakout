CC=g++
cflags=-O2 -std=c++20 -Wshadow -Wall -pedantic -lm
dbg=-fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG -g -DLOCAL
glut=-lGL -lGLU -lglut
objs=

bin/game.out: src/main.cpp $(objs)
	$(CC) $(cflags) $(objs) src/main.cpp -o bin/game.out $(glut)

run: ./bin/game.out
	./bin/game.out
