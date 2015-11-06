FLAGS = -Wall -Werror -ansi -pedantic

all: 
	rm -rf bin
	mkdir -p bin
	g++ $(FLAGS) ./src/rshell.cpp ./src/main.h ./src/connector.h -o ./bin/rshell

rshell:
	rm -rf bin
	mkdir -p bin
	g++ $(FLAGS) ./src/rshell.cpp ./src/main.h ./src/connector.h -o ./bin/rshell

clean:
	rm -rf bin
