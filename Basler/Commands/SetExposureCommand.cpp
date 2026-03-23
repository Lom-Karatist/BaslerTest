#include "SetExposureCommand.h"
#include "Basler/BaslerApi.h"
#include "Basler/BaslerApi.h"

SetExposureCommand::SetExposureCommand(double exposureMs)
    : m_exposureMs(exposureMs)
{
}

void SetExposureCommand::execute(BaslerApi* api)
{
    api->applyExposureChanging(m_exposureMs);
}
