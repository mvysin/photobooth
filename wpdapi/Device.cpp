// Device.cpp

#include "wpdapi.h"

namespace WPD {

PortableDevice::~PortableDevice()
{
	this->!PortableDevice();
}

PortableDevice::!PortableDevice()
{
	if (eventSink)
	{
		pDev->Unadvise(eventCookie);
		eventSink->Release();

		CoTaskMemFree(eventCookie);
		eventCookie = 0;
		eventSink = 0;
	}

	if (pDev)
		pDev->Release();
	pDev = 0;
}

PortableDevice::PortableDevice(PortableDeviceManager ^mgr, LPWSTR id)
{
	this->pDev = 0;
	this->isReadOnly = false;
	this->eventSink = 0;
	this->eventCookie = 0;

	this->mgr = mgr;
	this->deviceId = gcnew String(id);
	this->name = mgr->GetDeviceFriendlyName(deviceId);
	this->desc = mgr->GetDeviceDescription(deviceId);
	this->manuf = mgr->GetDeviceManufacturer(deviceId);
	this->cats = nullptr;
}

FunctionCategory PortableDevice::CategoryFromGUID(const GUID &guid)
{
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_AUDIO_CAPTURE, guid))
		return FunctionCategory::AudioCapture;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_DEVICE, guid))
		return FunctionCategory::Device;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_NETWORK_CONFIGURATION, guid))
		return FunctionCategory::NetworkConfiguration;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_RENDERING_INFORMATION, guid))
		return FunctionCategory::RenderingInformation;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_SMS, guid))
		return FunctionCategory::SMS;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_STILL_IMAGE_CAPTURE, guid))
		return FunctionCategory::StillImageCapture;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_STORAGE, guid))
		return FunctionCategory::Storage;
	if (IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_VIDEO_CAPTURE, guid))
		return FunctionCategory::VideoCapture;

	return FunctionCategory::Any;
}

System::Collections::Generic::IList<PortableDeviceCategory^>^ PortableDevice::Categories::get()
{
	if (this->cats != nullptr)
		return this->cats->AsReadOnly();

	// make sure the device is open, at least readonly
	Open(false);

	// create the category list
	System::Collections::Generic::List<PortableDeviceCategory^>^ cats =
		gcnew System::Collections::Generic::List<PortableDeviceCategory^>();

	// Get the capabilities
	IPortableDeviceCapabilities *pCaps = 0;
	HRESULT hr = pDev->Capabilities(&pCaps);
	if (FAILED(hr))
		Util::COMError("Error retrieving device capabilities", hr);

	// enumerate the categories
	IPortableDevicePropVariantCollection *catIds = 0;
	hr = pCaps->GetFunctionalCategories(&catIds);
	if (FAILED(hr)) {
		pCaps->Release();
		Util::COMError("Error retrieving categories for enumeration", hr);
	}

	// loop over them...
	DWORD nCats = 0;
	hr = catIds->GetCount(&nCats);
	if (FAILED(hr)) {
		catIds->Release();
		pCaps->Release();
		Util::COMError("Error getting category count", hr);
	}

	for (DWORD i = 0; i < nCats; i++)
	{
		PROPVARIANT curCat;
		catIds->GetAt(i, &curCat);

		FunctionCategory cat = CategoryFromGUID(*curCat.puuid);

		PortableDeviceCategory ^newCat = gcnew PortableDeviceCategory(this, Util::CreateGuid(*curCat.puuid), cat);
		cats->Add(newCat);

		PropVariantClear(&curCat);
	}

	catIds->Release();
	pCaps->Release();

	this->cats = cats;
	return this->cats->AsReadOnly();
}

PortableDeviceCategory ^PortableDevice::GetCategory(FunctionCategory cat)
{
	for each (PortableDeviceCategory ^c in Categories)
	{
		if (c->Category == cat)
			return c;
	}

	return nullptr;
}

HRESULT PortableDevice::OnEvent(IPortableDeviceValues *params)
{
	GUID guid;
	HRESULT hr = params->GetGuidValue(WPD_EVENT_PARAMETER_EVENT_ID, &guid);
	if (FAILED(hr))
		Util::COMError("Error retrieving event id", hr);

	PortableDeviceEventArgs ^e = gcnew PortableDeviceEventArgs();

	BOOL bVal = 0;
	LPWSTR szVal = 0;
	ULONG ulVal = 0;

	hr = params->GetBoolValue(WPD_EVENT_OPTION_IS_AUTOPLAY_EVENT, &bVal);
	e->Autoplay = bVal ? true : false;

	hr = params->GetBoolValue(WPD_EVENT_OPTION_IS_BROADCAST_EVENT, &bVal);
	e->Broadcast = bVal ? true : false;

	hr = params->GetBoolValue(WPD_EVENT_PARAMETER_CHILD_HIERARCHY_CHANGED, &bVal);
	e->ChildHierarchyChanged = bVal ? true : false;

	hr = params->GetStringValue(WPD_EVENT_PARAMETER_OBJECT_CREATION_COOKIE, &szVal);
	if (szVal) {
		e->ObjectCreationCookie = gcnew String(szVal);
		CoTaskMemFree(szVal);
	}

	hr = params->GetStringValue(WPD_EVENT_PARAMETER_OBJECT_PARENT_PERSISTENT_UNIQUE_ID, &szVal);
	if (szVal) {
		e->ObjectParentId = gcnew String(szVal);
		CoTaskMemFree(szVal);
	}

	hr = params->GetUnsignedIntegerValue(WPD_EVENT_PARAMETER_OPERATION_PROGRESS, &ulVal);
	e->Progress = (int)ulVal;

	hr = params->GetUnsignedIntegerValue(WPD_EVENT_PARAMETER_OPERATION_STATE, &ulVal);
	switch (ulVal)
	{
	case WPD_OPERATION_STATE_UNSPECIFIED:	e->State = OperationState::Unspecified; break;
	case WPD_OPERATION_STATE_STARTED:		e->State = OperationState::Started; break;
	case WPD_OPERATION_STATE_RUNNING:		e->State = OperationState::Running; break;
	case WPD_OPERATION_STATE_PAUSED:		e->State = OperationState::Paused; break;
	case WPD_OPERATION_STATE_CANCELLED:		e->State = OperationState::Cancelled; break;
	case WPD_OPERATION_STATE_FINISHED:		e->State = OperationState::Finished; break;
	case WPD_OPERATION_STATE_ABORTED:		e->State = OperationState::Aborted; break;
	}

	if (IsEqualGUID(WPD_EVENT_DEVICE_CAPABILITIES_UPDATED, guid))
		OnDeviceCapabilitiesUpdated(e);
	else if (IsEqualGUID(WPD_EVENT_DEVICE_REMOVED, guid))
		OnDeviceRemoved(e);
	else if (IsEqualGUID(WPD_EVENT_DEVICE_RESET, guid))
		OnDeviceReset(e);
	else if (IsEqualGUID(WPD_EVENT_OBJECT_ADDED, guid))
		OnObjectAdded(e);
	else if (IsEqualGUID(WPD_EVENT_OBJECT_REMOVED, guid))
		OnObjectRemoved(e);
	else if (IsEqualGUID(WPD_EVENT_OBJECT_TRANSFER_REQUESTED, guid))
		OnObjectTransferRequested(e);
	else if (IsEqualGUID(WPD_EVENT_OBJECT_UPDATED, guid))
		OnObjectUpdated(e);
	else if (IsEqualGUID(WPD_EVENT_STORAGE_FORMAT, guid))
		OnStorageFormat(e);

	return S_OK;
}

void PortableDevice::OnDeviceCapabilitiesUpdated(PortableDeviceEventArgs ^e)
{
	DeviceCapabilitiesUpdated(this, e);
}

void PortableDevice::OnDeviceRemoved(PortableDeviceEventArgs ^e)
{
	DeviceRemoved(this, e);
}

void PortableDevice::OnDeviceReset(PortableDeviceEventArgs ^e)
{
	DeviceReset(this, e);
}

void PortableDevice::OnObjectAdded(PortableDeviceEventArgs ^e)
{
	ObjectAdded(this, e);
}

void PortableDevice::OnObjectRemoved(PortableDeviceEventArgs ^e)
{
	ObjectRemoved(this, e);
}

void PortableDevice::OnObjectTransferRequested(PortableDeviceEventArgs ^e)
{
	ObjectTransferRequested(this, e);
}

void PortableDevice::OnObjectUpdated(PortableDeviceEventArgs ^e)
{
	ObjectUpdated(this, e);
}

void PortableDevice::OnStorageFormat(PortableDeviceEventArgs ^e)
{
	StorageFormat(this, e);
}

void PortableDevice::Open(bool readOnly)
{
	// device open?  If so... check if readonly status needs to change
	if (pDev)
	{
		if (readOnly || !isReadOnly)
			return;

		// close it
		if (eventSink) {
			pDev->Unadvise(eventCookie);
			eventSink->Release();

			CoTaskMemFree(eventCookie);
			eventCookie = 0;
			eventSink = 0;
		}

		pDev->Release();
		pDev = 0;
	}

	IPortableDeviceValues *pValues = 0;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceValues, 0, CLSCTX_INPROC_SERVER,
		IID_IPortableDeviceValues, (void**)&pValues);
	if (FAILED(hr))
		Util::COMError("Error creating device open parameter object", hr);

	if (readOnly)
	{
		hr = pValues->SetUnsignedIntegerValue(WPD_CLIENT_DESIRED_ACCESS, GENERIC_READ);
		if (FAILED(hr)) {
			pValues->Release();
			Util::COMError("Error requesting read-only access", hr);
		}
	}

	hr = pValues->SetUnsignedIntegerValue(WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE, SECURITY_IMPERSONATION);
	if (FAILED(hr)) {
		pValues->Release();
		Util::COMError("Error requesting impersonation", hr);
	}

	// doesn't really matter if these fail
	pValues->SetUnsignedIntegerValue(WPD_CLIENT_MAJOR_VERSION, 1);
	pValues->SetUnsignedIntegerValue(WPD_CLIENT_MINOR_VERSION, 0);
	pValues->SetStringValue(WPD_CLIENT_NAME, L"WPD Camera API (c) Michael Vysin");
	pValues->SetUnsignedIntegerValue(WPD_CLIENT_REVISION, 1000);

	IPortableDevice *newDev = 0;
	hr = CoCreateInstance(CLSID_PortableDevice, 0, CLSCTX_INPROC_SERVER, IID_IPortableDevice, (void**)&newDev);
	if (FAILED(hr)) {
		pValues->Release();
		Util::COMError("Error creating device interface", hr);
	}

	pin_ptr<const wchar_t> pinid = PtrToStringChars(deviceId);
	hr = newDev->Open(pinid, pValues);
	pValues->Release();
	if (FAILED(hr)) {
		newDev->Release();
		Util::COMError("Error opening device", hr);
	}

	cats = nullptr;

	// register event callbacks
	EventSink *eventSink = new EventSink(this);
	LPWSTR cookie;
	hr = newDev->Advise(0, eventSink, 0, &cookie);
	if (FAILED(hr)) {
		newDev->Release();
		eventSink->Release();
		Util::COMError("Error registering event interface", hr);
	}

	this->eventSink = eventSink;
	this->eventCookie = cookie;

	isReadOnly = readOnly;
	pDev = newDev;
}




PortableDeviceCategory::PortableDeviceCategory(PortableDevice ^dev, Guid ^guid, FunctionCategory cat)
{
	this->dev = dev;
	this->cat = cat;
	this->guid = guid;
	this->objs = nullptr;
}

IList<FunctionalObject^> ^PortableDeviceCategory::Functions::get()
{
	if (this->objs != nullptr)
		return this->objs->AsReadOnly();

	List<FunctionalObject^> ^newObjs = gcnew List<FunctionalObject^>();

	// Get the capabilities
	IPortableDeviceCapabilities *pCaps = 0;
	HRESULT hr = Device->Handle->Capabilities(&pCaps);
	if (FAILED(hr))
		Util::COMError("Error retrieving device capabilities", hr);

	GUID myGuid;
	Util::CreateNativeGuid(Id, myGuid);

	IPortableDevicePropVariantCollection *objectIds = 0;
	hr = pCaps->GetFunctionalObjects(myGuid, &objectIds);
	if (FAILED(hr)) {
		pCaps->Release();
		Util::COMError("Error retrieving functional objects", hr);
	}

	DWORD n = 0;
	hr = objectIds->GetCount(&n);
	if (FAILED(hr)) {
		pCaps->Release();
		objectIds->Release();
		Util::COMError("Error retrieving functional object count", hr);
	}

	for (DWORD i = 0; i < n; i++)
	{
		PROPVARIANT curObj;
		hr = objectIds->GetAt(i, &curObj);
		if (FAILED(hr)) {
			pCaps->Release();
			objectIds->Release();
			Util::COMError("Error retrieving functional object", hr);
		}

		FunctionalObject ^newObj = nullptr;
		String ^strId = gcnew String(curObj.pwszVal);
		switch (this->Category)
		{
		case FunctionCategory::StillImageCapture:
			newObj = gcnew StillImageObject(this, strId);
			break;
		case FunctionCategory::Storage:
			newObj = gcnew StorageObject(this, strId);
			break;
		default:
			newObj = gcnew FunctionalObject(this, strId);
			break;
		}
		newObjs->Add(newObj);

		PropVariantClear(&curObj);
	}

	pCaps->Release();
	objectIds->Release();

	this->objs = newObjs;
	return this->objs->AsReadOnly();
}

}