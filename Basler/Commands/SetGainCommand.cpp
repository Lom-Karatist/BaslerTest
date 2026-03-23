#include "SetGainCommand.h"
#include "Basler/BaslerApi.h"

SetGainCommand::SetGainCommand(double gain)
    : m_gain(gain)
{
}

void SetGainCommand::execute(BaslerApi* api)
{
    api->applyGainChanging(m_gain);
}
