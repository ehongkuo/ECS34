#include "game.hpp"
#include "interface_type.hpp"

int main(int argc, char *argv[])
{
    InterfaceType it = InterfaceType::Print;
    if (argc >= 2) it = InterfaceType::Curses;
    Game g{"game_files/game6.txt", it};
    g.run();
}
