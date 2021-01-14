#include "building.hpp"

const int Building::width = 6;
const int Building::height = 4;

Building::Building(int y, int x)
    : mY(y), mX(x)
{
}

void Building::drawIn(std::vector<std::string> &board) const {
    for (int i = mY; i < (mY + height); ++i) {
        for (int j = mX; j < (mX + width); ++j) {
            board[i][j] = '.';
        }
    }
    for (int i = (mY + 2); i < (mY + height); ++i) {
        for (int j = (mX + 2); j < (mX + width - 2); ++j) {
            board[i][j] = '&';
        }
    }
}
