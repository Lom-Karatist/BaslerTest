#include "SetBinningVerticalCommand.h"
#include "Basler/BaslerApi.h"

SetBinningVerticalCommand::SetBinningVerticalCommand(int binningVertical)
    : m_binningVertical(binningVertical)
{

}

void SetBinningVerticalCommand::execute(BaslerApi* api)
{
    api->applyBinningVerticalChanging(m_binningVertical);
}
