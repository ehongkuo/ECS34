#ifndef INTERFACE_TYPE_HPP
#define INTERFACE_TYPE_HPP

/**
 * You shouldn't need to change this file.
 *
 * I didn't put this in view.hpp or controller.hpp because if I had, then:
 * a) One would have to include the other just for an enumerator.
 * b) The file that implements main() would have to include one of those,
 *    even though the file that implements main() shouldn't need to care
 *    or know about the view/controller.
 */
enum class InterfaceType
{
    Curses,
    Print
};

#endif // INTERFACE_TYPE_HPP
