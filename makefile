FLAGS = -g -Wall -Werror -ansi -pedantic

all: 
	rm -rfv bin
	mkdir -pv bin
	g++ $(FLAGS) ./src/*.cpp -o ./bin/rshell

rshell:
	rm -rfv bin
	mkdir -pv bin
	g++ $(FLAGS) ./src/*.cpp -o ./bin/rshell

clean:
	rm -rfv bin
