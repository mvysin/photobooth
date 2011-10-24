// Events.h

#pragma once

namespace WPD {

	ref class PortableDevice;

	class EventSink : public IPortableDeviceEventCallback
	{
		ULONG refs;
		gcroot<PortableDevice^> dev;

	public:
		EventSink(PortableDevice ^dev)
		{
			this->dev = dev;
			refs = 1;
		}

		HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
		{
			if (!ppv)
				return E_INVALIDARG;
			if (riid == IID_IUnknown || riid == IID_IPortableDeviceEventCallback)
			{
				AddRef();
				*ppv = this;
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		ULONG __stdcall AddRef()
		{
			InterlockedIncrement((long*)&refs);
			return refs;
		}

		ULONG __stdcall Release()
		{
			ULONG newRefs = refs - 1;
			if (InterlockedDecrement((long*)&refs) == 0)
			{
				delete this;
				return 0;
			}

			return newRefs;
		}

		HRESULT __stdcall OnEvent(IPortableDeviceValues *params);
	};

}