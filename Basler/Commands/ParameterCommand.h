#ifndef PARAMETERCOMMAND_H
#define PARAMETERCOMMAND_H

class BaslerApi;

class ParameterCommand
{
public:
    virtual ~ParameterCommand() = default;
    virtual void execute(BaslerApi* api) = 0;
};

#endif // PARAMETERCOMMAND_H