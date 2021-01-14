#include "print_controller.hpp"

#include <iostream>

PrintController::~PrintController()
{
}

Command PrintController::getInput()
{
    char input;
    std::cout << "Enter input: ";
    std::cin >> input;
    if (input == 'a' || input == 'A') { return Command::Left; }
    else if (input == 'd' || input == 'D') { return Command::Right; }
    else if (input == 'w' || input == 'W') { return Command::Forward; }
    else if (input == 'q' || input == 'Q') { return Command::Quit; }
    else { return Command::Invalid; }
}
