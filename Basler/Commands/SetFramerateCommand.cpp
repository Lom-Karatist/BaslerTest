#include "SetFramerateCommand.h"
#include "Basler/BaslerApi.h"

SetFramerateCommand::SetFramerateCommand(double fps)
    : m_fps(fps)
{
}

void SetFramerateCommand::execute(BaslerApi* api)
{
    api->applyFramerateChanging(m_fps);
}
