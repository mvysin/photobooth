// Manager.h

#pragma once

using namespace System;

bool matchDeviceCategory(GUID &guidCat, WPD::FunctionCategory enumCat);

namespace WPD {

	public ref class PortableDeviceManager
	{
		IPortableDeviceManager *pMgr;
		System::Collections::Generic::List<PortableDevice^> ^devs;

		void CreateDeviceList();

	internal:
		String ^GetDeviceDescription(String ^id);
		String ^GetDeviceFriendlyName(String ^id);
		String ^GetDeviceManufacturer(String ^id);

	public:
		PortableDeviceManager()
		{
			IPortableDeviceManager *pMgr;
			HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager, 0, CLSCTX_ALL, IID_IPortableDeviceManager, (void**)&pMgr);
			if (FAILED(hr))
				Util::COMError("Error connecting to manager", hr);
			this->pMgr = pMgr;

			CreateDeviceList();
		}

		~PortableDeviceManager() { this->!PortableDeviceManager(); }

		!PortableDeviceManager()
		{
			if (pMgr)
				pMgr->Release();
			pMgr = 0;
		}

		property System::Collections::Generic::IList<PortableDevice^>^ Devices {
			System::Collections::Generic::IList<PortableDevice^>^ get() {
				return devs->AsReadOnly();
			}
		}

		System::Collections::Generic::IList<PortableDevice^>^ DevicesFilteredBy(FunctionCategory cat);
	};
}