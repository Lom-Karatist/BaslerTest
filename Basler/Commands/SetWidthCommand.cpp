#include "SetWidthCommand.h"
#include "Basler/BaslerApi.h"

SetWidthCommand::SetWidthCommand(int width)
    : m_width(width)
{
}

void SetWidthCommand::execute(BaslerApi* api)
{
    api->applyWidthChanging(m_width);
}
