#include "SetOffsetXCommand.h"
#include "Basler/BaslerApi.h"

SetOffsetXCommand::SetOffsetXCommand(int offsetX)
    : m_offsetX(offsetX)
{

}

void SetOffsetXCommand::execute(BaslerApi* api)
{
    api->applyOffsetXChanging(m_offsetX);
}
