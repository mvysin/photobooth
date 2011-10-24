// Events.cpp

#include "wpdapi.h"

namespace WPD {

HRESULT __stdcall EventSink::OnEvent(IPortableDeviceValues *params)
{
	return dev->OnEvent(params);
}

}