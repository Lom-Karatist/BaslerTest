#ifndef SETFRAMERATECOMMAND_H
#define SETFRAMERATECOMMAND_H

#include "ParameterCommand.h"

class SetFramerateCommand : public ParameterCommand
{
public:
    explicit SetFramerateCommand(double fps);
    void execute(BaslerApi* api) override;

private:
    double m_fps;
};

#endif // SETFRAMERATECOMMAND_H
