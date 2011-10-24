// StillImageObject.cpp

#include "wpdapi.h"

namespace WPD {

StillImageObject::StillImageObject(PortableDeviceCategory ^cat, String ^str) : FunctionalObject(cat, str)
{
	pProps = 0;
}

StillImageObject::~StillImageObject()
{
	this->!StillImageObject();
}

StillImageObject::!StillImageObject()
{
	if (pProps)
		pProps->Release();
	pProps = 0;
}

void StillImageObject::Capture()
{
	IPortableDeviceValues *values = 0;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceValues, 0, CLSCTX_INPROC_SERVER, IID_IPortableDeviceValues, (void**)&values);
	if (FAILED(hr))
		Util::COMError("Error creating values for capture", hr);

	pin_ptr<const wchar_t> objectId = PtrToStringChars(this->Id);

	hr = values->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_STILL_IMAGE_CAPTURE_INITIATE.fmtid);
	HRESULT hr2 = values->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_STILL_IMAGE_CAPTURE_INITIATE.pid);
	HRESULT hr3 = values->SetStringValue(WPD_PROPERTY_COMMON_COMMAND_TARGET, objectId);
	if (FAILED(hr) || FAILED(hr2) || FAILED(hr3)) {
		values->Release();
		Util::COMError("Error setting command parameters", hr);
	}

	IPortableDeviceValues *result = 0;
	hr = Device->Handle->SendCommand(0, values, &result);
	values->Release();

	if (FAILED(hr))
		Util::COMError("Error sending capture command", hr);

	if (result)
		result->Release();
}

void StillImageObject::Open()
{
	if (pProps != 0)
		return;

	IPortableDeviceContent *content = 0;
	HRESULT hr = Device->Handle->Content(&content);
	if (FAILED(hr))
		Util::COMError("Error getting device content handle", hr);

	IPortableDeviceProperties *props = 0;
	hr = content->Properties(&props);
	if (FAILED(hr))
		Util::COMError("Error getting properties handle", hr);

	content->Release();
	pProps = props;
}

int StillImageObject::PropGetI4(const PROPERTYKEY id)
{
	Open();
	
	pin_ptr<const wchar_t> objectId = PtrToStringChars(this->Id);

	IPortableDeviceKeyCollection *pColl = 0;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection, 0, CLSCTX_INPROC_SERVER, IID_IPortableDeviceKeyCollection, (void**)&pColl);
	if (FAILED(hr))
		Util::COMError("Error creating key collection", hr);

	hr = pColl->Add(id);
	if (FAILED(hr)) {
		pColl->Release();
		Util::COMError("Error adding property to collection", hr);
	}

	IPortableDeviceValues *pValue = 0;
	hr = pProps->GetValues(objectId, pColl, &pValue);
	pColl->Release();
	if (FAILED(hr))
		Util::COMError("Error retrieving property values", hr);

	PROPVARIANT value;
	hr = pValue->GetValue(id, &value);
	if (FAILED(hr)) {
		pValue->Release();
		Util::COMError("Error unboxing property value", hr);
	}

	if (value.vt == VT_ERROR) {
		pValue->Release();
		PropVariantClear(&value);
		Util::COMError("Error retrieving value of property", value.scode);
	}

	int retval = value.intVal;
	PropVariantClear(&value);
	pValue->Release();

	return retval;
}

unsigned int StillImageObject::PropGetUI4(const PROPERTYKEY id)
{
	Open();
	
	pin_ptr<const wchar_t> objectId = PtrToStringChars(this->Id);

	IPortableDeviceKeyCollection *pColl = 0;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection, 0, CLSCTX_INPROC_SERVER, IID_IPortableDeviceKeyCollection, (void**)&pColl);
	if (FAILED(hr))
		Util::COMError("Error creating key collection", hr);

	hr = pColl->Add(id);
	if (FAILED(hr)) {
		pColl->Release();
		Util::COMError("Error adding property to collection", hr);
	}

	IPortableDeviceValues *pValue = 0;
	hr = pProps->GetValues(objectId, pColl, &pValue);
	pColl->Release();
	if (FAILED(hr))
		Util::COMError("Error retrieving property values", hr);

	PROPVARIANT value;
	hr = pValue->GetValue(id, &value);
	if (FAILED(hr)) {
		pValue->Release();
		Util::COMError("Error unboxing property value", hr);
	}

	if (value.vt == VT_ERROR) {
		pValue->Release();
		PropVariantClear(&value);
		Util::COMError("Error retrieving value of property", value.scode);
	}

	unsigned int retval = value.uintVal;
	PropVariantClear(&value);
	pValue->Release();

	return retval;
}

void StillImageObject::PropSetI4(const PROPERTYKEY id, int v)
{
	Open();
	
	pin_ptr<const wchar_t> objectId = PtrToStringChars(this->Id);

	IPortableDeviceValues *pValues = 0;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceValues, 0, CLSCTX_INPROC_SERVER, IID_IPortableDeviceValues, (void**)&pValues);
	if (FAILED(hr))
		Util::COMError("Error creating key collection", hr);

	PROPVARIANT value;
	PropVariantInit(&value);
	value.vt = VT_I4;
	value.intVal = v;

	hr = pValues->SetValue(id, &value);
	if (FAILED(hr)) {
		pValues->Release();
		Util::COMError("Error adding property to collection", hr);
	}

	PropVariantClear(&value);

	IPortableDeviceValues *pSuccessValue = 0;
	hr = pProps->SetValues(objectId, pValues, &pSuccessValue);
	pValues->Release();
	if (FAILED(hr))
		Util::COMError("Error setting property values", hr);

	hr = pSuccessValue->GetValue(id, &value);
	if (FAILED(hr)) {
		pSuccessValue->Release();
		Util::COMError("Error unboxing property error value", hr);
	}

	if (value.vt == VT_ERROR && FAILED(value.scode)) {
		pSuccessValue->Release();
		Util::COMError("Error setting value of property", value.scode);
	}

	PropVariantClear(&value);
	pSuccessValue->Release();
}

void StillImageObject::PropSetUI4(const PROPERTYKEY id, unsigned int v)
{
	Open();
	
	pin_ptr<const wchar_t> objectId = PtrToStringChars(this->Id);

	IPortableDeviceValues *pValues = 0;
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceValues, 0, CLSCTX_INPROC_SERVER, IID_IPortableDeviceValues, (void**)&pValues);
	if (FAILED(hr))
		Util::COMError("Error creating key collection", hr);

	PROPVARIANT value;
	PropVariantInit(&value);
	value.vt = VT_UI4;
	value.uintVal = v;

	hr = pValues->SetValue(id, &value);
	if (FAILED(hr)) {
		pValues->Release();
		Util::COMError("Error adding property to collection", hr);
	}

	PropVariantClear(&value);

	IPortableDeviceValues *pSuccessValue = 0;
	hr = pProps->SetValues(objectId, pValues, &pSuccessValue);
	pValues->Release();
	if (FAILED(hr))
		Util::COMError("Error setting property values", hr);

	hr = pSuccessValue->GetValue(id, &value);
	if (FAILED(hr)) {
		pSuccessValue->Release();
		Util::COMError("Error unboxing property error value", hr);
	}

	if (value.vt == VT_ERROR && FAILED(value.scode)) {
		pSuccessValue->Release();
		Util::COMError("Error setting value of property", value.scode);
	}

	PropVariantClear(&value);
	pSuccessValue->Release();
}




unsigned int StillImageObject::FromCaptureProgram(CaptureProgram v)
{
	switch (v)
	{
	case CaptureProgram::Normal:		return WPD_CAPTURE_MODE_NORMAL;
	case CaptureProgram::Burst:			return WPD_CAPTURE_MODE_BURST;
	case CaptureProgram::Timelapse:		return WPD_CAPTURE_MODE_TIMELAPSE;
	default:							return WPD_CAPTURE_MODE_UNDEFINED;
	}
}

CaptureProgram StillImageObject::ToCaptureProgram(unsigned int v)
{
	switch (v)
	{
	case WPD_CAPTURE_MODE_NORMAL:		return CaptureProgram::Normal;
	case WPD_CAPTURE_MODE_BURST:		return CaptureProgram::Burst;
	case WPD_CAPTURE_MODE_TIMELAPSE:	return CaptureProgram::Timelapse;
	default:							return CaptureProgram::Undefined;
	}
}

unsigned int StillImageObject::FromExposureMetering(ExposureMetering v)
{
	switch (v)
	{
	case ExposureMetering::Average:		return WPD_EXPOSURE_METERING_MODE_AVERAGE;
	case ExposureMetering::CenterWeightedAverage:	return WPD_EXPOSURE_METERING_MODE_CENTER_WEIGHTED_AVERAGE;
	case ExposureMetering::MultiSpot:	return WPD_EXPOSURE_METERING_MODE_MULTI_SPOT;
	case ExposureMetering::CenterSpot:	return WPD_EXPOSURE_METERING_MODE_CENTER_SPOT;
	default:							return WPD_EXPOSURE_METERING_MODE_UNDEFINED;
	}
}

ExposureMetering StillImageObject::ToExposureMetering(unsigned int v)
{
	switch (v)
	{
	case WPD_EXPOSURE_METERING_MODE_AVERAGE:		return ExposureMetering::Average;
	case WPD_EXPOSURE_METERING_MODE_CENTER_WEIGHTED_AVERAGE:	return ExposureMetering::CenterWeightedAverage;
	case WPD_EXPOSURE_METERING_MODE_MULTI_SPOT:		return ExposureMetering::MultiSpot;
	case WPD_EXPOSURE_METERING_MODE_CENTER_SPOT:	return ExposureMetering::CenterSpot;
	default:										return ExposureMetering::Undefined;
	}
}

unsigned int StillImageObject::FromExposureProgram(ExposureProgram v)
{
	switch (v)
	{
	case ExposureProgram::Manual:			return WPD_EXPOSURE_PROGRAM_MODE_MANUAL;
	case ExposureProgram::Auto:				return WPD_EXPOSURE_PROGRAM_MODE_AUTO;
	case ExposureProgram::AperturePriority:	return WPD_EXPOSURE_PROGRAM_MODE_APERTURE_PRIORITY;
	case ExposureProgram::ShutterPriority:	return WPD_EXPOSURE_PROGRAM_MODE_SHUTTER_PRIORITY;
	case ExposureProgram::Creative:			return WPD_EXPOSURE_PROGRAM_MODE_CREATIVE;
	case ExposureProgram::Action:			return WPD_EXPOSURE_PROGRAM_MODE_ACTION;
	case ExposureProgram::Portrait:			return WPD_EXPOSURE_PROGRAM_MODE_PORTRAIT;
	default:								return WPD_EXPOSURE_PROGRAM_MODE_UNDEFINED;
	}
}

ExposureProgram StillImageObject::ToExposureProgram(unsigned int v)
{
	switch (v)
	{
	case WPD_EXPOSURE_PROGRAM_MODE_MANUAL:		return ExposureProgram::Manual;
	case WPD_EXPOSURE_PROGRAM_MODE_AUTO:		return ExposureProgram::Auto;
	case WPD_EXPOSURE_PROGRAM_MODE_APERTURE_PRIORITY:	return ExposureProgram::AperturePriority;
	case WPD_EXPOSURE_PROGRAM_MODE_SHUTTER_PRIORITY:	return ExposureProgram::ShutterPriority;
	case WPD_EXPOSURE_PROGRAM_MODE_CREATIVE:	return ExposureProgram::Creative;
	case WPD_EXPOSURE_PROGRAM_MODE_ACTION:		return ExposureProgram::Action;
	case WPD_EXPOSURE_PROGRAM_MODE_PORTRAIT:	return ExposureProgram::Portrait;
	default:									return ExposureProgram::Undefined;
	}
}

unsigned int StillImageObject::FromFlashProgram(FlashProgram v)
{
	switch (v)
	{
	case FlashProgram::Auto:				return WPD_FLASH_MODE_AUTO;
	case FlashProgram::Off:					return WPD_FLASH_MODE_OFF;
	case FlashProgram::Fill:				return WPD_FLASH_MODE_FILL;
	case FlashProgram::RedEyeAuto:			return WPD_FLASH_MODE_RED_EYE_AUTO;
	case FlashProgram::RedEyeFill:			return WPD_FLASH_MODE_RED_EYE_FILL;
	case FlashProgram::ExternalSync:		return WPD_FLASH_MODE_EXTERNAL_SYNC;
	default:								return WPD_FLASH_MODE_UNDEFINED;
	}
}

FlashProgram StillImageObject::ToFlashProgram(unsigned int v)
{
	switch (v)
	{
	case WPD_FLASH_MODE_AUTO:				return FlashProgram::Auto;
	case WPD_FLASH_MODE_OFF:				return FlashProgram::Off;
	case WPD_FLASH_MODE_FILL:				return FlashProgram::Fill;
	case WPD_FLASH_MODE_RED_EYE_AUTO:		return FlashProgram::RedEyeAuto;
	case WPD_FLASH_MODE_RED_EYE_FILL:		return FlashProgram::RedEyeFill;
	case WPD_FLASH_MODE_EXTERNAL_SYNC:		return FlashProgram::ExternalSync;
	default:								return FlashProgram::Undefined;
	}
}

unsigned int StillImageObject::FromFocusMetering(FocusMetering v)
{
	switch (v)
	{
	case FocusMetering::CenterSpot:			return WPD_FOCUS_METERING_MODE_CENTER_SPOT;
	case FocusMetering::MultiSpot:			return WPD_FOCUS_METERING_MODE_MULTI_SPOT;
	default:								return WPD_FOCUS_METERING_MODE_UNDEFINED;
	}
}

FocusMetering StillImageObject::ToFocusMetering(unsigned int v)
{
	switch (v)
	{
	case WPD_FOCUS_METERING_MODE_CENTER_SPOT:	return FocusMetering::CenterSpot;
	case WPD_FOCUS_METERING_MODE_MULTI_SPOT:	return FocusMetering::MultiSpot;
	default:									return FocusMetering::Undefined;
	}
}

unsigned int StillImageObject::FromFocusProgram(FocusProgram v)
{
	switch (v)
	{
	case FocusProgram::Manual:				return WPD_FOCUS_MANUAL;
	case FocusProgram::Auto:				return WPD_FOCUS_AUTOMATIC;
	case FocusProgram::AutoMacro:			return WPD_FOCUS_AUTOMATIC_MACRO;
	default:								return WPD_FOCUS_UNDEFINED;
	}
}

FocusProgram StillImageObject::ToFocusProgram(unsigned int v)
{
	switch (v)
	{
	case WPD_FOCUS_MANUAL:					return FocusProgram::Manual;
	case WPD_FOCUS_AUTOMATIC:				return FocusProgram::Auto;
	case WPD_FOCUS_AUTOMATIC_MACRO:			return FocusProgram::AutoMacro;
	default:								return FocusProgram::Undefined;
	}
}

unsigned int StillImageObject::FromWhiteBalance(WhiteBalance v)
{
	switch (v)
	{
	case WhiteBalance::Manual:				return WPD_WHITE_BALANCE_MANUAL;
	case WhiteBalance::Automatic:			return WPD_WHITE_BALANCE_AUTOMATIC;
	case WhiteBalance::OnePushAutonatic:	return WPD_WHITE_BALANCE_ONE_PUSH_AUTOMATIC;
	case WhiteBalance::Daylight:			return WPD_WHITE_BALANCE_DAYLIGHT;
	case WhiteBalance::Tungsten:			return WPD_WHITE_BALANCE_TUNGSTEN;
	case WhiteBalance::Flash:				return WPD_WHITE_BALANCE_FLASH;
	default:								return WPD_WHITE_BALANCE_UNDEFINED;
	}
}

WhiteBalance StillImageObject::ToWhiteBalance(unsigned int v)
{
	switch (v)
	{
	case WPD_WHITE_BALANCE_MANUAL:			return WhiteBalance::Manual;
	case WPD_WHITE_BALANCE_AUTOMATIC:		return WhiteBalance::Automatic;
	case WPD_WHITE_BALANCE_ONE_PUSH_AUTOMATIC:	return WhiteBalance::OnePushAutonatic;
	case WPD_WHITE_BALANCE_DAYLIGHT:		return WhiteBalance::Daylight;
	case WPD_WHITE_BALANCE_TUNGSTEN:		return WhiteBalance::Tungsten;
	case WPD_WHITE_BALANCE_FLASH:			return WhiteBalance::Flash;
	default:								return WhiteBalance::Undefined;
	}
}

}