#ifndef SETWIDTHCOMMAND_H
#define SETWIDTHCOMMAND_H

#include "ParameterCommand.h"

class SetWidthCommand : public ParameterCommand
{
public:
    explicit SetWidthCommand(int width);
    void execute(BaslerApi* api) override;

private:
    int m_width;
};

#endif // SETWIDTHCOMMAND_H