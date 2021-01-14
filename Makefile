all: main

main: main.cpp
	g++ -Wall -Werror -g -std=c++11 main.cpp item.cpp map_segment.cpp curses_controller.cpp print_controller.cpp curses_view.cpp print_view.cpp game.cpp view.cpp building.cpp -o main -lcurses

clean: 
	rm -rf main
