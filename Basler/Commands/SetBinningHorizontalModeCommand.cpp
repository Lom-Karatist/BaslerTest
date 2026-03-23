#include "SetBinningHorizontalModeCommand.h"

SetBinningHorizontalModeCommand::SetBinningHorizontalModeCommand(BinningHorizontalModeEnums mode)
    : m_mode(mode)
{
}

void SetBinningHorizontalModeCommand::execute(BaslerApi* api)
{
    api->applyBinningHorizontalModeChanging(m_mode);
}
