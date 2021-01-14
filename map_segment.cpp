#include "map_segment.hpp"

char MapSegment::VERTICAL_BORDER_CHAR = '|';
char MapSegment::HORIZONTAL_BORDER_CHAR = '=';
char MapSegment::PORTAL_CHAR = '@';

MapSegment::MapSegment(int height, int width)
    : mHeight(height), mWidth(width)
    , mPortalY{(mHeight - 1) / 2}
    , mPortalX{(mWidth - 1) / 2}
{
}

void MapSegment::addBuilding(int y, int x) {
    Building buf(y, x);
    buildings.push_back(buf);
}

void MapSegment::addItem(int y, int x) {
    Item buf(y, x);
    items.push_back(buf);
}

void MapSegment::removeItem(int y, int x) {
    for (unsigned i = 0; i < items.size(); ++i) {
        if (items[i].getY() == y && items[i].getX() == x) {
            items.erase(items.begin() + i);
        }
    }
}

void MapSegment::addPortal(std::string orientation) {
    if (orientation.front() == 'l') {
        Portal newPortal = Portal::Left;
        portals.push_back(newPortal);
    }
    else if (orientation.front() == 'r') {
        Portal newPortal = Portal::Right;
        portals.push_back(newPortal);
    }
    else if (orientation.front() == 'u') {
        Portal newPortal = Portal::Up;
        portals.push_back(newPortal);
    }
    else if (orientation.front() == 'd') {
        Portal newPortal = Portal::Down;
        portals.push_back(newPortal);
    }
}

std::vector<std::string> MapSegment::getAsLines() const {
    std::string line(mWidth, ' ');
    std::vector<std::string> map;
    for (int i = 0; i < mHeight; ++i) {
        map.push_back(line);
    }
    for (int i = 0; i < mHeight; ++i) {
        map[i][0] = VERTICAL_BORDER_CHAR;
        map[i][mWidth - 1] = VERTICAL_BORDER_CHAR;
    }
    for (int i = 0; i < mWidth; ++i) {
        map[0][i] = HORIZONTAL_BORDER_CHAR;
        map[mHeight - 1][i] = HORIZONTAL_BORDER_CHAR;
    }
    for (Building b : buildings) {
        b.drawIn(map);
    }
    for (Item i : items) {
        map[i.getY()][i.getX()] = Item::ICON;
    }
    for (Portal p : portals) {
        if (p == Portal::Left) { map[mPortalY][0] = PORTAL_CHAR; }
        else if (p == Portal::Right) { map[mPortalY][mWidth - 1] = PORTAL_CHAR; }
        else if (p == Portal::Up) { map[0][mPortalX] = PORTAL_CHAR; }
        else if (p == Portal::Down) { map[mHeight - 1][mPortalX] = PORTAL_CHAR; }
    }
    return map;
}
