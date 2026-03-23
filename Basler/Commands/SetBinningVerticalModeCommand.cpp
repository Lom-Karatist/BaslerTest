#include "SetBinningVerticalModeCommand.h"

SetBinningVerticalModeCommand::SetBinningVerticalModeCommand(BinningVerticalModeEnums mode)
    : m_mode(mode)
{
}

void SetBinningVerticalModeCommand::execute(BaslerApi* api)
{
    api->applyBinningVerticalModeChanging(m_mode);
}
