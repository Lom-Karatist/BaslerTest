#ifndef SETOFFSETYCOMMAND_H
#define SETOFFSETYCOMMAND_H

#include "ParameterCommand.h"

class SetOffsetYCommand : public ParameterCommand
{
public:
    SetOffsetYCommand(int offsetY);
    void execute(BaslerApi* api) override;

private:
    int m_offsetY;
};

#endif // SETOFFSETYCOMMAND_H
