#include "SetPixelFormatCommand.h"
#include "Basler/BaslerApi.h"

SetPixelFormatCommand::SetPixelFormatCommand(int pixelFormat)
    : m_pixelFormat(pixelFormat)
{
}

void SetPixelFormatCommand::execute(BaslerApi* api)
{
    api->applyPixelFormatChanging(m_pixelFormat);
}
