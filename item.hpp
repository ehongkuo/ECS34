#ifndef ITEM_HPP
#define ITEM_HPP

/**
 * You shouldn't have any reason to modify this file.
 *
 * This is probably the least justifiable class in the entire assignment,
 * but I want you to practice with many classes.
 */

class Item
{
public:
    Item(int y, int x);

    int getY() const { return mY; }
    int getX() const { return mX; }

    static char ICON;

private:
    /**
     * Each item is one unit, so only need to store coordinate,
     * not size.
     */
    int mY;
    int mX;
};

#endif // ITEM_HPP
