#ifndef PRINT_CONTROLLER_HPP
#define PRINT_CONTROLLER_HPP

#include "controller.hpp"

class PrintController : public Controller
{
public:
    PrintController() = default;
    virtual ~PrintController() override;
    PrintController(const PrintController&) = delete;
    PrintController& operator=(const PrintController&) = delete;
    virtual Command getInput() override;
};

#endif // PRINT_CONTROLLER_HPP
