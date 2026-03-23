#ifndef SETOFFSETXCOMMAND_H
#define SETOFFSETXCOMMAND_H

#include "ParameterCommand.h"

class SetOffsetXCommand : public ParameterCommand
{
public:
    SetOffsetXCommand(int offsetX);
    void execute(BaslerApi* api) override;

private:
    int m_offsetX;
};

#endif // SETOFFSETXCOMMAND_H
