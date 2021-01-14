#include "curses_controller.hpp"

#include <curses.h>

CursesController::~CursesController()
{
}

Command CursesController::getInput()
{
    char input;
    input = getch();
    if (input == 'a') { return Command::Left; }
    else if (input == 'd') { return Command::Right; }
    else if (input == 'w') { return Command::Forward; }
    else if (input == 27) {
        input = getch();
        input = getch();
        if (input == 'D') { return Command::Left; }
        else if (input == 'C') { return Command::Right; }
        else if (input == 'A') { return Command::Forward; }
    }
    else if (input == 'q') {
        return Command::Quit;
    }
    return Command::Invalid;
}
