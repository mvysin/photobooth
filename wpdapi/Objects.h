// Objects.h

#pragma once

namespace WPD {

	ref class StorageObject;

	public ref class StorageItem
	{
		StorageObject ^obj;
		StorageItem ^parent;
		String ^id;
		IList<StorageItem^> ^children;

	internal:
		StorageItem(StorageObject ^obj, StorageItem ^parent, String ^id);
	
	public:

		property String ^Id {
			String ^get() { return id; }
		}

		property IList<StorageItem^>^ Items {
			IList<StorageItem^> ^get();
		}

		property String^ Name {
			String ^get();
		}

		property StorageObject ^ParentObject {
			StorageObject ^get() { return obj; }
		}

		property StorageItem ^ParentItem {
			StorageItem ^get() { return parent; }
		}

		void TransferTo(System::IO::MemoryStream ^ms);
	};

	public ref class StorageObject : FunctionalObject
	{
		IList<StorageItem^> ^children;

	internal:
		StorageObject(PortableDeviceCategory ^cat, String ^id);

	public:
		~StorageObject();
		!StorageObject();

		property IList<StorageItem^>^ Items {
			IList<StorageItem^> ^get();
		}

		void Refresh();
	};





	public enum class CaptureProgram
	{
		Undefined,
		Normal,
		Burst,
		Timelapse
	};

	public enum class ExposureMetering
	{
		Undefined,
		Average,
		CenterWeightedAverage,
		MultiSpot,
		CenterSpot
	};

	public enum class ExposureProgram
	{
		Undefined,
		Manual,
		Auto,
		AperturePriority,
		ShutterPriority,
		Creative,
		Action,
		Portrait
	};

	public enum class FlashProgram
	{
		Undefined,
		Auto,
		Off,
		Fill,
		RedEyeAuto,
		RedEyeFill,
		ExternalSync
	};

	public enum class FocusMetering
	{
		Undefined,
		CenterSpot,
		MultiSpot
	};

	public enum class FocusProgram
	{
		Undefined,
		Manual,
		Auto,
		AutoMacro
	};

	public enum class WhiteBalance
	{
		Undefined,
		Manual,
		Automatic,
		OnePushAutonatic,
		Daylight,
		Tungsten,
		Flash
	};

	public ref class StillImageObject : FunctionalObject
	{
		IPortableDeviceProperties *pProps;

		CaptureProgram ToCaptureProgram(unsigned int v);
		ExposureMetering ToExposureMetering(unsigned int v);
		ExposureProgram ToExposureProgram(unsigned int v);
		FlashProgram ToFlashProgram(unsigned int v);
		FocusMetering ToFocusMetering(unsigned int v);
		FocusProgram ToFocusProgram(unsigned int v);
		WhiteBalance ToWhiteBalance(unsigned int v);

		unsigned int FromCaptureProgram(CaptureProgram v);
		unsigned int FromExposureMetering(ExposureMetering v);
		unsigned int FromExposureProgram(ExposureProgram v);
		unsigned int FromFlashProgram(FlashProgram v);
		unsigned int FromFocusMetering(FocusMetering v);
		unsigned int FromFocusProgram(FocusProgram v);
		unsigned int FromWhiteBalance(WhiteBalance v);

		void Open();

		unsigned int PropGetUI4(const PROPERTYKEY id);
		int PropGetI4(const PROPERTYKEY id);

		void PropSetUI4(const PROPERTYKEY id, unsigned int value);
		void PropSetI4(const PROPERTYKEY id, int value);

	internal:
		StillImageObject(PortableDeviceCategory ^cat, String ^id);

	public:
		~StillImageObject();
		!StillImageObject();

		void Capture();

		property int BurstInterval {
			int get() { return PropGetI4(WPD_STILL_IMAGE_BURST_INTERVAL); }
			void set(int value) { PropSetI4(WPD_STILL_IMAGE_BURST_INTERVAL, value); }
		}
		property int BurstNumber {
			int get() { return PropGetI4(WPD_STILL_IMAGE_BURST_NUMBER); }
			void set(int value) { PropSetI4(WPD_STILL_IMAGE_BURST_NUMBER, value); }
		}
		property int CaptureDelay {
			int get() { return PropGetI4(WPD_STILL_IMAGE_CAPTURE_DELAY); }
			void set(int value) { PropSetI4(WPD_STILL_IMAGE_CAPTURE_DELAY, value); }
		}
		property CaptureProgram CaptureMode {
			CaptureProgram get() { return ToCaptureProgram(PropGetUI4(WPD_STILL_IMAGE_CAPTURE_MODE)); }
			void set(CaptureProgram value) { PropSetUI4(WPD_STILL_IMAGE_CAPTURE_MODE, FromCaptureProgram(value)); }
		}
		property int ExposureBias {
			int get() { return PropGetI4(WPD_STILL_IMAGE_EXPOSURE_BIAS_COMPENSATION); }
			void set(int value) { PropSetI4(WPD_STILL_IMAGE_EXPOSURE_BIAS_COMPENSATION, value); }
		}
		property int ExposureISO {
			int get() { return (int)PropGetUI4(WPD_STILL_IMAGE_EXPOSURE_INDEX); }
			void set(int value) { PropSetUI4(WPD_STILL_IMAGE_EXPOSURE_INDEX, (unsigned int)value); }
		}
		property ExposureMetering ExposureMeteringMode {
			ExposureMetering get() { return ToExposureMetering(PropGetUI4(WPD_STILL_IMAGE_EXPOSURE_METERING_MODE)); }
			void set(ExposureMetering value) { PropSetUI4(WPD_STILL_IMAGE_EXPOSURE_METERING_MODE, FromExposureMetering(value)); }
		}
		property ExposureProgram ExposureProgramMode {
			ExposureProgram get() { return ToExposureProgram(PropGetUI4(WPD_STILL_IMAGE_EXPOSURE_PROGRAM_MODE)); }
			void set(ExposureProgram value) { PropSetUI4(WPD_STILL_IMAGE_EXPOSURE_PROGRAM_MODE, FromExposureProgram(value)); }
		}
		property int ExposureTime {
			int get() { return (int)PropGetUI4(WPD_STILL_IMAGE_EXPOSURE_TIME); }
			void set(int value) { PropSetUI4(WPD_STILL_IMAGE_EXPOSURE_TIME, (unsigned int)value); }
		}
		property FlashProgram FlashMode {
			FlashProgram get() { return ToFlashProgram(PropGetUI4(WPD_STILL_IMAGE_FLASH_MODE)); }
			void set(FlashProgram value) { PropSetUI4(WPD_STILL_IMAGE_FLASH_MODE, FromFlashProgram(value)); }
		}
		property int FNumber {
			int get() { return (int)PropGetUI4(WPD_STILL_IMAGE_FNUMBER); }
			void set(int value) { PropSetUI4(WPD_STILL_IMAGE_FNUMBER, (unsigned int)value); }
		}
		property FocusMetering FocusMeteringMode {
			FocusMetering get() { return ToFocusMetering(PropGetUI4(WPD_STILL_IMAGE_FOCUS_METERING_MODE)); }
			void set(FocusMetering value) { PropSetUI4(WPD_STILL_IMAGE_FOCUS_METERING_MODE, FromFocusMetering(value)); }
		}
		property FocusProgram FocusMode {
			FocusProgram get() { return ToFocusProgram(PropGetUI4(WPD_STILL_IMAGE_FOCUS_MODE)); }
			void set(FocusProgram value) { PropSetUI4(WPD_STILL_IMAGE_FOCUS_MODE, FromFocusProgram(value)); }
		}
		property WhiteBalance WhiteBalanceMode {
			WhiteBalance get() { return ToWhiteBalance(PropGetUI4(WPD_STILL_IMAGE_WHITE_BALANCE)); }
			void set(WhiteBalance value) { PropSetUI4(WPD_STILL_IMAGE_WHITE_BALANCE, FromWhiteBalance(value)); }
		}
	};

}