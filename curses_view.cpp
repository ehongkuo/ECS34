#include "curses_view.hpp"

/**
 * Side note: This class should probably be what is known as a singleton
 * class, i.e. a class that uses some setup (e.g. a static variable) to
 * prevent more than one instance of it from being created. I did not do this.
 */

CursesView::CursesView(int height, int width)
    : View(height, width)
{
    initscr();
    cbreak();
    noecho();
    clear();
}

CursesView::~CursesView()
{
    clear();
    endwin();
}

void CursesView::draw(const std::vector<std::string>& lines)
{
    for (unsigned i = 0; i < lines.size(); ++i) {
        const char* cStr = lines[i].c_str();
        mvaddstr(i + 3, 0, cStr);
    }
    refresh();
    curs_set(0);


}
