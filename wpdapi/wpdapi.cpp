// wpdapi.cpp

#include "wpdapi.h"

bool matchDeviceCategory(const GUID &guidCat, WPD::FunctionCategory enumCat)
{
	if (enumCat == WPD::FunctionCategory::Any)
		return true;

	if ((IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_STORAGE, guidCat) && enumCat == WPD::FunctionCategory::Storage) ||
		(IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_STILL_IMAGE_CAPTURE, guidCat) && enumCat == WPD::FunctionCategory::StillImageCapture) ||
		(IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_AUDIO_CAPTURE, guidCat) && enumCat == WPD::FunctionCategory::AudioCapture) ||
		(IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_SMS, guidCat) && enumCat == WPD::FunctionCategory::SMS) ||
		(IsEqualGUID(WPD_FUNCTIONAL_CATEGORY_RENDERING_INFORMATION, guidCat) && enumCat == WPD::FunctionCategory::RenderingInformation))
		return true;

	return false;
}

const GUID &guidFromCategory(WPD::FunctionCategory enumCat)
{
	switch (enumCat)
	{
	case WPD::FunctionCategory::Any:
		return WPD_FUNCTIONAL_CATEGORY_ALL;
	case WPD::FunctionCategory::AudioCapture:
		return WPD_FUNCTIONAL_CATEGORY_AUDIO_CAPTURE;
	case WPD::FunctionCategory::RenderingInformation:
		return WPD_FUNCTIONAL_CATEGORY_RENDERING_INFORMATION;
	case WPD::FunctionCategory::SMS:
		return WPD_FUNCTIONAL_CATEGORY_SMS;
	case WPD::FunctionCategory::StillImageCapture:
		return WPD_FUNCTIONAL_CATEGORY_STILL_IMAGE_CAPTURE;
	case WPD::FunctionCategory::Storage:
		return WPD_FUNCTIONAL_CATEGORY_STORAGE;
	default:
		return WPD_FUNCTIONAL_CATEGORY_ALL;
	}
}