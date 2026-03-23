#ifndef SETBINNINGVERTICALCOMMAND_H
#define SETBINNINGVERTICALCOMMAND_H

#include "ParameterCommand.h"

class SetBinningVerticalCommand : public ParameterCommand
{
public:
    SetBinningVerticalCommand(int binningVertical);
    void execute(BaslerApi* api) override;

private:
    int m_binningVertical;
};

#endif // SETBINNINGVERTICALCOMMAND_H
