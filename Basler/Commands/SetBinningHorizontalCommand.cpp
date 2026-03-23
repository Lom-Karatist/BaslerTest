#include "SetBinningHorizontalCommand.h"
#include "Basler/BaslerApi.h"

SetBinningHorizontalCommand::SetBinningHorizontalCommand(int binningHorizontal)
    : m_binningHorizontal(binningHorizontal)
{

}

void SetBinningHorizontalCommand::execute(BaslerApi* api)
{
    api->applyBinningHorizontalChanging(m_binningHorizontal);
}
