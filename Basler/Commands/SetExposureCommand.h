#ifndef SETEXPOSURECOMMAND_H
#define SETEXPOSURECOMMAND_H

#include "ParameterCommand.h"

class SetExposureCommand : public ParameterCommand
{
public:
    explicit SetExposureCommand(double exposureMs);
    void execute(BaslerApi* api) override;

private:
    double m_exposureMs;
};

#endif // SETEXPOSURECOMMAND_H
