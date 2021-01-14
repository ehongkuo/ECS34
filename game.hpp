#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <cmath>

#include "controller.hpp"
#include "view.hpp"
#include "interface_type.hpp"
#include "map_segment.hpp"

class Game
{
public:  // public methods
    /**
     * DO NOT change the prototype of this function.
     * The autograder WILL call this function.
     */
    Game(const std::string& filename, InterfaceType interfaceType);

    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    
    /**
     * DO NOT change the prototype of this function.
     * The autograder WILL call this function.
     */
    void run();

private:  // private methods
    /**
     * You DO NOT need to have any of the private methods below.
     * I just left some commented out declarations in case it
     * helps guide your thinking, but you don't need to use them.
     */

    /**
     * Set up the 1D vector of strings that is then given to the draw()
     * method of an instance of a subclass of View.
     */
    void draw();

    /**
     * Loop until the game is over. On each iteration, use the controller
     * to get input from the user.
     */
    // void doGameLoop();

    /**
     * Update matters related to game logic, e.g. collisions, level completion.
     */
    void update();

    /**
     * Load information related to new level into member variables,
     * being careful to not leave traces of information from previous levels. 
     */
    void loadLevel(std::string filename);
    void resetMap();
    void connectPortals(std::string firstMapNum, std::string portal1,
                        std::string secondMapNum, std::string portal2);
    void printInfo();
    bool gameOver();
    bool levelPassed();
    void show();

private:
    static const int MIN_VIEW_HEIGHT;
    static const int MIN_VIEW_WIDTH;
    static const char HERO_ICON_LEFT;
    static const char HERO_ICON_RIGHT;
    static const char HERO_ICON_UP;
    static const char HERO_ICON_DOWN;

    std::string path;

    int centerY;
    int centerX;

    int currentLevel;
    int currentMap;
    int playerY;
    int playerX;
    int x1, x2, y1, y2;
    char orientation;
    std::vector<MapSegment> maps;
    int numItems = 0;
    int numMoves;
    std::vector<std::vector<std::string>> connections;
    std::vector<std::string> lines;
    std::vector<std::string> levels;
    int numLevels;
    /**
     * Because one of the constructor parameters must be checked in order
     * to determine the subclass types for the below members, they can't
     * be references, because there's no default constructors for them.
     * (Recall that each member in a class must either be initialized by an
     * initializer list in the constructor OR have a default constructor.)
     */
    View* mView;
    Controller* mController;

    InterfaceType mInterfaceType;
};



#endif // GAME_HPP
