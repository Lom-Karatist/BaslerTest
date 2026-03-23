#ifndef SETBINNINGHORIZONTALCOMMAND_H
#define SETBINNINGHORIZONTALCOMMAND_H

#include "ParameterCommand.h"

class SetBinningHorizontalCommand : public ParameterCommand
{
public:
    SetBinningHorizontalCommand(int binningHorizontal);
    void execute(BaslerApi* api) override;

private:
    int m_binningHorizontal;
};

#endif // SETBINNINGHORIZONTALCOMMAND_H
