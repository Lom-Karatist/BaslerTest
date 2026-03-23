#ifndef SETHEIGHTCOMMAND_H
#define SETHEIGHTCOMMAND_H

#include "ParameterCommand.h"

class SetHeightCommand : public ParameterCommand
{
public:
    SetHeightCommand(int height);
    void execute(BaslerApi* api) override;

private:
    int m_height;
};

#endif // SETHEIGHTCOMMAND_H
