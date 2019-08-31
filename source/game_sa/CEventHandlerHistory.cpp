#include "StdInc.h"

void CEventHandlerHistory::ClearAllEvents()
{
    plugin::CallMethod<0x4BC550, CEventHandlerHistory*>(this);
}

CEvent* CEventHandlerHistory::GetCurrentEvent()
{
    return plugin::CallMethodAndReturn<CEvent*, 0x4B8CA0, CEventHandlerHistory*>(this);
}