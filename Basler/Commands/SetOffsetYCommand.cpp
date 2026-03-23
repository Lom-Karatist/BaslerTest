#include "SetOffsetYCommand.h"
#include "Basler/BaslerApi.h"

SetOffsetYCommand::SetOffsetYCommand(int offsetY)
    : m_offsetY(offsetY)
{

}
void SetOffsetYCommand::execute(BaslerApi* api)
{
    api->applyOffsetYChanging(m_offsetY);
}
