#include "print_view.hpp"

#include <iostream>

/**
 * The constructor and destructor shouldn't need anything added.
 */

PrintView::PrintView(int height, int width)
    : View(height, width)
{
}

PrintView::~PrintView()
{
}

void PrintView::draw(const std::vector<std::string>& lines)
{
    for (std::string a : lines) {
        std::cout << a << std::endl;
    }
}
