#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "game.hpp"
#include "curses_view.hpp"
#include "curses_controller.hpp"
#include "print_view.hpp"
#include "print_controller.hpp"

const int Game::MIN_VIEW_HEIGHT = 15;
const int Game::MIN_VIEW_WIDTH = 15;
const char Game::HERO_ICON_LEFT = '<';
const char Game::HERO_ICON_RIGHT = '>';
const char Game::HERO_ICON_UP = '^';
const char Game::HERO_ICON_DOWN = 'v';

Game::Game(const std::string& filename, InterfaceType interfaceType)
        : mView{nullptr}
        , mController{nullptr}
        , mInterfaceType{interfaceType}
{
    size_t found = filename.find_last_of("/");
    path = filename.substr(0, found);
    std::ifstream file{filename};
    std::string line;
    std::string a, b;
    getline(file, line);
    std::stringstream ss(line);
    ss >> a >> b;

    if (mInterfaceType == InterfaceType::Print) {
        mView = new PrintView(stoi(a), stoi(b));
        mController = new PrintController();
    }
    else if (mInterfaceType == InterfaceType::Curses) {
        mView = new CursesView(stoi(a), stoi(b));
        mController = new CursesController();
    }

    getline(file, line);
    numLevels = stoi(line);
    while (getline(file, line)) {
        levels.push_back(line);
    }
}

Game::~Game()
{
    delete mView;
    delete mController;
}

void Game::connectPortals(std::string firstMapNum, std::string portal1,
                          std::string secondMapNum, std::string portal2) {
    std::vector<std::string> connection;
    connection.push_back(firstMapNum);
    connection.push_back(portal1);
    connection.push_back(secondMapNum);
    connection.push_back(portal2);
    connections.push_back(connection);
}

void Game::resetMap() {
    lines.clear();
    std::string row(mView->getWidth() + 2, '~');
    for (int i = 0; i < mView->getHeight() + 2; ++i) {
        lines.push_back(row);
    }
    for (int i = 0; i < mView->getWidth() + 2; ++i) {
        lines[0][i] = '*';
        lines[mView->getHeight() + 1][i] = '*';
    }
    for (int i = 0; i < mView->getHeight() + 2; ++i) {
        lines[i][0] = '*';
        lines[i][mView->getWidth() + 1] = '*';
    }
}

void Game::loadLevel(std::string filename) {
    maps.clear();
    connections.clear();
    std::string fullFilename;
    fullFilename.append(path);
    fullFilename.append("/");
    fullFilename.append(filename);
    std::ifstream file{fullFilename};
    std::string line;
    std::string a, b;

    getline(file, line);
    currentMap = stoi(line);

    getline(file, line);
    std::stringstream ss(line);
    ss >> a >> b;
    playerY = stoi(a);
    playerX = stoi(b);

    getline(file, line);
    if (line == "left") { orientation = HERO_ICON_LEFT; }
    else if (line == "right") { orientation = HERO_ICON_RIGHT; }
    else if (line == "up") { orientation = HERO_ICON_UP; }
    else if (line == "down") { orientation = HERO_ICON_DOWN; }

    while(getline(file, line)) {
        std::stringstream ss(line);
        std::string letter;
        ss >> letter;
        if (letter == "M") {
            std::string height, width;
            ss >> height >> width;
            MapSegment buf(stoi(height), stoi(width));
            maps.push_back(buf);
        }
        else if (letter == "B") {
            std::string mapIndex, y, x;
            ss >> mapIndex >> y >> x;
            maps[stoi(mapIndex)].addBuilding(stoi(y), stoi(x));
        }
        else if (letter == "I") {
            std::string mapIndex, y, x;
            ss >> mapIndex >> y >> x;
            maps[stoi(mapIndex)].addItem(stoi(y), stoi(x));
            numItems++;
        }
        else if (letter == "P") {
            std::string from, fromOrientation, to, toOrientation;
            ss >> from >> fromOrientation >> to >> toOrientation;
            maps[stoi(from)].addPortal(fromOrientation);
            maps[stoi(to)].addPortal(toOrientation);
            connectPortals(from, fromOrientation, to, toOrientation);
        }
        else if (letter == "N") {
            std::string moves;
            ss >> moves;
            numMoves = stoi(moves);
        }
    }
    resetMap();
    centerY = (mView->getHeight() + 1) / 2;
    centerX = (mView->getWidth() + 1) / 2;
    x1 = centerX - playerX;
    x2 = x1 + maps[currentMap].getWidth() - 1;
    y1 = centerY - playerY;
    y2 = y1 + maps[currentMap].getHeight() - 1;
}

bool Game::gameOver() {
    if (numMoves == 0) { return true; }
    return false;
}

bool Game::levelPassed() {
    if (numItems == 0) { return true; }
    return false;
}

void Game::draw() {
    std::vector<std::string> map = maps[currentMap].getAsLines();
    resetMap();
    for (int i = y1; i <= y2; ++i) {
        if (i > 0 && i <= mView->getHeight()) {
            for (int j = x1; j <= x2; ++j) {
                if (j > 0 && j <= mView->getWidth()) {
                    lines[i][j] = map[i - y1][j - x1];
                }
            }
        }
    }
    lines[centerY][centerX] = orientation;
}

void Game::printInfo() {
    if (mInterfaceType == InterfaceType::Curses) {
        clear();
        printw("Level: %d\nItems remaining: %d\nMoves remaining: %d\n", currentLevel, numItems, numMoves);
    }
    std::cout << "Level: " << currentLevel << std::endl;
    std::cout << "Items remaining: " << numItems << std::endl;
    std::cout << "Moves remaining: " << numMoves << std::endl;
}

void Game::update() {
    Command currentCommand = mController->getInput();
    if (currentCommand == Command::Quit) {
        if (mInterfaceType == InterfaceType::Curses) {
            clear();
            mvaddstr(0, 0, "You quit the game.\nPress any key to exit.");
            getch();
            clear();
            endwin();
        }
        std::cout << "You quit the game." << std::endl;
        exit(0);
    }
    else if (currentCommand == Command::Left) {
        if (orientation == HERO_ICON_LEFT) { orientation = HERO_ICON_DOWN; }
        else if (orientation == HERO_ICON_DOWN) { orientation = HERO_ICON_RIGHT; }
        else if (orientation == HERO_ICON_RIGHT) { orientation = HERO_ICON_UP; }
        else if (orientation == HERO_ICON_UP) { orientation = HERO_ICON_LEFT; }
    }
    else if (currentCommand == Command::Right) {
        if (orientation == HERO_ICON_LEFT) { orientation = HERO_ICON_UP; }
        else if (orientation == HERO_ICON_UP) { orientation = HERO_ICON_RIGHT; }
        else if (orientation == HERO_ICON_RIGHT) { orientation = HERO_ICON_DOWN; }
        else if (orientation == HERO_ICON_DOWN) { orientation = HERO_ICON_LEFT; }
    }
    else if (currentCommand == Command::Forward) {
        if (orientation == HERO_ICON_LEFT) {
            if (lines[centerY][centerX - 1] == ' ') {
                x1++;
                x2++;
                numMoves--;
            }
            else if (lines[centerY][centerX - 1] == '$') {
                x1++;
                x2++;
                numItems--;
                numMoves--;
                maps[currentMap].removeItem(centerY - y1, centerX - x1);
            }
            else if (lines[centerY][centerX - 1] == '@') {
                numMoves--;
                for (unsigned i = 0; i < connections.size(); ++i) {
                    if (connections[i][1] == "left" && stoi(connections[i][0]) == currentMap) {
                        currentMap = stoi(connections[i][2]);
                        if (connections[i][3] == "left") {
                            orientation = HERO_ICON_RIGHT;
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "right") {
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "up") {
                            orientation = HERO_ICON_DOWN;
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][3] == "down") {
                            orientation = HERO_ICON_UP;
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                    else if (connections[i][3] == "left" && stoi(connections[i][2]) == currentMap) {
                        currentMap = stoi(connections[i][0]);
                        if (connections[i][1] == "left") {
                            orientation = HERO_ICON_RIGHT;
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "right") {
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "up") {
                            orientation = HERO_ICON_DOWN;
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][1] == "down") {
                            orientation = HERO_ICON_UP;
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                }
            }
        }
        else if (orientation == HERO_ICON_RIGHT) {
            if (lines[centerY][centerX + 1] == ' ') {
                x1--;
                x2--;
                numMoves--;
            }
            else if (lines[centerY][centerX + 1] == '$') {
                x1--;
                x2--;
                numItems--;
                numMoves--;
                maps[currentMap].removeItem(centerY - y1, centerX - x1);
            }
            else if (lines[centerY][centerX + 1] == '@') {
                numMoves--;
                for (unsigned i = 0; i < connections.size(); ++i) {
                    if (connections[i][1] == "right" && stoi(connections[i][0]) == currentMap) {
                        currentMap = stoi(connections[i][2]);
                        if (connections[i][3] == "left") {
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "right") {
                            orientation = HERO_ICON_LEFT;
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "up") {
                            orientation = HERO_ICON_DOWN;
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][3] == "down") {
                            orientation = HERO_ICON_UP;
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                    else if (connections[i][3] == "right" && stoi(connections[i][2]) == currentMap) {
                        currentMap = stoi(connections[i][0]);
                        if (connections[i][1] == "left") {
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "right") {
                            orientation = HERO_ICON_LEFT;
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "up") {
                            orientation = HERO_ICON_DOWN;
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][1] == "down") {
                            orientation = HERO_ICON_UP;
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                }
            }
        }
        else if (orientation == HERO_ICON_UP) {
            if (lines[centerY - 1][centerX] == ' ') {
                y1++;
                y2++;
                numMoves--;
            }
            else if (lines[centerY - 1][centerX] == '$') {
                y1++;
                y2++;
                numItems--;
                numMoves--;
                maps[currentMap].removeItem(centerY - y1, centerX - x1);
            }
            else if (lines[centerY - 1][centerX] == '@') {
                numMoves--;
                for (unsigned i = 0; i < connections.size(); ++i) {
                    if (connections[i][1] == "up" && stoi(connections[i][0]) == currentMap) {
                        currentMap = stoi(connections[i][2]);
                        if (connections[i][3] == "left") {
                            orientation = HERO_ICON_RIGHT;
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "right") {
                            orientation = HERO_ICON_LEFT;
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "up") {
                            orientation = HERO_ICON_DOWN;
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][3] == "down") {
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                    else if (connections[i][3] == "up" && stoi(connections[i][2]) == currentMap) {
                        currentMap = stoi(connections[i][0]);
                        if (connections[i][1] == "left") {
                            orientation = HERO_ICON_RIGHT;
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "right") {
                            orientation = HERO_ICON_LEFT;
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "up") {
                            orientation = HERO_ICON_DOWN;
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][1] == "down") {
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                }
            }
        }
        else if (orientation == HERO_ICON_DOWN) {
            if (lines[centerY + 1][centerX] == ' ') {
                y1--;
                y2--;
                numMoves--;
            }
            else if (lines[centerY + 1][centerX] == '$') {
                y1--;
                y2--;
                numItems--;
                numMoves--;
                maps[currentMap].removeItem(centerY - y1, centerX - x1);
            }
            else if (lines[centerY + 1][centerX] == '@') {
                numMoves--;
                for (unsigned i = 0; i < connections.size(); ++i) {
                    if (connections[i][1] == "down" && stoi(connections[i][0]) == currentMap) {
                        currentMap = stoi(connections[i][2]);
                        if (connections[i][3] == "left") {
                            orientation = HERO_ICON_RIGHT;
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "right") {
                            orientation = HERO_ICON_LEFT;
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][3] == "up") {
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][3] == "down") {
                            orientation = HERO_ICON_UP;
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                    else if (connections[i][3] == "down" && stoi(connections[i][2]) == currentMap) {
                        currentMap = stoi(connections[i][0]);
                        if (connections[i][1] == "left") {
                            orientation = HERO_ICON_RIGHT;
                            x1 = centerX - 1;
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "right") {
                            orientation = HERO_ICON_LEFT;
                            x2 = centerX + 1;
                            x1 = x2 - maps[currentMap].getWidth() + 1;
                            y1 = centerY - maps[currentMap].getPortalY();
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            break;
                        }
                        else if (connections[i][1] == "up") {
                            y1 = centerY - 1;
                            y2 = y1 + maps[currentMap].getHeight() - 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                        else if (connections[i][1] == "down") {
                            orientation = HERO_ICON_UP;
                            y2 = centerY + 1;
                            y1 = y2 - maps[currentMap].getHeight() + 1;
                            x1 = centerX - maps[currentMap].getPortalX();
                            x2 = x1 + maps[currentMap].getWidth() - 1;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Game::run() {
    for (int i = 0; i < numLevels; ++i) {
        currentLevel = i + 1;
        loadLevel(levels[i]);
        while (!levelPassed()) {
            if (gameOver()) {
                if (mInterfaceType == InterfaceType::Curses) {
                    clear();
                    mvaddstr(0, 0, "You lost the game.\nPress any key to exit.");
                    getch();
                    clear();
                    endwin();
                    exit(0);
                }
                else {
                    std::cout << "You lost the game." << std::endl;
                    exit(0);
                }
            }
            draw();
            printInfo();
            mView->draw(lines);
            update();
        }
    }
    if (mInterfaceType == InterfaceType::Curses) {
        clear();
        mvaddstr(0, 0, "You won the game.\nPress any key to exit.");
        getch();
        clear();
        endwin();
    }
    else {
        draw();
        printInfo();
        mView->draw(lines);
        std::cout << "You won the game." << std::endl;
    }
}