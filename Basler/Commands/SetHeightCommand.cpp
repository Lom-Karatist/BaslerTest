#include "SetHeightCommand.h"
#include "Basler/BaslerApi.h"

SetHeightCommand::SetHeightCommand(int height)
    : m_height(height)
{

}

void SetHeightCommand::execute(BaslerApi* api)
{
    api->applyHeightChanging(m_height);
}
