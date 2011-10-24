// Manager.cpp

#include "wpdapi.h"

namespace WPD {

void PortableDeviceManager::CreateDeviceList()
{
	DWORD n = 0;
	HRESULT hr = pMgr->GetDevices(0, &n);
	if (FAILED(hr))
		Util::COMError("Error retrieving device list", hr);

	LPWSTR *pIds = new LPWSTR[n];
	hr = pMgr->GetDevices(pIds, &n);
	if (FAILED(hr))
		Util::COMError("Error retrieving device count", hr);

	devs = gcnew System::Collections::Generic::List<PortableDevice^>();
	for (DWORD i = 0; i < n; i++)
	{
		PortableDevice ^dev = gcnew PortableDevice(this, pIds[i]);
		devs->Add(dev);
	}
}

System::Collections::Generic::IList<PortableDevice^>^ PortableDeviceManager::DevicesFilteredBy(FunctionCategory cat)
{
	if (cat == FunctionCategory::Any)
		return this->Devices;

	System::Collections::Generic::List<PortableDevice^> ^sublist = gcnew System::Collections::Generic::List<PortableDevice^>;
	for each (PortableDevice ^d in devs)
	{
		for each (PortableDeviceCategory ^c in d->Categories)
		{
			if (c->Category == cat)
				sublist->Add(d);
		}
	}

	return sublist->AsReadOnly();
}

String ^PortableDeviceManager::GetDeviceDescription(String ^id)
{
	pin_ptr<const wchar_t> pinid = PtrToStringChars(id);

	DWORD n = 0;
	HRESULT hr = pMgr->GetDeviceDescription(pinid, 0, &n);
	if (FAILED(hr))
		Util::COMError("Error getting device description", hr);

	WCHAR *desc = new WCHAR[n];
	hr = pMgr->GetDeviceDescription(pinid, desc, &n);
	if (FAILED(hr)) {
		delete [] desc;
		Util::COMError("Error getting device description", hr);
	}

	String ^ret = gcnew String(desc);
	delete [] desc;
	return ret;
}

String ^PortableDeviceManager::GetDeviceFriendlyName(String ^id)
{
	pin_ptr<const wchar_t> pinid = PtrToStringChars(id);

	DWORD n = 0;
	HRESULT hr = pMgr->GetDeviceFriendlyName(pinid, 0, &n);
	if (FAILED(hr))
		Util::COMError("Error getting device friendly name", hr);

	WCHAR *name = new WCHAR[n];
	hr = pMgr->GetDeviceFriendlyName(pinid, name, &n);
	if (FAILED(hr)) {
		delete [] name;
		Util::COMError("Error getting device friendly name", hr);
	}

	String ^ret = gcnew String(name);
	delete [] name;
	return ret;
}

String ^PortableDeviceManager::GetDeviceManufacturer(String ^id)
{
	pin_ptr<const wchar_t> pinid = PtrToStringChars(id);

	DWORD n = 0;
	HRESULT hr = pMgr->GetDeviceManufacturer(pinid, 0, &n);
	if (FAILED(hr))
		Util::COMError("Error getting device manufacturer", hr);

	WCHAR *man = new WCHAR[n];
	hr = pMgr->GetDeviceManufacturer(pinid, man, &n);
	if (FAILED(hr)) {
		delete [] man;
		Util::COMError("Error getting device manufacturer", hr);
	}

	String ^ret = gcnew String(man);
	delete [] man;
	return ret;
}

}