// Storage.cpp

#include "wpdapi.h"

namespace WPD {

StorageObject::StorageObject(PortableDeviceCategory ^cat, String ^id) : FunctionalObject(cat, id)
{
	this->children = nullptr;
}

StorageObject::~StorageObject()
{
	this->!StorageObject();
}

StorageObject::!StorageObject()
{
}

IList<StorageItem^> ^StorageObject::Items::get()
{
	if (children != nullptr)
		return children;

	IPortableDeviceContent *pContent = 0;
	HRESULT hr = Device->Handle->Content(&pContent);
	if (FAILED(hr))
		Util::COMError("Error getting device content", hr);

	pin_ptr<const wchar_t> id = PtrToStringChars(Id);
	IEnumPortableDeviceObjectIDs *pEnum = 0;
	hr = pContent->EnumObjects(0, id, 0, &pEnum);
	pContent->Release();
	if (FAILED(hr))
	{
		Util::COMError("Error getting enumerator for root", hr);
	}

	// loop over the objects
	LPWSTR objs[16];
	ULONG nObjs = 0;
	List<StorageItem^> ^list = gcnew List<StorageItem^>();
	for (;;)
	{
		hr = pEnum->Next(16, objs, &nObjs);
		if (FAILED(hr)) {
			pEnum->Release();
			Util::COMError("Error retrieving next set of records", hr);
		}

		for (ULONG i = 0; i < nObjs; i++)
		{
			list->Add(gcnew StorageItem(this, nullptr, gcnew String(objs[i])));
			CoTaskMemFree(objs[i]);
		}

		if (hr == S_FALSE)
			break;
	}

	pEnum->Release();
	children = list;

	return children;
}

void StorageObject::Refresh()
{
	children = nullptr;
}




StorageItem::StorageItem(StorageObject ^obj, StorageItem ^parent, String ^id)
{
	this->obj = obj;
	this->parent = parent;
	this->id = id;
	this->children = nullptr;
}

IList<StorageItem^> ^StorageItem::Items::get()
{
	if (children != nullptr)
		return children;

	IPortableDeviceContent *pContent = 0;
	HRESULT hr = ParentObject->Device->Handle->Content(&pContent);
	if (FAILED(hr))
		Util::COMError("Error getting device content", hr);

	pin_ptr<const wchar_t> id = PtrToStringChars(Id);
	IEnumPortableDeviceObjectIDs *pEnum = 0;
	hr = pContent->EnumObjects(0, id, 0, &pEnum);
	pContent->Release();
	if (FAILED(hr))
	{
		Util::COMError("Error getting enumerator for root", hr);
	}

	// loop over the objects
	LPWSTR objs[16];
	ULONG nObjs = 0;
	List<StorageItem^> ^list = gcnew List<StorageItem^>();
	for (;;)
	{
		hr = pEnum->Next(16, objs, &nObjs);
		if (FAILED(hr)) {
			pEnum->Release();
			Util::COMError("Error retrieving next set of records", hr);
		}

		for (ULONG i = 0; i < nObjs; i++)
			list->Add(gcnew StorageItem(ParentObject, this, gcnew String(objs[i])));

		if (hr == S_FALSE)
			break;
	}

	pEnum->Release();
	children = list;

	return children;
}

String ^StorageItem::Name::get()
{
	IPortableDeviceContent *pContent = 0;
	HRESULT hr = ParentObject->Device->Handle->Content(&pContent);
	if (FAILED(hr))
		Util::COMError("Error getting device content", hr);

	IPortableDeviceProperties *pProps = 0;
	hr = pContent->Properties(&pProps);
	if (FAILED(hr))
		Util::COMError("Error getting property interface", hr);

	pin_ptr<const wchar_t> id = PtrToStringChars(Id);

	IPortableDeviceKeyCollection *keys = 0;
	hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection, 0, CLSCTX_INPROC_SERVER, IID_IPortableDeviceKeyCollection, (void**)&keys);
	if (FAILED(hr))
		Util::COMError("Error creating key collection", hr);

	hr = keys->Add(WPD_OBJECT_NAME);
	if (FAILED(hr))
		Util::COMError("Error adding object name to retrieval list", hr);

	IPortableDeviceValues *values = 0;
	hr = pProps->GetValues(id, keys, &values);
	if (FAILED(hr))
		Util::COMError("Error getting object name", hr);

	LPWSTR str = 0;
	values->GetStringValue(WPD_OBJECT_NAME, &str);
	String ^ret = gcnew String(str);

	CoTaskMemFree(str);
	values->Release();
	keys->Release();
	pProps->Release();
	pContent->Release();

	return ret;
}

void StorageItem::TransferTo(System::IO::MemoryStream ^ms)
{
	IPortableDeviceContent *pContent = 0;
	HRESULT hr = ParentObject->Device->Handle->Content(&pContent);
	if (FAILED(hr))
		Util::COMError("Error getting device content", hr);

	IPortableDeviceResources *pRsrc = 0;
	hr = pContent->Transfer(&pRsrc);
	if (FAILED(hr))
		Util::COMError("Error getting transfer interface", hr);

	pContent->Release();

	pin_ptr<const wchar_t> id = PtrToStringChars(Id);

	DWORD szBfr = 1024;
	IStream *pStrm = 0;
	hr = pRsrc->GetStream(id, WPD_RESOURCE_DEFAULT, STGM_READ, &szBfr, &pStrm);
	if (FAILED(hr))
		Util::COMError("Error getting stream", hr);

	unsigned char *bfr = new unsigned char[szBfr];
	array<unsigned char> ^arr = gcnew array<unsigned char>(szBfr);
	ULONG szRead = 0;

	for (;;)
	{
		hr = pStrm->Read(bfr, szBfr, &szRead);
		if (szRead == 0)
			break;

		System::Runtime::InteropServices::Marshal::Copy((IntPtr)bfr, arr, 0, szRead);
		ms->Write(arr, 0, szRead);
		if (FAILED(hr) || hr == S_FALSE)
			break;
	}

	pStrm->Release();
	pRsrc->Release();
}

}