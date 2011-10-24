// Device.h

using namespace System;
using namespace System::Collections::Generic;

namespace WPD
{
	ref class PortableDeviceManager;
	ref class PortableDeviceCategory;
	ref class FunctionalObject;

	public enum class OperationState
	{
		Unspecified,
		Started,
		Running,
		Paused,
		Cancelled,
		Finished,
		Aborted
	};
	
	public ref class PortableDeviceEventArgs : public EventArgs
	{
	public:
		property bool Autoplay;
		property bool Broadcast;
		property bool ChildHierarchyChanged;
		property String ^ObjectCreationCookie;
		property String ^ObjectParentId;
		property int Progress;
		property OperationState State;

		virtual String ^ToString() override {
			System::Text::StringBuilder ^sb = gcnew System::Text::StringBuilder();
			sb->AppendFormat("Autoplay: {0}\n", Autoplay);
			sb->AppendFormat("Broadcast: {0}\n", Broadcast);
			sb->AppendFormat("ChildHierarchyChanged: {0}\n", ChildHierarchyChanged);
			sb->AppendFormat("ObjectCreationCookie: {0}\n", ObjectCreationCookie);
			sb->AppendFormat("ObjectParentId: {0}\n", ObjectParentId);
			sb->AppendFormat("Progress: {0}\n", Progress);
			sb->AppendFormat("State: {0}\n", State);

			return sb->ToString();
		}
	};

	public delegate void PortableDeviceEventHandler(Object ^sender, PortableDeviceEventArgs ^e);


	public ref class PortableDevice
	{
		IPortableDevice *pDev;
		bool isReadOnly;
		LPWSTR eventCookie;
		EventSink *eventSink;

		PortableDeviceManager ^mgr;
		String ^deviceId, ^name, ^manuf, ^desc;
		List<PortableDeviceCategory^>^ cats;

		FunctionCategory CategoryFromGUID(const GUID &guid);

		void Open(bool readOnly);

	internal:
		PortableDevice(PortableDeviceManager^, LPWSTR);

		property IPortableDevice *Handle {
			IPortableDevice *get() { return pDev; }
		}

		HRESULT OnEvent(IPortableDeviceValues *params);


	protected:
		virtual void OnDeviceCapabilitiesUpdated(PortableDeviceEventArgs ^e);
		virtual void OnDeviceRemoved(PortableDeviceEventArgs ^e);
		virtual void OnDeviceReset(PortableDeviceEventArgs ^e);
		virtual void OnObjectAdded(PortableDeviceEventArgs ^e);
		virtual void OnObjectRemoved(PortableDeviceEventArgs ^e);
		virtual void OnObjectTransferRequested(PortableDeviceEventArgs ^e);
		virtual void OnObjectUpdated(PortableDeviceEventArgs ^e);
		virtual void OnStorageFormat(PortableDeviceEventArgs ^e);


	public:
		~PortableDevice();
		!PortableDevice();

		event PortableDeviceEventHandler^ DeviceCapabilitiesUpdated;
		event PortableDeviceEventHandler^ DeviceRemoved;
		event PortableDeviceEventHandler^ DeviceReset;
		event PortableDeviceEventHandler^ ObjectAdded;
		event PortableDeviceEventHandler^ ObjectRemoved;
		event PortableDeviceEventHandler^ ObjectTransferRequested;
		event PortableDeviceEventHandler^ ObjectUpdated;
		event PortableDeviceEventHandler^ StorageFormat;

		property IList<PortableDeviceCategory^>^ Categories {
			IList<PortableDeviceCategory^>^ get();
		}

		property String^ Description {
			String ^get() { return desc; }
		}

		PortableDeviceCategory ^GetCategory(FunctionCategory cat);

		property String^ Id {
			String ^get() { return deviceId; }
		}

		property String^ Manufacturer {
			String ^get() { return manuf; }
		}

		property String^ Name {
			String ^get() { return name; }
		}

		virtual String ^ToString() override
		{
			return Name;
		}
	};

	public ref class PortableDeviceCategory
	{
		FunctionCategory cat;
		PortableDevice ^dev;
		Guid ^guid;
		List<FunctionalObject^> ^objs;

	internal:
		PortableDeviceCategory(PortableDevice ^dev, Guid ^guid, FunctionCategory cat);

	public:

		property FunctionCategory Category {
			FunctionCategory get() { return cat; }
		}

		property PortableDevice^ Device {
			PortableDevice ^get() { return dev; }
		}

		property IList<FunctionalObject^>^ Functions {
			IList<FunctionalObject^>^ get();
		}

		property Guid^ Id {
			Guid ^get() { return guid; }
		}
	};


	public ref class FunctionalObject
	{
		String^ id;
		PortableDeviceCategory ^cat;

	internal:
		FunctionalObject(PortableDeviceCategory ^cat, String ^str)
		{
			this->cat = cat;
			this->id = str;
		}

	public:
		property PortableDeviceCategory^ Category {
			PortableDeviceCategory ^get() { return cat; }
		}

		property PortableDevice^ Device {
			PortableDevice ^get() { return Category->Device; }
		}

		property String^ Id {
			String ^get() { return id; }
		}
	};
}